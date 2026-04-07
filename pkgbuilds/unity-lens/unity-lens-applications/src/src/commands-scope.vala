/* -*- mode: vala; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 * Copyright (C) 2011-2013 Canonical Ltd
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
 * Authored by Didier Roche <didrocks@ubuntu.com>
 *
 */

using Dee;
using Gee;


namespace Unity.ApplicationsLens {

  public static const string UNITY_RUNNER_PREFIX = "x-unity-no-preview:";

  private class AboutEntry {
    public string name;
    public string exec;
    public Icon   icon;

    public AboutEntry (string name, string exec, Icon icon)
    {
      this.name = name;
      this.exec = exec;
      this.icon = icon;
    }
  }

  private class CommandsScope : Unity.AbstractScope
  {
    // The ApplicationsScope holds a reference to us, so avoid a
    // reference cycle.
    public unowned ApplicationsScope appscope;

    /* for now, load the same keys as gnome-panel */
    private const string HISTORY_KEY = "history";
    private const int    MAX_HISTORY = 10;

    public Gee.HashMap<string,AboutEntry> about_entries;
    public Gee.List<string> history;
    public ExecSearcher exec_searcher = new ExecSearcher ();

    private Settings gp_settings;

    public CommandsScope (ApplicationsScope appscope)
    {
      this.appscope = appscope;

      /* create the private about entries */
      about_entries = new Gee.HashMap<string,AboutEntry> ();
      load_about_entries ();

      this.gp_settings = new Settings ("com.canonical.Unity.Runner");
      history = new Gee.ArrayList<string> ();
      load_history ();
    }

    public void add_history (string last_command)
    {

      // new history list: better, greatest, latest!
      var new_history = new Gee.ArrayList<string> ();
      var history_store = new string [this.history.size + 1];
      int i = 1;

      new_history.add (last_command);
      history_store[0] = last_command;
      for (var j = 0; (j < this.history.size) && (i < MAX_HISTORY); j++)
      {
        if (this.history[j] == last_command)
           continue;

        new_history.add(history[j]);
        history_store[i] = history[j];
        i++;
      }
      this.history = new_history;

      // store in gsettings
      this.gp_settings.set_strv (HISTORY_KEY, history_store);

      // force a search to refresh history order (TODO: be more clever in the future)
      results_invalidated (SearchType.DEFAULT);
    }

    private void load_history ()
    {
      int i = 0;
      string[] history_store = this.gp_settings.get_strv (HISTORY_KEY);
      foreach (var command in history_store)
      {
        if (i >= MAX_HISTORY)
          break;
        this.history.add((string) command.data);
        i++;
      }
    }

    private void load_about_entries ()
    {
      AboutEntry entry;
      string name;
      string exec;
      Icon icon;

      // first about:config
      name = "about:config";
      exec = "ccsm -p unityshell";
      try {
        icon = Icon.new_for_string (@"$(Config.PREFIX)/share/ccsm/icons/hicolor/64x64/apps/plugin-unityshell.png");
      }
      catch (Error err) {
        warning ("Can't find unityshell icon: %s", err.message);
        icon = new ThemedIcon ("gtk-execute");
      }
      entry = new AboutEntry (name, exec, icon);

      about_entries[name] = entry;
      about_entries[exec] = entry;

      // second about:robots
      name = "Robots have a plan.";
      exec = "firefox about:robots";
      entry = new AboutEntry (name, exec, icon = new ThemedIcon ("battery"));

      about_entries["about:robots"] = entry;
      about_entries[exec] = entry;

    }

    public override string get_group_name ()
    {
      return "com.canonical.Unity.Scope.Applications";
    }

    public override string get_unique_name ()
    {
      return "/com/canonical/unity/scope/commands";
    }

    public override Unity.CategorySet get_categories ()
    {
      var categories = new Unity.CategorySet ();
      var icon_dir = File.new_for_path (ICON_PATH);
      var cat = new Unity.Category ("results", _("Results"),
                                    new FileIcon (icon_dir.get_child ("group-installed.svg")));
      categories.add (cat);

      cat = new Unity.Category ("history", _("History"),
                                new FileIcon (icon_dir.get_child ("group-available.svg")));

      categories.add (cat);

      return categories;
    }

    public override Unity.FilterSet get_filters ()
    {
      var filters = new Unity.FilterSet ();
      return filters;
    }

    public override Unity.Schema get_schema ()
    {
      var schema = new Unity.Schema ();
      return schema;
    }

    public override string get_search_hint ()
    {
      return _("Run a command");
    }

    public override string normalize_search_query (string search_query)
    {
      return search_query.strip();
    }

    public override Unity.ScopeSearchBase create_search_for_query (Unity.SearchContext search_context)
    {
      return new CommandsSearch (this, search_context);
    }

