/* -*- mode: vala; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 * Copyright (C) 2010-2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 *             Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */
using Dee;
using Zeitgeist;
using Zeitgeist.Timestamp;
using Config;
using Gee;
using GMenu;

namespace Unity.ApplicationsLens {

  /* Number of 'Apps available for download' to show if no search query is provided AND a filter is active.
     It shouldn't be too high as this may impact lens performance.
   */
  const uint MAX_APP_FOR_DOWNLOAD_FOR_EMPTY_QUERY = 100;

  /* Number of top rated apps to show in 'Apps available for download' if no search query is provided AND NO filter is active. */
  const uint MAX_TOP_RATED_APPS_FOR_EMPTY_QUERY = 12;

  /* Number of "What's new" apps to show in 'Apps available for download' if no search query is provided AND NO filter is active. */
  const uint MAX_WHATS_NEW_APPS_FOR_EMPTY_QUERY = 10;

  /* Time between queries to SoftwareCenterDataProvider */
  const int64 TOP_RATED_ITEMS_CACHE_LIFETIME = 24*3600; // 24 hours

  const string ICON_PATH = Config.DATADIR + "/icons/unity-icon-theme/places/svg/";
  const string GENERIC_APP_ICON = "applications-other";

  const string LIBUNITY_SCHEMA = "com.canonical.Unity.Lenses";

  private class ApplicationsScope : Unity.AbstractScope
  {
    public Zeitgeist.Log log;
    public Zeitgeist.Index zg_index;
    private Zeitgeist.Monitor monitor;

    public Map<string, int> popularity_map;
    public bool popularities_dirty;

    /* The searcher for online material may be null if it fails to load
     * the Xapian index from the Software Center */
    public Unity.Package.Searcher? pkgsearcher;
    public Unity.Package.Searcher appsearcher;

    /* Read the app ratings dumped by the Software Center */
    private bool ratings_db_initialized = false;
    public Unity.Ratings.Database? ratings = null;

    /* Support aptd dbus interface; created when application install/remove was requested by preview action */
    private AptdProxy aptdclient;
    private AptdTransactionProxy aptd_transaction;

    public SoftwareCenterUtils.MangledDesktopFileLookup sc_mangler;

    /* Used for adding launcher icon on app installation from the preview */
    private LauncherProxy launcherservice;

    /* Desktop file & icon name for unity-install:// install candidate displayed in last preview; we store
     * them here to avoid extra query for app details if app install action is activated */
    public string preview_installable_desktop_file;
    public string preview_installable_icon_file;

    public string preview_developer_website;

    /* SoftwareCenter data provider used for app preview details */
    public SoftwareCenterDataProviderProxy sc_data_provider;

    public Unity.ApplicationsLens.CommandsScope commands_scope;

    private Gee.List<string> image_extensions;
    private HashTable<string,Icon> file_icon_cache;

    /* Monitor the favorite apps in the launcher, so we can filter them
     * out of the results for Recent Apps */
    public Unity.LauncherFavorites favorite_apps;
    public AppWatcher app_watcher;

    public PtrArray zg_templates;

    /* Gnome menu structure - also used to check whether apps are installed */
    private uint app_menu_changed_reindex_timeout = 0;
    private GMenu.Tree app_menu = null;

    private GLib.Regex? uri_regex;
    private GLib.Regex  mountable_regex;

    private Settings gp_settings;

    private const string DISPLAY_RECENT_APPS_KEY = "display-recent-apps";
    private const string DISPLAY_AVAILABLE_APPS_KEY = "display-available-apps";

    public bool display_recent_apps { get; set; default = true; }
    public bool display_available_apps { get; set; default = true; }
    public bool force_small_icons_for_suggestions { get; set; default = true; }

    public PurchaseInfoHelper purchase_info = null;

    construct
    {
      populate_zg_templates ();

      log = new Zeitgeist.Log();
      zg_index = new Zeitgeist.Index();
      monitor = new Zeitgeist.Monitor (new Zeitgeist.TimeRange.from_now (),
                                       zg_templates);
      monitor.events_inserted.connect (mark_dirty);
      monitor.events_deleted.connect (mark_dirty);
      log.install_monitor (monitor);

      popularity_map = new HashMap<string, int> ();
      popularities_dirty = true;
      // refresh the popularities every now and then
      Timeout.add_seconds (30 * 60, () =>
      {
        popularities_dirty = true;
        return true;
      });

      this.gp_settings = new Settings ("com.canonical.Unity.ApplicationsLens");
      this.gp_settings.bind(DISPLAY_RECENT_APPS_KEY, this, "display_recent_apps", SettingsBindFlags.GET);
      this.gp_settings.bind(DISPLAY_AVAILABLE_APPS_KEY, this, "display_available_apps", SettingsBindFlags.GET);

      pkgsearcher = new Unity.Package.Searcher ();
      if (pkgsearcher == null)
      {
        critical ("Failed to load Software Center index. 'Apps Available for Download' will not be listed");
      }

      /* Image file extensions in order of popularity */
      image_extensions = new Gee.ArrayList<string> ();
      image_extensions.add ("png");
      image_extensions.add ("xpm");
      image_extensions.add ("svg");
      image_extensions.add ("tiff");
      image_extensions.add ("ico");
      image_extensions.add ("tif");
      image_extensions.add ("jpg");

      build_app_menu_index ();

      file_icon_cache = new HashTable<string,Icon>(str_hash, str_equal);
      sc_mangler = new SoftwareCenterUtils.MangledDesktopFileLookup ();

      /* Listen for changes in the installed applications */
      AppInfoManager.get_default().changed.connect (mark_dirty);

      /* Now start the RunEntry */
      commands_scope = new Unity.ApplicationsLens.CommandsScope (this);

      try {
        uri_regex = new GLib.Regex ("^[a-z]+:.+$");
        mountable_regex = new GLib.Regex ("((ftp|ssh|sftp|smb|dav)://).+");
      } catch (GLib.RegexError e) {
        uri_regex = null;
        critical ("Failed to compile URI regex. URL launching will be disabled");
      }

      favorite_apps = Unity.LauncherFavorites.get_default ();
      favorite_apps.changed.connect(mark_dirty);

      app_watcher = new AppWatcher ();
      app_watcher.running_applications_changed.connect (mark_dirty);

      aptdclient = new AptdProxy ();
      launcherservice = new LauncherProxy ();
    }

    public void init_ratings_db ()
    {
      if (ratings_db_initialized) return;
      try
      {
        ratings = new Unity.Ratings.Database ();
      }
      catch (FileError e)
      {
        warning ("%s", e.message);
        ratings = null;
      }
      ratings_db_initialized = true;
    }

    public override string get_group_name ()
    {
      return "com.canonical.Unity.Scope.Applications";
    }

    public override string get_unique_name ()
    {
      return "/com/canonical/unity/scope/applications";
    }

    public override Unity.CategorySet get_categories ()
    {
      var categories = new Unity.CategorySet ();
      File icon_dir = File.new_for_path (ICON_PATH);

      var cat = new Unity.Category ("apps", _("Applications"),
                                    new FileIcon (icon_dir.get_child ("group-apps.svg")));
      categories.add (cat);

      cat = new Unity.Category ("recently-used", _("Recently used"),
                                new FileIcon (icon_dir.get_child ("group-recent.svg")));
      categories.add (cat);

      cat = new Unity.Category ("recent", _("Recent apps"),
                                new FileIcon (icon_dir.get_child ("group-apps.svg")));
      categories.add (cat);

      cat = new Unity.Category ("installed", _("Installed"),
                                new FileIcon (icon_dir.get_child ("group-installed.svg")));
      categories.add (cat);

      cat = new Unity.Category ("more", _("More suggestions"),
                                new FileIcon (icon_dir.get_child ("group-treat-yourself.svg")));
      categories.add (cat);

      return categories;
    }

    public override Unity.FilterSet get_filters()
    {
      var filters = new Unity.FilterSet ();

      /* Type filter */
      {
        var filter = new CheckOptionFilter ("type", _("Type"));
        filter.sort_type = Unity.OptionsFilter.SortType.DISPLAY_NAME;

        filter.add_option ("accessories", _("Accessories"));
        filter.add_option ("education", _("Education"));
        filter.add_option ("game", _("Games"));
        filter.add_option ("graphics", _("Graphics"));
        filter.add_option ("internet", _("Internet"));
        filter.add_option ("fonts", _("Fonts"));
        filter.add_option ("office", _("Office"));
        filter.add_option ("media", _("Media"));
        filter.add_option ("customization", _("Customization"));
        filter.add_option ("accessibility", _("Accessibility"));
        filter.add_option ("developer", _("Developer"));
        filter.add_option ("science-and-engineering", _("Science & engineering"));
        filter.add_option ("scopes", _("Dash plugins"));
        filter.add_option ("system", _("System"));

        filters.add (filter);
      }

      return filters;
    }

    public override Unity.Schema get_schema ()
    {
      var schema = new Unity.Schema ();
      return schema;
    }

    public override string get_search_hint ()
    {
      return _("Search applications");
    }

    public override string normalize_search_query (string search_query)
    {
      return search_query.strip ();
    }

    public override Unity.ScopeSearchBase create_search_for_query (Unity.SearchContext search_context)
    {
      return new ApplicationsSearch (this, search_context);
    }

    public override Unity.ResultPreviewer create_previewer (Unity.ScopeResult result, Unity.SearchMetadata metadata)
    {
      return new ApplicationsResultPreviewer (this, result, metadata);
    }

    public override Unity.ActivationResponse? activate (Unity.ScopeResult result, Unity.SearchMetadata metadata, string? action_id)
    {
      if (action_id == "buy")
        return start_software_center (result.uri);
      else if (action_id == "install")
        return app_preview_install (result.uri);
      else if (action_id == "install-paid")
        return start_software_center (result.uri);
      else if (action_id == "uninstall")
        return app_preview_uninstall (result.uri);
      else if (action_id == "website")
        return app_preview_website (result.uri);
      else
        return app_launch (result, metadata);
    }

    private Unity.ActivationResponse app_launch (Unity.ScopeResult result, Unity.SearchMetadata metadata)
    {
      string[] args;
      string exec_or_dir = null;
      if (result.uri.has_prefix ("unity-install://"))
      {
        var prv = create_previewer (result, metadata).run ();
        if (prv is Unity.Preview)
        {
          return new Unity.ActivationResponse.with_preview (
            prv as Unity.Preview);
        }
        else
        {
          warning ("Failed to generate preview for %s", result.uri);
          return new Unity.ActivationResponse (Unity.HandledType.NOT_HANDLED);
        }
      }
      else if (result.uri.has_prefix (UNITY_RUNNER_PREFIX))
      {
        string orig;
        orig = result.uri.offset (UNITY_RUNNER_PREFIX.length);
        if (orig.has_prefix("\\\\"))
          orig = orig.replace ("\\\\","smb://");
        if (uri_regex != null && uri_regex.match (orig)) {
          try {
            /* this code ensures that a file manager will be used
             * if uri it's a remote location that should be mounted */
            if (mountable_regex.match (orig)) {
              var muris = new GLib.List<string>();
              muris.prepend (orig);
              var file_manager = AppInfo.get_default_for_type("inode/directory", true);
              file_manager.launch_uris(muris,null);
            } else {
              AppInfo.launch_default_for_uri (orig, null);
            }
          } catch (GLib.Error error) {
            warning ("Failed to launch URI %s", orig);
            return new Unity.ActivationResponse(Unity.HandledType.NOT_HANDLED);
          }
          return new Unity.ActivationResponse(Unity.HandledType.HIDE_DASH);

        } else {
          exec_or_dir = Utils.subst_tilde (orig);
          args = exec_or_dir.split (" ", 0);
          for (int i = 0; i < args.length; i++)
            args[i] = Utils.subst_tilde (args[i]);
        }
        this.commands_scope.add_history (orig);
      }
      else
      {
        /* Activation of standard application:// uris */

        /* Make sure fresh install learns quickly */
        if (popularity_map.size <= 5) popularities_dirty = true;

        return new Unity.ActivationResponse(Unity.HandledType.NOT_HANDLED);
      }

      if ((exec_or_dir != null) && FileUtils.test (exec_or_dir, FileTest.IS_DIR))
      {
        try {
          AppInfo.launch_default_for_uri ("file://" + exec_or_dir, null);
        } catch (GLib.Error err) {
          warning ("Failed to open current folder '%s' in file manager: %s",
                   exec_or_dir, err.message);
          return new Unity.ActivationResponse(Unity.HandledType.NOT_HANDLED);
        }
      }
      else
      {
        try {
          unowned string home_dir = GLib.Environment.get_home_dir ();
          Process.spawn_async (home_dir, args, null, SpawnFlags.SEARCH_PATH, null, null);
        } catch (SpawnError e) {
          warning ("Failed to spawn software-center or direct URI activation '%s': %s",
                   result.uri, e.message);
          return new Unity.ActivationResponse(Unity.HandledType.NOT_HANDLED);
        }
      }

      return new Unity.ActivationResponse(Unity.HandledType.HIDE_DASH);
    }