    public override Unity.ResultPreviewer create_previewer (Unity.ScopeResult result, Unity.SearchMetadata metadata)
    {
      return null;
    }

    public override Unity.ActivationResponse? activate (Unity.ScopeResult result, Unity.SearchMetadata metadata, string? action_id)
    {
      return appscope.activate (result, metadata, action_id);
    }
  }

  private class CommandsSearch : Unity.ScopeSearchBase
  {
    private CommandsScope scope;

    public CommandsSearch (CommandsScope scope, Unity.SearchContext search_context)
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
      update_search.begin (() =>
      {
        async_callback (this);
      });
    }

    private void add_result (Unity.ResultSet result_set, string uri,
                             string icon_hint, uint category_id,
                             string mimetype, string title)
    {
      var result = Unity.ScopeResult ();
      result.uri = uri;
      result.icon_hint = icon_hint;
      result.category = category_id;
      result.result_type = ResultType.DEFAULT;
      result.mimetype = mimetype;
      result.title = title;
      result.comment = "";
      result.dnd_uri = uri;
      result.metadata = new HashTable<string, Variant> (str_hash, str_equal);
      result_set.add_result (result);
    }

    private async void update_search ()
    {
      var context = this.search_context;
      var result_set = context.result_set;
      var executables_match = new Gee.ArrayList<string> ();
      var dirs_match = new Gee.ArrayList<string> ();

      var search_string = context.search_query;
      bool has_search = !Utils.is_search_empty (search_string);

      string uri;
      Icon icon;
      string mimetype;
      string display_name;
      var category_id = RunnerCategory.HISTORY;

      foreach (var command in scope.history)
      {
        display_name = get_icon_uri_and_mimetype (command, out icon, out uri, out mimetype);
        add_result (result_set, uri, icon.to_string (), category_id, mimetype,
                    display_name);
      }

      if (!has_search)
      {
        return;
      }

      Timer timer = new Timer ();

      /* no easter egg in unity */
      if (search_string == "free the fish")
      {
        uri = "about:blank";
        string commenteaster = _("There is no easter egg in Unity");
        icon = new ThemedIcon ("gnome-panel-fish");
        add_result (result_set, uri, icon.to_string (), 0, "text/plain",
                    commenteaster);
        return;
      }
      else if (search_string == "gegls from outer space")
      {
        uri = "about:blank";
        string commentnoeaster = _("Still no easter egg in Unity");
        icon = new ThemedIcon ("gnome-panel-fish");
        add_result (result_set, uri, icon.to_string (), 0, "text/plain",
                    commentnoeaster);
        return;

      }

      /* manual seek with directory and executables result */
      if (search_string.has_prefix ("/") || search_string.has_prefix ("~"))
      {
        search_string = Utils.subst_tilde (search_string);
        var search_dirname = Path.get_dirname (search_string);
        var directory = File.new_for_path (search_dirname);
        var search_dirname_in_path = false;

        /* strip path_directory if in executable in path */
        foreach (var path_directory in Environment.get_variable ("PATH").split(":"))
        {
          if (search_dirname == path_directory || search_dirname == path_directory + "/")
          {
            search_dirname_in_path = true;
            break;
          }
        }

        try {
          var iterator = directory.enumerate_children (FileAttribute.STANDARD_NAME + "," + FileAttribute.STANDARD_TYPE + "," + FileAttribute.ACCESS_CAN_EXECUTE,
                                                 0, null);
          while (true)
          {
            var subelem_info = iterator.next_file ();
            if (subelem_info == null)
              break;

            var complete_path = Path.build_filename (search_dirname, subelem_info.get_name ());
            if (complete_path.has_prefix (search_string))
            {
              if (subelem_info.get_file_type () == FileType.DIRECTORY)
              {
                dirs_match.add (complete_path);
              }
              else if (subelem_info.get_attribute_boolean (FileAttribute.ACCESS_CAN_EXECUTE))
              {
                // only include exec name if we are in the PATH
                if (search_dirname_in_path)
                  executables_match.add (subelem_info.get_name ());
                else
                  executables_match.add (complete_path);
              }
            }
          }
        }
        catch (Error err) {
          warning("Error listing directory executables: %s\n", err.message);
        }

      }
      /* complete again system executables */
      else
      {
        var matching_executables = yield scope.exec_searcher.find_prefixed (search_string);
        foreach (var matching_exec in matching_executables)
        {
          executables_match.add (matching_exec);
        }
      }

      executables_match.sort ();
      dirs_match.sort ();

      category_id = RunnerCategory.RESULTS;

      // populate results
      // 1. enable launching the exact search string if no other result
      if ((executables_match.size == 0) && (dirs_match.size == 0))
      {
        display_name = get_icon_uri_and_mimetype (search_string, out icon, out uri, out mimetype);
        add_result (result_set, uri.strip (), icon.to_string (),
                    category_id, mimetype, display_name);
      }

      // 2. add possible directories (we don't store them)
      mimetype = "inode/directory";
      icon = ContentType.get_icon (mimetype);
      foreach (var dir in dirs_match)
      {
        uri = UNITY_RUNNER_PREFIX + dir;
        add_result (result_set, uri, icon.to_string (),
                    category_id, mimetype, dir);
      }

      // 3. add available exec
      foreach (var final_exec in executables_match)
      {
        // TODO: try to match to a desktop file for the icon
        uri = UNITY_RUNNER_PREFIX + final_exec;
        display_name = get_icon_uri_and_mimetype (final_exec, out icon, out uri, out mimetype);
        add_result (result_set, uri, icon.to_string (),
                    category_id, mimetype, display_name);
      }

      timer.stop ();
      debug ("Entry search listed %i dir matches and %i exec matches in %fms for search: %s",
             dirs_match.size, executables_match.size, timer.elapsed ()*1000, search_string);
    }