    private async void call_install_packages (string package_name, out string tid) throws IOError
    {
      tid = yield aptdclient.install_packages ({package_name});
    }

    private async void call_remove_packages (string package_name, out string tid) throws IOError
    {
      tid = yield aptdclient.remove_packages ({package_name});
    }

    /**
     * Handler for free apps installation.
     * Triggers package installation via apt-daemon DBus service
     */
    private Unity.ActivationResponse app_preview_install (string uri)
    {
      if (uri.has_prefix ("unity-install://"))
      {
        string app = uri.substring (16); // trim "unity-install://"
        string[] parts = app.split ("/");

        if (parts.length > 1)
        {
          string pkgname = parts[0];
          string appname = parts[1];
          try
          {
            aptdclient.connect_to_aptd ();
          }
          catch (IOError e)
          {
            warning ("Failed to connect to aptd: '%s'", e.message);
            return new Unity.ActivationResponse(Unity.HandledType.NOT_HANDLED);
          }
          call_install_packages.begin (pkgname, (obj, res) =>
          {
            try {
              string tid;
              call_install_packages.end (res, out tid);
              debug ("transaction started: %s, pkg: %s\n", tid, pkgname);
              aptd_transaction = new AptdTransactionProxy ();
              aptd_transaction.connect_to_aptd (tid);
              aptd_transaction.simulate ();
              aptd_transaction.run ();

              launcherservice.connect_to_launcher ();
              string desktop_file = preview_installable_desktop_file;
              Icon icon = find_pkg_icon (null, preview_installable_icon_file);
              launcherservice.add_launcher_item_from_position.begin (appname, icon.to_string (), 0, 0, 32, desktop_file, tid);
            }
            catch (IOError e)
            {
              warning ("Package '%s' installation failed: %s", pkgname, e.message);
            }
          });
        }
        else
        {
          warning ("Bad install uri: '%s'", uri);
        }
      }
      else
      {
        warning ("Can't handle '%s' in app_preview_install handler", uri);
        return new Unity.ActivationResponse(Unity.HandledType.NOT_HANDLED);
      }
      return new Unity.ActivationResponse(Unity.HandledType.HIDE_DASH);
    }

    private Unity.ActivationResponse start_software_center (string uri)
    {
      unowned string pkg = uri.offset (16); // strip off "unity-install://" prefix
      debug ("Installing: %s", pkg);

      var args = new string[2];
      args[0] = "software-center";
      args[1] = pkg;

      try
      {
        Process.spawn_async (GLib.Environment.get_home_dir (), args, null, SpawnFlags.SEARCH_PATH, null, null);
      }
      catch (SpawnError e)
      {
        warning ("Failed to spawn software-center for uri '%s': %s", uri, e.message);
        return new Unity.ActivationResponse(Unity.HandledType.NOT_HANDLED);
      }

      return new Unity.ActivationResponse(Unity.HandledType.HIDE_DASH);
    }

    private Unity.ActivationResponse app_preview_website (string uri)
    {
      try
      {
        AppInfo.launch_default_for_uri (preview_developer_website, null);
        return new Unity.ActivationResponse(Unity.HandledType.HIDE_DASH);
      }
      catch (Error e)
      {
        warning ("Failed to launch a web browser for uri '%s': '%s'", uri, e.message);
      }
      return new Unity.ActivationResponse(Unity.HandledType.NOT_HANDLED);
    }

    private Unity.ActivationResponse app_preview_uninstall (string uri)
    {
      if (uri.has_prefix ("application://"))
      {
        string desktopfile = uri.substring (14); // trim "application://"

        // de-mangle desktop file names back to what S-C expects
        if (sc_mangler.contains (desktopfile))
        {
          desktopfile = sc_mangler.get (desktopfile);
        }

        var pkginfo = pkgsearcher.get_by_desktop_file (desktopfile);

        if (pkginfo != null && pkginfo.package_name != null)
        {
          try
          {
            aptdclient.connect_to_aptd ();
          }
          catch (IOError e)
          {
            warning ("Failed to connect to aptd: '%s'", e.message);
            return new Unity.ActivationResponse(Unity.HandledType.NOT_HANDLED);
          }

          call_remove_packages.begin (pkginfo.package_name, (obj, res) =>
          {
            try {
              string tid;
              call_remove_packages.end (res, out tid);
              debug ("transaction started: %s, pkg: %s\n", tid, pkginfo.package_name);
              aptd_transaction = new AptdTransactionProxy ();
              aptd_transaction.connect_to_aptd (tid);
              aptd_transaction.simulate ();
              aptd_transaction.run ();
            }
            catch (IOError e)
            {
              warning (@"Package '$(pkginfo.package_name)' removal failed: $(e.message)");
            }
          });
          return new Unity.ActivationResponse(Unity.HandledType.HIDE_DASH);
        }
        else
        {
          warning (@"Cannot find package info for $uri");
        }
      }
      warning (@"Can't handle '%s' in app_preview_uninstall handler", uri);
      return new Unity.ActivationResponse(Unity.HandledType.NOT_HANDLED);
    }