    private string get_icon_uri_and_mimetype (string exec_string, out Icon? icon, out string? uri, out string? mimetype)
    {

      AboutEntry? entry = null;

      mimetype = "application/x-unity-run";
      entry = scope.about_entries[exec_string];
      if (entry != null)
      {
        uri = UNITY_RUNNER_PREFIX + entry.exec;
        icon = entry.icon;
        return entry.name;
      }

      uri = UNITY_RUNNER_PREFIX + exec_string;


      // if it's a folder, show… a folder icone! + right exec
      if (FileUtils.test (exec_string, FileTest.IS_DIR))
      {
        mimetype = "inode/directory";
        icon = ContentType.get_icon (mimetype);
        return exec_string;
      }

      var s = exec_string.delimit ("-", '_').split (" ", 0)[0];
      var appresults = scope.appscope.appsearcher.search (
        @"type:Application AND exec:$s", 0,
        Unity.Package.SearchType.EXACT,
        Unity.Package.Sort.BY_NAME);
      foreach (unowned Unity.Package.PackageInfo pkginfo in appresults.results)
      {

        if (pkginfo.desktop_file == null)
          continue;

        // pick the first one
        icon = scope.appscope.find_pkg_icon (pkginfo.desktop_file, pkginfo.icon);
        return exec_string;

      }

      // if no result, default icon
      icon = new ThemedIcon ("gtk-execute");
      return exec_string;

    }
  }

  private class ExecSearcher: Object
  {
    public ExecSearcher ()
    {
      Object ();
    }

    private Gee.List<string> executables;

    construct
    {
      executables = new Gee.ArrayList<string> ();
      listing_status = ListingStatus.NOT_STARTED;
    }

    // TODO: add reload
    private async void find_system_executables ()
    {
      if (this.executables.size > 0)
        return;

      foreach (var path_directory in Environment.get_variable ("PATH").split(":"))
      {
        var dir = File.new_for_path (path_directory);
        try {
          var e = yield dir.enumerate_children_async (FileAttribute.STANDARD_NAME + "," + FileAttribute.ACCESS_CAN_EXECUTE,
                                                      0, Priority.DEFAULT, null);
          while (true) {
            var files = yield e.next_files_async (64, Priority.DEFAULT, null);
            if (files == null)
                break;

            foreach (var info in files) {
              if (info.get_attribute_boolean (FileAttribute.ACCESS_CAN_EXECUTE))
              {
                this.executables.add (info.get_name ());
              }
            }
          }
        }
        catch (Error err) {
          warning("Error listing directory executables: %s\n", err.message);
        }
      }
    }

    private enum ListingStatus
    {
      NOT_STARTED,
      STARTED,
      FINISHED
    }

    public int listing_status { get; private set; }

    public async Gee.Collection<string> find_prefixed (string search_string)
    {
      // initialize the available binaries lazily
      if (listing_status == ListingStatus.NOT_STARTED)
      {
        listing_status = ListingStatus.STARTED;
        yield find_system_executables ();
        listing_status = ListingStatus.FINISHED;
      }
      else if (listing_status == ListingStatus.STARTED)
      {
        var sig_id = this.notify["listing-status"].connect (() =>
        {
          if (listing_status == ListingStatus.FINISHED)
            find_prefixed.callback ();
        });
        yield;
        SignalHandler.disconnect (this, sig_id);
      }

      var matching = new Gee.ArrayList<string> ();
      foreach (var exec_candidate in executables)
      {
        if (exec_candidate.has_prefix (search_string))
        {
          matching.add (exec_candidate);
        }
      }

      return matching;
    }
  }
}