    /* Load xdg menu info and build a Xapian index over it.
     * Do throttled re-index if the menu changes */
    private bool build_app_menu_index ()
    {
      if (app_menu == null)
      {
        debug ("Building initial application menu");

        /* We need INCLUDE_NODISPLAY to employ proper de-duping between
         * the Installed and Availabale categorys. If a NoDisplay app is installed,
         * eg. Evince, it wont otherwise be in the menu index, only in the
         * S-C index - thus show up in the Available category */
        app_menu = new GMenu.Tree ("unity-lens-applications.menu",
                                        GMenu.TreeFlags.INCLUDE_NODISPLAY);

        app_menu.changed.connect (() => {
            /* Reschedule the timeout if we already have one. The menu tree triggers
             * many change events during app installation. This way we wait the full
             * delay *after* the last change event has triggered */
            if (app_menu_changed_reindex_timeout != 0)
            {
              Source.remove (app_menu_changed_reindex_timeout);
            }

            app_menu_changed_reindex_timeout = Timeout.add_seconds (5, build_app_menu_index_and_result_models);
          });
      }

      // gnome menu tree needs to be loaded on startup and after receiving 'changed' signal - see gmenu-tree.c in gnome-menus-3.6.0.
      try
      {
        app_menu.load_sync (); //FIXME: libgnome-menu doesn't provide async method (yet?)
      }
      catch (GLib.Error e)
      {
        warning ("Failed to load menu entries: %s", e.message);
      }

      debug ("Indexing application menu");
      appsearcher = new Unity.Package.Searcher.for_menu (app_menu);
      app_menu_changed_reindex_timeout = 0;

      return false;
    }

    /* Called when our app_menu structure changes - probably because something
     * has been installed or removed. We reload gnome menu tree,
     * rebuild the index and update the result models for global and scope.
     * We need to update both because
     * we can't know exactly what Unity may be showing */
    private bool build_app_menu_index_and_result_models ()
    {
      build_app_menu_index ();

      mark_dirty ();

      return false;
    }

    private void populate_zg_templates ()
    {
      /* Create a template that activation of applications */
      zg_templates = new PtrArray.sized(1);
      var ev = new Zeitgeist.Event.full (ZG_ACCESS_EVENT, ZG_USER_ACTIVITY, "",
                             new Subject.full ("application*",
                                               "", //NFO_SOFTWARE,
                                               "",
                                               "", "", "", ""));
      zg_templates.add ((ev as GLib.Object).ref());
    }

    private void mark_dirty ()
    {
      results_invalidated (SearchType.DEFAULT);
      results_invalidated (SearchType.GLOBAL);
    }

    public async void update_popularities ()
    {
      try
      {
        // simulate a kind of frecency
        var end_ts = Timestamp.now ();
        var start_ts = end_ts - Timestamp.WEEK * 3;
        var rs = yield log.find_events (new TimeRange (start_ts, end_ts),
                                        zg_templates,
                                        StorageState.ANY,
                                        256,
                                        Zeitgeist.ResultType.MOST_POPULAR_SUBJECTS,
                                        null);

        // most popular apps must have high value, so unknown apps (where
        // popularity_map[app] == 0 aren't considered super popular
        int relevancy = 256;
        foreach (unowned Event event in rs)
        {
          for (int i = 0; i < event.num_subjects (); i++)
          {
            unowned string uri = event.get_subject (i).get_uri ();
            if (uri == null) continue;
            popularity_map[uri] = relevancy;
          }
          relevancy--;
        }
      }
      catch (GLib.Error err)
      {
        warning ("%s", err.message);
      }
    }

    public Icon find_pkg_icon (string? desktop_file, string icon_name)
    {
      if (desktop_file != null)
      {
        string desktop_id = Path.get_basename (desktop_file);
        bool installed = AppInfoManager.get_default().lookup (desktop_id) != null;

        /* If the app is already installed we should be able to pull the
         * icon from the theme */
        if (installed)
          return new ThemedIcon (icon_name);
      }

      /* App is not installed - we need to find the right icon in the bowels
       * of the software center */
      if (icon_name.has_prefix ("/"))
      {
        return new FileIcon (File.new_for_path (icon_name));
      }
      else
      {
        Icon icon = file_icon_cache.lookup (icon_name);

        if (icon != null)
          return icon;

        /* If the icon name contains a . it probably already have a
         * type postfix - so test icon name directly */
        string path;
        if ("." in icon_name)
        {
          path = @"$(Config.DATADIR)/app-install/icons/$(icon_name)";
          if (FileUtils.test (path, FileTest.EXISTS))
          {
            icon = new FileIcon (File.new_for_path (path));
            file_icon_cache.insert (icon_name, icon);
            return icon;
          }
          /* Try also software center cache dir */
          path = Path.build_filename (Environment.get_user_cache_dir (),
                                      "software-center",
                                      "icons",
                                      icon_name);
          if (FileUtils.test (path, FileTest.EXISTS))
          {
            icon = new FileIcon (File.new_for_path (path));
            file_icon_cache.insert (icon_name, icon);
            return icon;
          }
        }

        /* Now try appending all the image extensions we know */
        foreach (var ext in image_extensions)
        {
          path = @"$(Config.DATADIR)/app-install/icons/$(icon_name).$(ext)";
          if (FileUtils.test (path, FileTest.EXISTS))
          {
            /* Got it! Cache the icon path and return the icon */
            icon = new FileIcon (File.new_for_path (path));
            file_icon_cache.insert (icon_name, icon);
            return icon;
          }
        }
      }

      /* Cache the fact that we couldn't find this icon */
      var icon = new ThemedIcon (GENERIC_APP_ICON);
      file_icon_cache.insert (icon_name, icon);

      return icon;
    }

    public async SoftwareCenterData.AppDetailsData get_app_details (string appname, string pkgname) throws Error
    {
      if (sc_data_provider == null)
      {
        sc_data_provider = new SoftwareCenterDataCache (TOP_RATED_ITEMS_CACHE_LIFETIME);
      }

      debug ("Requesting pkg info: %s, %s\n", pkgname, appname);
      return yield sc_data_provider.get_app_details (appname, pkgname);
    }

    public async bool get_version_and_screenshot (string app_id, out string version, out string screenshot)
    {
      version = null;
      screenshot = null;
      // de-mangle desktop file names back to what S-C expects
      string mangled_id;
      if (sc_mangler.contains (app_id))
      {
        mangled_id = sc_mangler.get (app_id);
      }
      else
      {
        mangled_id = app_id;
      }
      var pkginfo = pkgsearcher.get_by_desktop_file (mangled_id);
      if (pkginfo != null)
      {
        SoftwareCenterData.AppDetailsData? details;
        try {
          details = yield get_app_details(pkginfo.application_name,
                                          pkginfo.package_name);
        } catch (Error e) {
          return false;
        }
        if (details != null) {
          version = details.version;
          screenshot = details.screenshot;
          return true;
        }
      }
      return false;
    }
  }

  private class ApplicationsSearch : Unity.ScopeSearchBase
  {
    private ApplicationsScope scope;

    public ApplicationsSearch (ApplicationsScope scope, Unity.SearchContext search_context)
    {
      this.scope = scope;
      set_search_context (search_context);
    }

    public override void run ()
    {
      var ml = new MainLoop ();
      run_async (() => { ml.quit (); });
      ml.run ();
    }

    public override void run_async (Unity.ScopeSearchBaseCallback async_callback)
    {
      dispatch_search.begin (() =>
      {
        async_callback (this);
      });
    }

    private async void dispatch_search ()
    {
      var context = this.search_context;

      if (scope.popularities_dirty)
      {
        scope.popularities_dirty = false;
        // we're not passing the cancellable, cause cancelling this search
        // shouldn't cancel getting most popular apps
        yield scope.update_popularities ();
        if (context.cancellable.is_cancelled ()) return;
      }

      if (context.search_type == SearchType.DEFAULT)
        yield search_default ();
      else
        yield search_global ();
    }

    private bool local_apps_active ()
    {
      var filter = search_context.filter_state.get_filter_by_id ("unity-sources") as Unity.OptionsFilter;
      if (filter.filtering)
      {
        var option = filter.get_option ("local");
        return option == null || option.active;
      }
      return true;
    }

    /* Returns TRUE if application is NOT installed */
    public bool filter_cb (Unity.Package.PackageInfo pkginfo)
    {
      var appmanager = AppInfoManager.get_default();
      AppInfo? app = appmanager.lookup (pkginfo.desktop_file);
      return app == null;
    }

    private bool is_phablet_ui ()
    {
      var form_factor = search_context.search_metadata.form_factor;

      return form_factor == "phone" || form_factor == "tablet";
    }

    private async void search_default ()
    {
      var context = this.search_context;
      var result_set = context.result_set;
      /* We'll clear the model once we finish waiting for the dbus-call
       * to finish, to prevent flicker. */

      var search_string = context.search_query;
      debug ("Searching for: %s", search_string);

      var type_filter = context.filter_state.get_filter_by_id ("type") as OptionsFilter;

      string pkg_search_string = XapianUtils.prepare_pkg_search_string (search_string, type_filter);

      bool has_filter = (type_filter != null && type_filter.filtering);
      bool has_search = !Utils.is_search_empty (search_string);
      bool running_on_phablet = is_phablet_ui ();

      Timer timer = new Timer ();

      /* Even though the Installed apps search is super fast, we wait here
       * for the Most Popular search to finish, because otherwise we'll update
       * the Installed category too soon and this will cause flicker
       * in the Dash. (lp:868192) */

      Set<string> installed_uris = new HashSet<string> ();
      Set<string> available_uris = new HashSet<string> ();
      var appresults = scope.appsearcher.search (
        pkg_search_string, 0, Unity.Package.SearchType.PREFIX,
        has_search ?
          Unity.Package.Sort.BY_RELEVANCY :
          Unity.Package.Sort.BY_NAME);
      if (local_apps_active ())
      {
        if (has_search) resort_pkg_search_results (appresults);
        add_pkg_search_result (appresults, installed_uris, available_uris,
                               result_set, Category.INSTALLED,
                               0, running_on_phablet);
      }

      timer.stop ();
      debug ("Entry search listed %i Installed apps in %fms for query: %s",
             appresults.num_hits, timer.elapsed ()*1000, pkg_search_string);

      if (local_apps_active () && scope.display_recent_apps)
      {
        try
        {
          timer.start ();
          /* Ignore the search string, we want to keep displaying the same apps
           * in the recent category and just filter out those that don't match
           * the search query */
          var zg_search_string = XapianUtils.prepare_zg_search_string ("", type_filter);

          var results = yield scope.zg_index.search (
            zg_search_string,
            new Zeitgeist.TimeRange.anytime (),
            scope.zg_templates,
            0,
            20,
            Zeitgeist.ResultType.MOST_RECENT_SUBJECTS,
            context.cancellable.get_gcancellable ());

          append_events_with_category (results, result_set, Category.RECENT,
                                       false, 6, installed_uris);

          timer.stop ();
          debug ("Entry search found %u/%u Recently Used apps in %fms for query '%s'",
                 results.size (), results.estimated_matches (),
                 timer.elapsed()*1000, zg_search_string);

        } catch (IOError.CANCELLED ioe) {
          // no need to bother
          return;
        } catch (GLib.Error e) {
          warning ("Error performing search '%s': %s", search_string, e.message);
        }
      }

      result_set.flush ();

      scope.purchase_info = new PurchaseInfoHelper ();

      /* XXX:
      if (model.get_n_rows () == 0)
      {
        search.set_reply_hint ("no-results-hint",
          _("Sorry, there are no applications that match your search."));
      }
      */
    }

    private async void search_global ()
    {
      var context = this.search_context;
      var result_set = context.result_set;
      /*
       * In global search, with a non-empty search string, we collate all
       * hits under one Applications category
       */
      var search_string = context.search_query;

      if (Utils.is_search_empty (search_string))
      {
        yield update_global_without_search ();
        return;
      }

      bool running_on_phablet = is_phablet_ui ();

      var pkg_search_string = XapianUtils.prepare_pkg_search_string (search_string, null);
      Set<string> installed_uris = new HashSet<string> ();
      Set<string> available_uris = new HashSet<string> ();
      var timer = new Timer ();
      var appresults = scope.appsearcher.search (
        pkg_search_string, 0,
        Unity.Package.SearchType.PREFIX,
        Unity.Package.Sort.BY_RELEVANCY);
      resort_pkg_search_results (appresults);
      add_pkg_search_result (appresults, installed_uris, available_uris,
                             result_set, Category.APPLICATIONS, 0,
                             running_on_phablet);

      timer.stop ();
      debug ("Global search listed %i Installed apps in %fms for query: %s",
             appresults.num_hits, timer.elapsed ()*1000, pkg_search_string);
    }

    private async void update_global_without_search ()
    {
      /*
       * In global search, with an empty search string, we show just Recent Apps
       * Excluding apps with icons in the launcher (be they running or faves)
       */
      var context = this.search_context;
      var result_set = context.result_set;

      Timer timer = new Timer ();

      if (local_apps_active () && scope.display_recent_apps)
      {
        try
        {
          var zg_search_string = XapianUtils.prepare_zg_search_string ("",
                                                                       null);

          var time_range = new Zeitgeist.TimeRange.anytime ();
          var results = yield scope.log.find_events (
            time_range,
            scope.zg_templates,
            Zeitgeist.StorageState.ANY,
            40,
            Zeitgeist.ResultType.MOST_RECENT_SUBJECTS,
            context.cancellable.get_gcancellable ());

          append_events_with_category (results, result_set,
                                       Category.RECENT_APPS, false);

          timer.stop ();
          debug ("Entry search found %u/%u Recently Used apps in %fms for query '%s'",
                 results.size (), results.estimated_matches (),
                 timer.elapsed()*1000, zg_search_string);

        } catch (IOError.CANCELLED ioe) {
          // no need to bother
          return;
        } catch (GLib.Error e) {
          warning ("Error performing search '%s': %s",
                   search_context.search_query, e.message);
        }
      }
    }

    /*
     * Performs secondary level sorting of the results according to popularity
     * of individual desktop files.
     */
    private void resort_pkg_search_results (Unity.Package.SearchResult results)
    {
      results.results.sort_with_data ((a, b) =>
      {
        /* We'll cluster the relevancies into a couple of bins, because
         * there can be multiple terms adding to the relevancy (especially
         * when doing one/two character prefix searches - ie a "f*" search
         * will have slightly higher relevancy for item with name "Search
         * _f_or _f_iles" than "_F_irefox", and that's not what we want)
         */
        int rel_a = a.relevancy;
        int rel_b = b.relevancy;
        int delta = (rel_a - rel_b).abs ();
        if (delta < 10)
        {
          string id_a = scope.sc_mangler.extract_desktop_id (a.desktop_file);
          string id_b = scope.sc_mangler.extract_desktop_id (b.desktop_file);
          rel_a = scope.popularity_map["application://" + id_a];
          rel_b = scope.popularity_map["application://" + id_b];
        }
        return rel_b - rel_a; // we want higher relevancy first
      });
    }

    /**
     * Sanitize executable name -- make it suitable for Home Lens.
     */
    private static string sanitize_binary_name (string name)
    {
      return GLib.Path.get_basename (name);
    }

    private string get_annotated_icon (Icon app_icon, string price, bool paid,
                                       bool use_small_icon = true)
    {
      var annotated_icon = new AnnotatedIcon (app_icon);
      annotated_icon.category = CategoryType.APPLICATION;
      if (price != null && price != "")
      {
        if (paid)
          annotated_icon.ribbon = _("Paid");
        else
          annotated_icon.ribbon = price;
      }
      else
      {
        annotated_icon.ribbon = _("Free");
      }
      if (scope.force_small_icons_for_suggestions || use_small_icon
          || app_icon.to_string () == GENERIC_APP_ICON)
      {
        annotated_icon.size_hint = IconSizeHint.SMALL;
      }

      return annotated_icon.to_string ();
    }

    /**
     * Add all results obtained from SoftwareCenterDataProvider
     */
    private uint add_sc_category_results (SoftwareCenterData.AppInfo?[] results,
                                          Unity.ResultSet result_set,
                                          Category category,
                                          ref Set<string> duplicates_lookup,
                                          uint max_results)
    {
      uint i = 0;
      foreach (SoftwareCenterData.AppInfo app in results)
      {
        string uri = @"unity-install://$(app.package_name)/$(app.application_name)";
        if (uri in duplicates_lookup)
          continue;

        string icon_obj;
        Icon app_icon = scope.find_pkg_icon (app.desktop_file, app.icon);
        var pinfo = scope.purchase_info.find (
          app.application_name, app.package_name);
        if (pinfo != null)
        {
          // magazines need to use large icons
          bool use_small_icon = app.desktop_file.has_suffix (".desktop");
          var annotated_icon = get_annotated_icon (app_icon,
                                                   pinfo.formatted_price,
                                                   pinfo.paid,
                                                   use_small_icon);
          icon_obj = annotated_icon.to_string ();
        }
        else
        {
          warning ("No purchase info for: %s, %s", app.application_name, app.package_name);
          icon_obj = app_icon.to_string ();
        }

        var result = Unity.ScopeResult ();
        result.uri = uri;
        result.icon_hint = icon_obj;
        result.category = category;
        result.result_type = pinfo != null && pinfo.paid ?
          Unity.ResultType.PERSONAL : Unity.ResultType.DEFAULT;
        result.mimetype = "application/x-desktop";
        result.title = app.application_name;
        result.comment = "";
        result.dnd_uri = "file://" + app.desktop_file;
        result.metadata = new HashTable<string,Variant> (str_hash, str_equal);

        result_set.add_result (result);
        duplicates_lookup.add (uri);
        i++;
        if (i == max_results)
          break;
      }
      return i;
    }

    private void add_pkg_search_result (Unity.Package.SearchResult results,
                                        Set<string> installed_uris,
                                        Set<string> available_uris,
                                        Unity.ResultSet result_set,
                                        Category category,
                                        uint max_add,
                                        bool running_on_phablet)
    {
      var appmanager = AppInfoManager.get_default();
      uint n_added = 0;

      foreach (unowned Unity.Package.PackageInfo pkginfo in results.results)
      {
        if (pkginfo.desktop_file == null)
          continue;

        string desktop_id = scope.sc_mangler.extract_desktop_id (
          pkginfo.desktop_file, category == Category.AVAILABLE);
        string full_path;

        AppInfo? app = appmanager.lookup (desktop_id);
        full_path = appmanager.get_path (desktop_id);

        if (running_on_phablet)
        {
          // XXX: This filtering should be pushed down to the Xapian
          // index and query builder logic.
          bool x_ubuntu_touch = false;
          bool x_is_click = false;

          if (full_path != null)
          {
            GLib.KeyFile key_file = new GLib.KeyFile ();
            try
            {
              key_file.load_from_file (full_path, GLib.KeyFileFlags.NONE);
              x_ubuntu_touch = key_file.get_boolean ("Desktop Entry", "X-Ubuntu-Touch");
              x_is_click = key_file.has_key ("Desktop Entry", "X-Ubuntu-Application-ID");
            }
            catch (Error e)
            {
              x_ubuntu_touch = false;
            }
          }

          // click apps are handled by unity-scope-click, so don't show them here either
          if (!x_ubuntu_touch || x_is_click)
          {
            continue;
          }
        }

        /* De-dupe by 'application://foo.desktop' URI. Also note that we need
         * to de-dupe before we chuck out NoDisplay app infos, otherwise they'd
         * show up from alternate sources */
        string uri = @"application://$(desktop_id)";
        if (uri in installed_uris || uri in available_uris)
          continue;

        /* Extract basic metadata and register de-dupe keys */
        var res = Unity.ScopeResult();
        res.uri = uri;
        res.category = category;
        res.result_type = (category != Category.AVAILABLE &&
                           !results.fuzzy_search) ?
          Unity.ResultType.PERSONAL : Unity.ResultType.DEFAULT;
        res.mimetype = "application/x-desktop";
        res.dnd_uri = full_path != null ? "file://" + full_path : "";
        res.metadata = new HashTable<string, Variant> (str_hash, str_equal);
        switch (category)
        {
          case Category.INSTALLED:
          case Category.APPLICATIONS:
            installed_uris.add (uri);
            if (app != null)
            {
              res.title =  app.get_display_name ();
              res.comment = sanitize_binary_name (app.get_executable ());
            }
            else
            {
              res.title = pkginfo.application_name;
            }

            break;
          case Category.AVAILABLE:
            available_uris.add (uri);
            res.title = pkginfo.application_name;
            res.comment = "";
            break;
          default:
            warning (@"Illegal category for package search $(category)");
            continue;
        }
        if (res.title == null) res.title = "";
        if (res.comment == null) res.comment = "";

        /* We can only chuck out NoDisplay and OnlyShowIn app infos after
         * we have registered a de-dupe key for them - which is done in the
         * switch block above) */
        if (app != null && !app.should_show ())
          continue;

        Icon icon = scope.find_pkg_icon (pkginfo.desktop_file, pkginfo.icon);
        if (category == Category.AVAILABLE)
        {
          /* If we have an available item, which is not a dupe, but is
           * installed anyway, we weed it out here, because it's probably
           * left out from the Installed section because of some rule in the
           * .menu file */
          if (app != null)
            continue;

          /* Apps that are not installed, ie. in the Available category
           * use the 'unity-install://pkgname/Full App Name' URI scheme,
           * but only use that after we've de-duped the results.
           * But only change the URI *after* we've de-duped the results! */
          res.uri = @"unity-install://$(pkginfo.package_name)/$(pkginfo.application_name)";
          available_uris.add (uri);

          // magazines need to use large icons
          bool use_small_icon = pkginfo.desktop_file.has_suffix (".desktop");
          res.icon_hint = get_annotated_icon (icon, pkginfo.price,
                                              !pkginfo.needs_purchase,
                                              use_small_icon);
        }
        else
        {
          res.icon_hint = icon.to_string ();
        }

        result_set.add_result (res);

        /* Stop if we added the number of items requested */
        n_added++;
        if (max_add > 0 && n_added >= max_add)
          return;
      }
    }

    /* Appends the subject URIs from a set of Zeitgeist.Events to our Dee.Model
     * assuming that these events are already sorted */
    public void append_events_with_category (Zeitgeist.ResultSet events,
                                             Unity.ResultSet result_set,
                                             uint category_id,
                                             bool include_favorites,
                                             int max_results = int.MAX,
                                             Set<string>? allowed_uris = null)
    {
      int num_results = 0;
      foreach (var ev in events)
      {
        string? app_uri = null;
        if (ev.num_subjects () > 0)
          app_uri = ev.get_subject (0).get_uri ();

        if (app_uri == null)
        {
          warning ("Unexpected event without subject");
          continue;
        }

        /* Assert that we indeed have a known application as actor */
        string desktop_id = Utils.get_desktop_id_for_actor (app_uri);

        /* Discard Recently Used apps that are in the launcher */
        if ((category_id == Category.RECENT ||
             category_id == Category.RECENT_APPS) &&
            !include_favorites &&
            (scope.favorite_apps.has_app_id (desktop_id)
            || scope.app_watcher.has_app_id (desktop_id)))
          continue;

        var appmanager = AppInfoManager.get_default ();
        AppInfo? app = appmanager.lookup (desktop_id);

        if (app == null)
          continue;

        if (!app.should_show ())
          continue;

        /* HACK! when using the max_results together with allowed_uris,
         * the limit serves as a truncation point - therefore results which
         * are not displayed the first time won't be displayed later
         * (consider results A, B, C - all of them are allowed and we use
         *  limit 2 - so only A and B is displayed, later we change allowed to
         *  B and C, so normally we would display both B and C, but that's not
         *  desired because C wasn't shown in the first place, so we display
         *  only B) */
        if (num_results++ >= max_results) break;

        if (allowed_uris != null && !(app_uri in allowed_uris)) continue;

        var result = Unity.ScopeResult ();
        result.uri = app_uri;
        result.icon_hint = app.get_icon ().to_string ();
        result.category = category_id;
        result.result_type = Unity.ResultType.DEFAULT;
        result.mimetype = "application/x-desktop";
        result.title = app.get_display_name ();
        result.comment = sanitize_binary_name (app.get_executable ());
        string full_path = appmanager.get_path (desktop_id);
        result.dnd_uri = full_path != null ? "file://" + full_path : app_uri;
        result.metadata = new HashTable<string,Variant> (str_hash, str_equal);

        result_set.add_result (result);
      }
    }
  }

  private class ApplicationsResultPreviewer : Unity.ResultPreviewer
  {
    private ApplicationsScope scope;

    public ApplicationsResultPreviewer (ApplicationsScope scope, Unity.ScopeResult result, Unity.SearchMetadata metadata)
    {
      this.scope = scope;
      set_scope_result (result);
      set_search_metadata (metadata);
    }

    public override Unity.AbstractPreview? run ()
    {
      var ml = new MainLoop ();
      Unity.AbstractPreview? preview = null;
      run_async ((obj, p) =>
        {
          preview = p;
          ml.quit ();
        });
      ml.run ();
      return preview;
    }

    public override void run_async (Unity.AbstractPreviewCallback callback)
    {
      make_preview.begin ((obj, res) =>
        {
          var preview = make_preview.end (res);
          callback (this, preview);
        });
    }

    private async Unity.AbstractPreview? make_preview ()
    {
      Unity.ApplicationPreview? preview = null;
      string desktopfile = null;
      string pkgname = "";
      string appname = "";

      var uri = result.uri;
      bool installed = uri.has_prefix ("application://");

      if (installed)
      {
        desktopfile = uri.substring (14); //remove "application://" prefix

        // de-mangle desktop file names back to what S-C expects
        if (scope.sc_mangler.contains (desktopfile))
        {
          desktopfile = scope.sc_mangler.get (desktopfile);
        }

        var pkginfo = scope.pkgsearcher.get_by_desktop_file (desktopfile);
        if (pkginfo != null)
        {
          appname = pkginfo.application_name;
          pkgname = pkginfo.package_name;
        }
      }
      else // unity-install
      {
        string app = uri.substring (16); //remove "unity-install://" prefix
        string[] parts = app.split ("/");
        if (parts.length > 1)
        {
          pkgname = parts[0];
          appname = parts[1];
        }
      }

      if (pkgname != "")
      {
        try {
          var details = yield scope.get_app_details (appname, pkgname);

          Icon? icon = null;
          if (installed)
            icon = new GLib.ThemedIcon (details.icon);
          else
          {
            icon = scope.find_pkg_icon (null, details.icon);
            if (icon.to_string () == GENERIC_APP_ICON && details.icon_url != null && details.icon_url != "")
            {
              icon = new GLib.FileIcon (File.new_for_uri (details.icon_url));
            }
          }

          Icon? screenshot = null;

          if (details.screenshot != null)
          {
            File scr_file = File.new_for_uri (details.screenshot);
            screenshot = new FileIcon (scr_file);
          }

          string subtitle = "";
          if (details.version != "")
            subtitle = _("Version %s").printf (details.version);
          if (details.size > 0)
          {
            if (subtitle != "")
              subtitle += ", ";
            subtitle += (_("Size %s")).printf (GLib.format_size (details.size));
          }
          preview = new Unity.ApplicationPreview (details.name, subtitle, details.description, icon, screenshot);
          preview.license = details.license;

          scope.init_ratings_db ();
          if (scope.ratings != null)
          {
            Unity.Ratings.Result result;
            scope.ratings.query (pkgname, out result);
            preview.set_rating (result.average_rating / 5.0f, result.total_rating);
          }

          if (details.hardware_requirements != "")
            preview.add_info (new InfoHint ("hardware-requirements", _("Hardware requirements"), null, details.hardware_requirements));

          if (uri.has_prefix ("unity-install://")) // application needs to be purchased/installed
          {
            // uninstalled and not purchased before
            if (details.pkg_state == SoftwareCenterData.PackageState.NEEDS_PURCHASE)
            {
              var buy_action = new Unity.PreviewAction ("buy", _("Buy"), null);
              if (details.price != null && details.price != "")
              {
                buy_action.extra_text = details.price;
              }

              preview.add_action (buy_action);
            }
            else // uninstalled, purchased before
            {

              Unity.PreviewAction install_action = null;
              if (details.raw_price == null || details.raw_price == "")
              {
                install_action = new Unity.PreviewAction ("install", _("Free Download"), null);
              }
              else
              {
                install_action = new Unity.PreviewAction ("install-paid", _("Install"), null);
              }
              preview.add_action (install_action);
            }

            if (details.website != null && details.website != "")
            {
              scope.preview_developer_website = details.website;
              var website_action = new Unity.PreviewAction ("website", _("Developer Site"), null);
              preview.add_action (website_action);
            }
          }
          else // application is already installed
          {
            preview.add_info (new InfoHint ("date-installed", _("Installed on"), null, details.installation_date));
            var launch_action = new Unity.PreviewAction ("launch", _("Launch"), null);
            preview.add_action (launch_action);
            if (!details.is_desktop_dependency)
            {
              var uninstall_action = new Unity.PreviewAction ("uninstall", _("Uninstall"), null);
              preview.add_action (uninstall_action);
            }
          }

          scope.preview_installable_desktop_file = details.desktop_file;
          scope.preview_installable_icon_file = details.icon;
        }
        catch (Error e)
        {
          warning ("Failed to get package details for '%s': %s", uri, e.message);
          preview = null;
        }
      }

      // xapian db doesn't know this .desktop file or S-C dbus data provider fails,
      // fallback to DesktopAppInfo (based on installed .desktop file) if available
      if (preview == null && desktopfile != null)
      {
        var app_info = new DesktopAppInfo (desktopfile);
        if (app_info != null)
        {
          preview = new Unity.ApplicationPreview (app_info.get_display_name (), "", app_info.get_description () ?? "", app_info.get_icon (), null);
          var launch_action = new Unity.PreviewAction ("launch", _("Launch"), null);
          preview.add_action (launch_action);
        }
      }

      if (preview == null)
      {
        warning ("No pksearcher nor desktop app info for '%s'", uri);
      }
      return preview;
    }
  }

} /* namespace */
