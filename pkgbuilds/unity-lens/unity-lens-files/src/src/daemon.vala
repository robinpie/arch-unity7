/*
 * Copyright (C) 2010-2012 Canonical Ltd
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
 *             Michal Hruby <michal.hruby@canonical.com>
 *             Angelo Compagnucci <angelo.compagnucci@gmail.com>
 *
 */
using Zeitgeist;
using Zeitgeist.Timestamp;
using Config;
using Gee;

namespace Unity.FilesLens {

  const string ICON_PATH = Config.DATADIR + "/icons/unity-icon-theme/places/svg/";

  public class Daemon : GLib.Object
  {
    private Zeitgeist.Log log;
    private Zeitgeist.Index index;
    private Zeitgeist.Monitor monitor;

    private Bookmarks bookmarks;
    private Devices devices;
    private UrlChecker urls;

    private Unity.DeprecatedScope scope;

    /* For each section we have a set of Zeitgeist.Event templates that
     * we use to query Zeitgeist */
    private HashTable<string, Event> type_templates;
    private HashTable<string, Event> download_dir_type_templates;

    private const string SCHEMA_NAME = "com.canonical.Unity.FilesLens";
    private const string USE_LOCATE_KEY = "use-locate";
    public bool use_locate { get; set; default = true; }

    private Settings scope_settings;
    private Variant empty_asv;

    construct
    {
      empty_asv = new Variant.array (VariantType.VARDICT.element (), {});
      prepare_type_templates ();

      if (SCHEMA_NAME in Settings.list_schemas ())
      {
        scope_settings = new Settings (SCHEMA_NAME);
        scope_settings.bind (USE_LOCATE_KEY, this, USE_LOCATE_KEY,
                            SettingsBindFlags.GET);
      }

      scope = new Unity.DeprecatedScope ("/com/canonical/unity/scope/files",
                                         "files-local.scope");
      scope.search_in_global = true;
      scope.search_hint = _("Search Files & Folders");
      scope.activate_uri.connect (activate);
      scope.preview_uri.connect (preview);

      populate_categories ();
      populate_filters ();

      /* Bring up Zeitgeist interfaces */
      log = new Zeitgeist.Log ();
      index = new Zeitgeist.Index ();

      /* Listen for all file:// related events from Zeitgeist */
      var templates = new GenericArray<Event>();
      var event = new Zeitgeist.Event ();
      var subject = new Zeitgeist.Subject ();
      subject.uri = "file://*";
      event.add_subject (subject);
      templates.add (event);
      monitor = new Zeitgeist.Monitor (new Zeitgeist.TimeRange.from_now (),
                                       templates);
      monitor.events_inserted.connect (on_zeitgeist_changed);
      monitor.events_deleted.connect (on_zeitgeist_changed);
      log.install_monitor (monitor);

      bookmarks = new Bookmarks ();
      urls = new UrlChecker ();

      devices = new Devices ();
      devices.changed.connect (() => {
        /* make sure our results are fresh */
        scope.queue_search_changed (SearchType.DEFAULT);
        scope.queue_search_changed (SearchType.GLOBAL);
      });

      scope.generate_search_key.connect ((search) => {
        return search.search_string.strip ();
      });

      /* Listen for changes to the search */
      scope.search_changed.connect ((search, search_type, cancellable) =>
      {
        dispatch_search.begin (search, search_type, cancellable);
      });

      scope.export ();
    }

    private async void dispatch_search (DeprecatedScopeSearch scope_search,
                                        SearchType search_type,
                                        GLib.Cancellable cancellable)
    {
      if (search_type == SearchType.GLOBAL)
      {
        yield update_global_search_async (scope_search, cancellable);
      }
      else
      {
        yield update_search_async (scope_search, cancellable);
      }

      // make sure we don't forget to emit finished (if we didn't get cancelled)
      if (!cancellable.is_cancelled ())
      {
        if (scope_search.results_model.get_n_rows () == 0)
        {
          scope_search.set_reply_hint ("no-results-hint",
            _("Sorry, there are no files or folders that match your search."));
        }

      }
      scope_search.finished ();
    }

    private void populate_filters ()
    {
      var filters = new Unity.FilterSet ();

      /* Last modified */
      {
        var filter = new RadioOptionFilter ("modified", _("Last modified"));

        filter.add_option ("last-7-days", _("Last 7 days"));
        filter.add_option ("last-30-days", _("Last 30 days"));
        filter.add_option ("last-year", _("Last year"));

        filters.add (filter);
      }

      /* Type filter */
      {
        var filter = new CheckOptionFilter ("type", _("Type"));
        filter.sort_type = OptionsFilter.SortType.DISPLAY_NAME;

        filter.add_option ("documents", _("Documents"));
        filter.add_option ("folders", _("Folders"));
        filter.add_option ("images", _("Images"));
        filter.add_option ("audio", _("Audio"));
        filter.add_option ("videos", _("Videos"));
        filter.add_option ("presentations", _("Presentations"));
        filter.add_option ("other", _("Other"));

        filters.add (filter);
      }

      /* Size filter */
      {
        var filter = new MultiRangeFilter ("size", _("Size"));

        filter.add_option ("100kB", _("100kB"));
        filter.add_option ("1MB", _("1MB"));
        filter.add_option ("10MB", _("10MB"));
        filter.add_option ("100MB", _("100MB"));
        filter.add_option ("1GB", _("1GB"));
        filter.add_option (">1GB", _(">1GB"));

        filters.add (filter);
      }

      scope.filters = filters;
    }

    private void populate_categories ()
    {
      var categories = new Unity.CategorySet ();
      var icon_dir = File.new_for_path (ICON_PATH);

      var cat = new Unity.Category ("global", _("Files & Folders"),
                                    new FileIcon (icon_dir.get_child ("group-folders.svg")));
      categories.add (cat);

      cat = new Unity.Category ("recent", _("Recent"),
                                new FileIcon (icon_dir.get_child ("group-recent.svg")));
      categories.add (cat);

      cat =  new Unity.Category ("downloads", _("Downloads"),
                                 new FileIcon (icon_dir.get_child ("group-downloads.svg")));
      categories.add (cat);

      cat = new Unity.Category ("folders", _("Folders"),
                                new FileIcon (icon_dir.get_child ("group-folders.svg")));
      categories.add (cat);

      scope.categories = categories;
    }

    private void init_templates (HashTable<string, Event> templates,
                                 string manifestation, string uri_prefix)
    {
      Event event;
      Subject sub;

      /* Section.ALL_FILES */
      event = new Event.full("", manifestation, "", "");
      sub = new Subject.full(uri_prefix, "", "", "", "", "", "");
      event.add_subject(sub);
      templates["all"] = event;

      /* Section.DOCUMENTS */
      event = new Event.full("", manifestation, "", "");
      sub = new Subject.full (uri_prefix, NFO.DOCUMENT, "", "", "", "", "");
      event.add_subject(sub);
      sub = new Subject.full ("", "!"+NFO.PRESENTATION, "", "", "", "", "");
      event.add_subject(sub);
      templates["documents"] = event;

      /* Section.FOLDERS
       * - we're using special ORIGIN queries here */
      event = new Event.full("", manifestation, "", "");
      sub =  new Subject.full (uri_prefix, "", "", "", "", "", "");
      event.add_subject(sub);
      templates["folders"] = event;

      /* Section.IMAGES */
      event = new Event.full("", manifestation, "", "");
      sub = new Subject.full (uri_prefix, NFO.IMAGE, "", "", "", "", "");
      event.add_subject(sub);
      templates["images"] = event;

      /* Section.AUDIO */
      event = new Event.full("", manifestation, "", "");
      sub = new Subject.full (uri_prefix, NFO.AUDIO, "", "", "", "", "");
      event.add_subject(sub);
      templates["audio"] = event;

      /* Section.VIDEOS */
      event = new Event.full("", manifestation, "", "" );
      sub = new Subject.full (uri_prefix, NFO.VIDEO, "", "", "", "", "");
      event.add_subject(sub);
      templates["videos"] = event;

      /* Section.PRESENTATIONS
       * FIXME: Zeitgeist logger needs to user finer granularity
       *        on classification as I am not sure it uses
       *        NFO.PRESENTATION yet */
      event = new Event.full("", manifestation, "", "");
      sub = new Subject.full (uri_prefix, NFO.PRESENTATION, "", "", "", "", "");
      event.add_subject(sub);
      templates["presentations"] = event;

      /* Section.OTHER
       * Note that subject templates are joined with logical AND */
      event = new Event.full("", manifestation, "");
      event.add_subject (new Subject.full (uri_prefix,
                                           "!"+NFO.DOCUMENT, "", "", "", "", ""));
      event.add_subject (new Subject.full ("",
                                           "!"+NFO.IMAGE,
                                           "",
                                           "", "", "", ""));
      event.add_subject (new Subject.full ("",
                                           "!"+NFO.AUDIO,
                                           "",
                                           "", "", "", ""));
      event.add_subject (new Subject.full ("",
                                           "!"+NFO.VIDEO,
                                           "",
                                           "", "", "", ""));
      event.add_subject (new Subject.full ("",
                                           "!"+NFO.PRESENTATION,
                                           "",
                                           "", "", "", ""));
      templates["other"] = event;
    }

    private void prepare_type_templates ()
    {
      type_templates = new HashTable<string, Event> (str_hash, str_equal);
      init_templates (type_templates, ZG.USER_ACTIVITY, "file:*");

      unowned string download_path;
      download_path = Environment.get_user_special_dir (UserDirectory.DOWNLOAD);
      if (download_path != null)
      {
        download_dir_type_templates = new HashTable<string, Event> (str_hash,
                                                                    str_equal);
        string download_uri = File.new_for_path (download_path).get_uri ();
        init_templates (download_dir_type_templates, "", download_uri + "/*");
      }
    }

    private const string[] ALL_TYPES =
    {
      "documents",
      "folders",
      "images",
      "audio",
      "videos",
      "presentations",
      "other"
    };

    private GenericArray<Event>? create_template (OptionsFilter? filter,
                                                  bool downloads_templates)
    {
      var templates = new GenericArray<Event> ();

      HashTable<string, Event>? prepared_templates = downloads_templates ?
        download_dir_type_templates : type_templates;
      if (prepared_templates == null) return null;

      if (filter == null || !filter.filtering)
      {
        /* Section.ALL_FILES */
        templates.add (prepared_templates["all"]);
        return templates;
      }

      string[] types = {};

      foreach (unowned string type_id in ALL_TYPES)
      {
        var option = filter.get_option (type_id);
        if (option == null || !option.active) continue;

        types += type_id;
      }

      if (types.length == ALL_TYPES.length)
      {
        /* Section.ALL_FILES */
        templates.add (prepared_templates["all"]);
        return templates;
      }

      foreach (unowned string type_id in types)
      {
        // we need to handle folders separately
        if (type_id == "folders") continue;

        templates.add (prepared_templates[type_id]);
      }

      return templates;
    }

    private bool is_search_empty (DeprecatedScopeSearch search)
    {
      if (search.search_string == null) return true;

      return search.search_string.strip () == "";
    }

    private string prepare_search_string (DeprecatedScopeSearch search)
    {
      var s = search.search_string;

      if (s.has_suffix (" "))
        s = s.strip ();

      if (!s.has_suffix ("*"))
        s = s + "*";

      /* The Xapian query parser (used by the Zeitgeist FTS Extension) seems to
       * handle hyphens in a special way, namely that it forces the joined
       * tokens into a phrase query no matter if it appears as the last word
       * in a query and we have the PARTIAL flag set on the query parser.
       * This makes 'unity-p' not match 'unity-package-search.cc' etc. */
      s = s.delimit ("-", ' ');

      return s;
    }

    private async void update_global_search_async (DeprecatedScopeSearch search,
                                                   GLib.Cancellable cancellable)
    {
      var has_search = !is_search_empty (search);
      var results_model = search.results_model;

      /*
       * For global searches we collate all results under one category heading
       * called Files & Folders
       */

      try {
        /* Get results ranked by recency */
        var results = yield run_zg_query (search,
                                          new Zeitgeist.TimeRange.anytime (),
                                          Zeitgeist.ResultType.MOST_RECENT_CURRENT_URI,
                                          null,
                                          20,
                                          cancellable);

        results_model.clear ();

        /* check if the thing typed isn't a url (like facebook.com)
         * or isn't a remote mountable url (like ftp://ftp.ubuntu.com)*/
        var url_type = UrlType.UNKNOWN;
        var checked_url = urls.check_url (search.search_string, out url_type);
        if (checked_url != null)
        {
          results_model.append (checked_url, urls.get_icon_for_type(url_type),
                Categories.FILES_AND_FOLDERS,
                ResultType.PERSONAL,
                "text/html", search.search_string,
                checked_url, checked_url, empty_asv);
        }

        var category_id = Categories.FILES_AND_FOLDERS;

        Set<string>? bookmark_uris = null;
        if (has_search)
        {
          /* add bookmarks first */
          GLib.List<Bookmark> matching_bookmarks;
          matching_bookmarks = bookmarks.prefix_search (search.search_string);
          append_bookmarks (matching_bookmarks, results_model, category_id);

          /* make sure we don't duplicate the bookmark uris */
          bookmark_uris = new Gee.HashSet<string> ();
          foreach (var bookmark_obj in matching_bookmarks)
          {
            /* uri has bookmark: scheme */
            bookmark_uris.add (bookmark_obj.dnd_uri);
          }
        }

        var result_flags = has_search ? ResultFlags.EXTRACT_ORIGINS : 0;
        append_events_sorted (search.search_string,
                              results, results_model,
                              0, int64.MAX, result_flags,
                              bookmark_uris, category_id);

        if (has_search)
        {
          GLib.List<Device> matching_devices;

          if (has_search)
            matching_devices = devices.search (search.search_string);
          else
            matching_devices = devices.list ();

          append_devices (matching_devices, results_model, Categories.FILES_AND_FOLDERS);
        }

        /* Add downloads catagory if we don't have a search */
        if (has_search == false)
        {
          results = yield run_zg_query (search,
                                        new Zeitgeist.TimeRange.anytime (),
                                        Zeitgeist.ResultType.MOST_RECENT_CURRENT_URI,
                                        null,
                                        40,
                                        cancellable,
                                        true);

          append_events_sorted (search.search_string,
                                results, results_model,
                                0, int64.MAX, ResultFlags.NONE,
                                null, Categories.DOWNLOADS);
        }

        if (has_search && use_locate)
        {
          yield perform_locate (search, 500, search.search_string,
                                search.results_model, cancellable,
                                Categories.FILES_AND_FOLDERS,
                                Categories.FILES_AND_FOLDERS);
        }

      } catch (IOError.CANCELLED ioe) {
        return;
      } catch (GLib.Error e) {
        warning ("Error performing global search '%s': %s",
                 search.search_string, e.message);
      }
    }

    private async void update_search_async (DeprecatedScopeSearch search,
                                            GLib.Cancellable cancellable)
    {
      var results_model = search.results_model;
      var txn = new Dee.Transaction (results_model);
      var has_search = !is_search_empty (search);

      var filter = search.get_filter ("type") as OptionsFilter;

      var active_filters = get_current_types (search);
      bool only_folders = active_filters != null &&
        active_filters.length == 1 && active_filters[0] == "folders";
      uint timer_id = 0;

      try
      {
        /* Get results ranked by recency */
        Zeitgeist.ResultSet? results = null;
        if (!only_folders)
        {
          results = yield run_zg_query (search,
                                        get_current_timerange (search),
                                        Zeitgeist.ResultType.MOST_RECENT_CURRENT_URI,
                                        filter,
                                        50,
                                        cancellable);
        }

        txn.clear ();

        /* check if the thing typed isn't a url (like facebook.com)
         * or isn't a remote mountable url (like ftp://ftp.ubuntu.com)*/
        var url_type = UrlType.UNKNOWN;
        var checked_url = urls.check_url (search.search_string, out url_type);
        if (checked_url != null)
        {
          txn.append (checked_url, urls.get_icon_for_type(url_type),
                Categories.RECENT, ResultType.PERSONAL,
                "text/html", search.search_string,
                checked_url, checked_url, empty_asv);
        }

        /* apply filters to results found by zeitgeist */
        int64 min_size, max_size;
        get_current_size_limits (search, out min_size, out max_size);

        if (results != null)
        {
          append_events_sorted (search.search_string,
                                results, txn,
                                min_size, max_size,
                                ResultFlags.SKIP_FOLDERS);
        }

        /* get recently downloaded files */
        results = yield run_zg_query (search,
                                      get_current_timerange (search),
                                      Zeitgeist.ResultType.MOST_RECENT_CURRENT_URI,
                                      filter,
                                      50,
                                      cancellable,
                                      true);

        append_events_sorted (search.search_string,
                              results, txn,
                              min_size, max_size,
                              ResultFlags.NONE,
                              null, Categories.DOWNLOADS);

        /* commit if the origin query is taking too long, if we committed right
         * away, we'd cause flicker */
        if (txn.get_n_rows () > 0)
        {
          /* here be something magical */
          timer_id = Timeout.add (200, () =>
          {
            if (!cancellable.is_cancelled () && !txn.is_committed ())
            {
              txn.commit ();
            }
            timer_id = 0;
            return false;
          });
        }

        /* folders are last category we need, update the model directly */
        if (filter == null ||
            !filter.filtering || filter.get_option ("folders").active)
        {
          results = yield run_zg_query (search, get_current_timerange (search),
                                        Zeitgeist.ResultType.MOST_RECENT_ORIGIN,
                                        null, 50, cancellable);

          if (!txn.is_committed ()) txn.commit ();

          /* add bookmarks first */
          GLib.List<Bookmark> matching_bookmarks;
          if (has_search)
            matching_bookmarks = bookmarks.prefix_search (search.search_string);
          else
            matching_bookmarks = bookmarks.list ();
          append_bookmarks (matching_bookmarks, results_model,
                            Categories.FOLDERS);

          /* make sure we don't duplicate the bookmark uris */
          var bookmark_uris = new Gee.HashSet<string> ();
          foreach (var bookmark_obj in matching_bookmarks)
          {
            /* uri has bookmark: scheme */
            bookmark_uris.add (bookmark_obj.dnd_uri);
          }

          append_events_sorted (search.search_string,
                                results, results_model,
                                min_size, max_size,
                                ResultFlags.USE_ORIGIN,
                                bookmark_uris);

          GLib.List<Device> matching_devices;

          if (has_search)
            matching_devices = devices.search (search.search_string);
          else
            matching_devices = devices.list ();

          append_devices (matching_devices, results_model,
                          Categories.FOLDERS);
        }
        else
        {
          /* just commit */
          if (!txn.is_committed ()) txn.commit ();
        }

        if (has_search && use_locate)
        {
          yield perform_locate (search, 500, search.search_string,
                                search.results_model, cancellable);
        }

      } catch (IOError.CANCELLED ioe) {
        return;
      } catch (GLib.Error e) {
        /* if we weren't cancelled, commit the transaction */
        warning ("Error performing global search '%s': %s",
                 search.search_string, e.message);
        if (!txn.is_committed ()) txn.commit ();
      } finally {
        if (timer_id != 0) Source.remove (timer_id);
      }
    }

    private string[]? get_current_types (DeprecatedScopeSearch search)
    {
      /* returns null if the filter is disabled / all options selected */
      var filter = search.get_filter ("type") as CheckOptionFilter;

      if (filter == null || !filter.filtering) return null;
      string[] types = {};

      foreach (unowned string type_id in ALL_TYPES)
      {
        var option = filter.get_option (type_id);
        if (option == null || !option.active) continue;

        types += type_id;
      }

      if (types.length == ALL_TYPES.length) return null;

      return types;
    }

    private TimeRange get_current_timerange (DeprecatedScopeSearch search)
    {
      var filter = search.get_filter ("modified") as RadioOptionFilter;
      Unity.FilterOption? option = filter.get_active_option ();

      string date = option == null ? "all" : option.id;

      if (date == "last-7-days")
        return new TimeRange (Timestamp.from_now() - Timestamp.WEEK, Timestamp.from_now ());
      else if (date == "last-30-days")
        return new TimeRange (Timestamp.from_now() - (Timestamp.WEEK * 4), Timestamp.from_now());
      else if (date == "last-year")
        return new TimeRange (Timestamp.from_now() - Timestamp.YEAR, Timestamp.from_now ());
      else
        return new TimeRange.anytime ();
    }

    private void get_current_size_limits (DeprecatedScopeSearch search, out int64 min_size, out int64 max_size)
    {
      var filter = search.get_filter ("size") as MultiRangeFilter;
      Unity.FilterOption? min_opt = filter.get_first_active ();
      Unity.FilterOption? max_opt = filter.get_last_active ();

      if (min_opt == null || max_opt == null)
      {
        min_size = 0;
        max_size = int64.MAX;
        return;
      }

      int64[] sizes =
      {
        0,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000,
        int64.MAX
      };

      switch (min_opt.id)
      {
        case "100kB":
          min_size = sizes[0]; break;
        case "1MB":
          min_size = sizes[1]; break;
        case "10MB":
          min_size = sizes[2]; break;
        case "100MB":
          min_size = sizes[3]; break;
        case "1GB":
          min_size = sizes[4]; break;
        case ">1GB":
          min_size = sizes[5]; break;
        default:
          warn_if_reached ();
          min_size = 0;
          break;
      }

      switch (max_opt.id)
      {
        case "100kB":
          max_size = sizes[1]; break;
        case "1MB":
          max_size = sizes[2]; break;
        case "10MB":
          max_size = sizes[3]; break;
        case "100MB":
          max_size = sizes[4]; break;
        case "1GB":
          max_size = sizes[5]; break;
        case ">1GB":
          max_size = sizes[6]; break;
        default:
          warn_if_reached ();
          max_size = int64.MAX;
          break;
      }
    }

    private async Zeitgeist.ResultSet? run_zg_query (
        DeprecatedScopeSearch search,
        TimeRange time_range,
        Zeitgeist.ResultType result_type,
        OptionsFilter? filters,
        uint num_results,
        GLib.Cancellable cancellable,
        bool downloads_only = false) throws Error
    {
      Zeitgeist.ResultSet results;
      var timer = new Timer ();
      var templates = create_template (filters, downloads_only);
      if (templates == null) return null;

      /* Copy the templates to a PtrArray which libzg expects */
      var ptr_arr = new GenericArray<Event>();
      for (int i = 0; i < templates.length; i++)
      {
        ptr_arr.add (templates[i]);
      }

      /* Get results ranked by recency */
      if (is_search_empty (search))
      {
        results = yield log.find_events (time_range,
                                         ptr_arr,
                                         Zeitgeist.StorageState.ANY,
                                         num_results,
                                         result_type,
                                         cancellable);
      }
      else
      {
        var search_string = prepare_search_string (search);

        results = yield index.search (search_string,
                                      time_range,
                                      ptr_arr,
                                      0, // offset
                                      num_results,
                                      result_type,
                                      cancellable);
      }

      debug ("Found %u/%u no search results in %fms",
             results.size (), results.estimated_matches (),
             timer.elapsed()*1000);

      return results;
    }

    private void append_bookmarks (GLib.List<Bookmark> bookmarks,
                                   Dee.Model results_model,
                                   Categories category)
    {
      foreach (var bookmark in bookmarks)
      {
        results_model.append (bookmark.uri, bookmark.icon, category,
                              ResultType.PERSONAL,
                              bookmark.mimetype, bookmark.display_name, "",
                              bookmark.dnd_uri, empty_asv);
      }
    }

    private void append_devices (GLib.List<Device> devices,
                                 Dee.Model results_model,
                                 Categories category)
    {
      foreach (var device in devices)
      {
        results_model.append (device.uri, device.icon_name, category,
                              ResultType.PERSONAL, "", device.display_name, "",
                              device.dnd_uri, empty_asv);
      }
    }

    private async void perform_locate (DeprecatedScopeSearch search,
                                       uint timeout,
                                       string query,
                                       Dee.Model results_model,
                                       GLib.Cancellable cancellable,
                                       int files_category = Categories.RECENT,
                                       int dirs_category = Categories.FOLDERS)
      throws Error
    {
      cancellable.set_error_if_cancelled (); // throws IOError
      /* Wait a while before starting the locate search */
      uint timer_src_id = Timeout.add_full (Priority.LOW, timeout,
                                            perform_locate.callback);
      var canc_id = cancellable.connect (() =>
      {
        if (timer_src_id != 0) Source.remove (timer_src_id);
        timer_src_id = 0;
        // can't invoke directly, the disconnect call would deadlock
        Idle.add_full (Priority.DEFAULT, perform_locate.callback);
      });
      yield;

      cancellable.disconnect (canc_id);
      if (timer_src_id != 0) Source.remove (timer_src_id);

      cancellable.set_error_if_cancelled (); // throws IOError

      var results = yield Locate.locate (query, cancellable);
      results.sort (Utils.cmp_file_info_by_mtime);

      /* make a list of uris we already have */
      var present_uris = new Gee.HashSet<string> ();
      var model_iter = results_model.get_first_iter ();
      var model_end_iter = results_model.get_last_iter ();
      while (model_iter != model_end_iter)
      {
        present_uris.add (results_model.get_string (model_iter, 0));
        model_iter = results_model.next (model_iter);
      }

      var timerange = get_current_timerange (search);
      int64 min_size, max_size;
      get_current_size_limits (search, out min_size, out max_size);
      var types = get_current_types (search);

      foreach (var info in results)
      {
        var uri = info.get_data<File> ("associated-gfile").get_uri ();
        warn_if_fail (uri != null);
        if (uri == null || uri in present_uris) continue;
        var mimetype = info.get_attribute_string (FileAttribute.STANDARD_FAST_CONTENT_TYPE);
        var icon_hint = Utils.check_icon_string (uri, mimetype, info);

        // check if we match the timerange
        uint64 mtime = info.get_attribute_uint64 (FileAttribute.TIME_MODIFIED) * 1000;

        if (mtime < timerange.start || mtime > timerange.end)
          continue;

        // check if type matches
        if (types != null && !Utils.file_info_matches_any (info, types))
          continue;

        // check if size is within bounds
        int64 size = info.get_size ();
        if (size < min_size || size > max_size)
          continue;

        uint category_id = mimetype == "inode/directory" ?
          dirs_category : files_category;

        results_model.append (uri, icon_hint, category_id, ResultType.PERSONAL,
                              mimetype, info.get_display_name (), "", uri,
                              empty_asv);
      }
    }

    private void count_directory_items (string top_path, ref int count, ref uint64 total_size, bool recurse=false) throws GLib.Error
    {
      var dir = GLib.File.new_for_path (top_path);
      var file_iter = dir.enumerate_children (GLib.FileAttribute.STANDARD_SIZE, GLib.FileQueryInfoFlags.NONE); //follow symlinks
      GLib.FileInfo? info = null;

      GLib.Queue<string>? dirs = null;
      do
      {
        info = file_iter.next_file ();
        if (info != null)
        {
          count++;
          if (info.get_file_type () == GLib.FileType.REGULAR)
          {
            total_size += info.get_size ();
          }
          else
          {
            if (info.get_file_type () == GLib.FileType.DIRECTORY && recurse)
            {
              if (dirs == null)
              {
                dirs = new GLib.Queue<string> ();
              }
              dirs.push_tail (info.get_name());
            }
          }
        }
      } while (info != null);
      file_iter.close();

      if (dirs != null)
      {
        while (!dirs.is_empty())
        {
          string path = top_path + "/" + dirs.pop_head ();
          count_directory_items (path, ref count, ref total_size, recurse);
        }
      }
    }

    public Unity.Preview preview (string uri)
    {
      debug ("Previewing: %s",  uri);
      Unity.GenericPreview preview = null;

      string real_uri;
      if (Bookmark.is_bookmark_uri (uri))
      {
        real_uri = Bookmark.uri_from_bookmark (uri);
      }
      else if (Device.is_device_uri (uri))
      {
        preview = create_device_preview (uri);
        return preview;
      }
      else
      {
        real_uri = uri;
      }

      var file = GLib.File.new_for_uri (real_uri);
      if (file != null)
      {
        GLib.FileInfo finfo = null;
        try
        {
          finfo = file.query_info ("*", GLib.FileQueryInfoFlags.NONE);
        }
        catch (GLib.Error e)
        {
          warning ("Error getting file info for '%s': %s", uri, e.message);
        }

        if (finfo != null)
        {
          preview = new Unity.GenericPreview (finfo.get_name (), "", null);
          if (Bookmark.is_bookmark_uri (uri))
          {
            preview.image_source_uri = Bookmark.uri_from_bookmark (uri);
          }
          else
          {
            preview.image_source_uri = uri;
          }

          GLib.TimeVal mtime = finfo.get_modification_time ();
          var date_time = new GLib.DateTime.from_timeval_local (mtime);
          if (date_time != null)
          {
            preview.subtitle = date_time.format ("%x, %X");
          }

          var email_app_info = GLib.AppInfo.get_default_for_uri_scheme ("mailto");

          string mime_type = finfo.get_content_type ();
          preview.add_info (new InfoHint ("format", _("Format"), null, GLib.ContentType.get_description (mime_type)));

          var open_action = new Unity.PreviewAction ("open", _("Open"), null);
          open_action.activated.connect (on_preview_open);
          preview.add_action (open_action);

          uint64 total_size = 0;
          if (finfo.get_file_type () == GLib.FileType.DIRECTORY)
          {
            if (file.get_path () != null)
            {
              int count = 0;
              try
              {
                count_directory_items (file.get_path (), ref count, ref total_size);
              }
              catch (GLib.Error e)
              {
                warning ("Error encounted when counting directory items for path '%s': %s", file.get_path(), e.message);
              }
              preview.add_info (new InfoHint ("size", _("Size"), null, _("%s, %u items").printf (GLib.format_size (total_size), count)));
            }
            else
            {
              warning ("No such uri '%s'\n", uri);
            }
          }
          else // single file preview
          {
            total_size = finfo.get_size ();
            preview.add_info (new InfoHint ("size", _("Size"), null, GLib.format_size (total_size)));

            var open_folder_action = new Unity.PreviewAction ("open-dir", _("Show in Folder"), null);
            open_folder_action.activated.connect (on_preview_open_folder);
            preview.add_action (open_folder_action);

            if (email_app_info != null)
            {
              var email_action = new Unity.PreviewAction ("email", _("Email"), null);
              email_action.activated.connect (on_email_file);
              preview.add_action (email_action);
            }
          }

          try
          {
            var path = Path.get_dirname (Filename.from_uri (real_uri));
            var home = Environment.get_home_dir ();

            if (path.has_prefix (home))
            {
              var relative_path = path.substring (home.length);

              if (relative_path[0] == '\0')
              {
                path = _("Home");
              }
              else
              {
                path = "~" + relative_path;
              }
            }

            preview.add_info (new InfoHint ("path", _("Path"), null, path));
          }
          catch (Error e)
          {
            warning ("Failed to get path for '%s': %s", real_uri, e.message);
          }
        }
        else
        {
          warning ("Couldn't stat file: '%s'", real_uri);
        }
      }
      else
      {
        warning ("Couldn't access file: '%s'", real_uri);
      }

      return preview;
    }


    private Unity.GenericPreview? create_device_preview (string uri)
    {
      var device = devices.get_device_from_uri (uri);

      if (device == null)
        return null;

      Unity.GenericPreview preview = new Unity.GenericPreview (device.display_name, "", device.icon);

      int contents_count = 0;
      uint64 contents_size = 0;
      string filesystem_type = "";
      uint64 total_capacity = 0;
      uint64 free_capacity = 0;

      try {
        var file = device.get_root_file ();
        var file_info = file.query_filesystem_info (GLib.FileAttribute.FILESYSTEM_SIZE + "," +
                                                    GLib.FileAttribute.FILESYSTEM_TYPE + "," +
                                                    GLib.FileAttribute.FILESYSTEM_FREE, null);

        count_directory_items (file.get_path (), ref contents_count, ref contents_size, false);
        filesystem_type = file_info.get_attribute_as_string (GLib.FileAttribute.FILESYSTEM_TYPE);
        total_capacity = file_info.get_attribute_uint64 (GLib.FileAttribute.FILESYSTEM_SIZE);
        free_capacity = file_info.get_attribute_uint64 (GLib.FileAttribute.FILESYSTEM_FREE);

        contents_size = total_capacity - free_capacity;

      } catch (Error e) {
        warning ("Failed to query filesystem info: %s", e.message);
      }

      preview.subtitle = _("Total capacity %s").printf (GLib.format_size(total_capacity));
      preview.add_info (new InfoHint ("filesytem_type", _("Filesystem type"), null, filesystem_type));
      preview.add_info (new InfoHint ("contents", _("Contents"), null, _("%s, %u items").printf (GLib.format_size (contents_size), contents_count)));

      var open_action = new Unity.PreviewAction ("open", _("Open"), null);
      open_action.activated.connect (on_preview_open);
      preview.add_action (open_action);

      return preview;
    }

    public Unity.ActivationResponse on_email_file (string uri)
    {
      debug (@"Emailing file: $uri");
      var email_app_info = GLib.AppInfo.get_default_for_uri_scheme ("mailto");
      if (email_app_info != null)
      {
        try {
          var files = new GLib.List<File> ();
          files.append (GLib.File.new_for_uri (uri));
          email_app_info.launch (files, null);
        }
        catch (Error e)
        {
          warning ("Couldn't launch email application '%s': '%s'", email_app_info.get_executable (), e.message);
        }
      }
      return new Unity.ActivationResponse(Unity.HandledType.HIDE_DASH);
    }

    public Unity.ActivationResponse on_preview_open (string uri)
    {
      try
      {
        if (bookmarks.launch_if_bookmark (uri) || devices.launch_if_device (uri) || GLib.AppInfo.launch_default_for_uri (uri, null))
        {
          return new Unity.ActivationResponse(Unity.HandledType.HIDE_DASH);
        }
      }
      catch (GLib.Error e)
      {
        warning ("Failed to launch default application for uri '%s': %s", uri, e.message);
      }
      return new Unity.ActivationResponse(Unity.HandledType.NOT_HANDLED);
    }

    public Unity.ActivationResponse on_preview_open_folder (string uri)
    {
      var file_manager = AppInfo.get_default_for_type("inode/directory", true);
      var uris = new GLib.List<string> ();
      uris.append (uri);

      try
      {
        if (file_manager.launch_uris (uris, null))
          return new Unity.ActivationResponse(Unity.HandledType.HIDE_DASH);
      }
      catch (GLib.Error e)
      {
        warning ("Failed to launch default application for uri '%s': %s", uri, e.message);
      }
      return new Unity.ActivationResponse(Unity.HandledType.NOT_HANDLED);
    }

    public Unity.ActivationResponse activate (string uri)
    {
      debug (@"Activating: $uri");
      try {
        if (bookmarks.launch_if_bookmark (uri) || devices.launch_if_device (uri))
          return new Unity.ActivationResponse(Unity.HandledType.HIDE_DASH);

        /* this code ensures that a file manager will be used
         * * if uri it's a remote location that should be mounted */
        var url_type = UrlType.UNKNOWN;
        var checked_url = urls.check_url (uri, out url_type);
        if (checked_url != null && url_type == UrlType.MOUNTABLE) {
          var muris = new GLib.List<string>();
          muris.prepend (uri);
          var file_manager = AppInfo.get_default_for_type("inode/directory", true);
          file_manager.launch_uris(muris,null);
          return new Unity.ActivationResponse(Unity.HandledType.HIDE_DASH);
        }
      } catch (GLib.Error error) {
        warning ("Failed to launch URI %s", uri);
      }
      return new Unity.ActivationResponse (Unity.HandledType.NOT_HANDLED);
    }

    private void on_zeitgeist_changed ()
    {
      /* make sure our results are fresh */
      scope.queue_search_changed (SearchType.DEFAULT);
      scope.queue_search_changed (SearchType.GLOBAL);
    }

    private const string ATTRS_TYPE_HIDDEN = FileAttribute.STANDARD_TYPE +
      "," + FileAttribute.STANDARD_IS_HIDDEN;
    private const string ATTRS_TYPE_SIZE_HIDDEN = FileAttribute.STANDARD_TYPE +
      "," + FileAttribute.STANDARD_SIZE +
      "," + FileAttribute.STANDARD_IS_HIDDEN;

    [Flags]
    public enum ResultFlags
    {
      NONE = 0,
      USE_ORIGIN,
      EXTRACT_ORIGINS,
      SKIP_FOLDERS,
    }

    /* Appends a set of Zeitgeist.Events to our Dee.Model assuming that
     * these events are already sorted with descending timestamps */
    public void append_events_sorted (string search_string,
                                      Zeitgeist.ResultSet? events,
                                      Dee.Model results,
                                      int64 min_size, int64 max_size,
                                      ResultFlags flags,
                                      Gee.Set<string>? excluded_uris = null,
                                      int category_override = -1)
    {
      if (events == null) return;
      uint category_id;
      Set<string>? extracted_directories = null;

      if (ResultFlags.EXTRACT_ORIGINS in flags)
      {
        var folded_search = search_string.casefold ();
        var origins = new HashSet<string> ();
        // loop through all found events, take a look at origin field, add it
        // to result set if it matches our search
        // NOTE: using separate zg query with MOST_RECENT_ORIGINS grouping
        // is more likely to find relevant directories, but is much slower
        // than this kind of "approximation"
        foreach (var ev in events)
        {
          if (ev.num_subjects () > 0)
          {
            string origin = ev.get_subject (0).origin;
            if (origin == null || origin == "") continue;
            var f = File.new_for_uri (origin);
            origin = f.get_uri ();
            if (excluded_uris != null && origin in excluded_uris) continue;
            if (!(origin in origins) && f.is_native () && f.query_exists ())
            {
              string? path = f.get_path ();
              if (path != null && path.contains ("/."))
                continue;

              var display_name = Path.get_basename (f.get_parse_name ());
              if (display_name.casefold ().has_prefix (folded_search))
              {
                string mimetype = "inode/directory";
                string icon = Utils.get_icon_for_uri (origin, mimetype);
                if (category_override >= 0)
                  category_id = category_override;
                else
                  category_id = Categories.FOLDERS;
                results.append (origin, icon, category_id, ResultType.PERSONAL,
                                mimetype, display_name, "", origin, empty_asv);
              }
              else
              {
                if (extracted_directories == null)
                {
                  extracted_directories = new HashSet<string> ();
                }
                extracted_directories.add (origin);
              }
              origins.add (origin);
            }
          }
        }
        // we need this because of way ResultSet works with foreach
        if (events.size () > 0) events.reset ();
      }
      foreach (var ev in events)
      {
        if (ev.num_subjects () > 0)
        {
          // FIXME: We only use the first subject...
          Zeitgeist.Subject su = ev.get_subject (0);

          string uri;
          string display_name;
          string mimetype;

          if (ResultFlags.USE_ORIGIN in flags)
          {
            uri = su.origin;
            display_name = "";
            mimetype = "inode/directory";
          }
          else
          {
            uri = su.current_uri;
            display_name = su.text;
            mimetype = su.mimetype;
            mimetype = su.mimetype != null ?
                       su.mimetype : "application/octet-stream";
          }
          if (uri == null) continue;
          File file = File.new_for_uri (uri);
          if (excluded_uris != null && file.get_uri () in excluded_uris) continue;

          if (display_name == null || display_name == "")
          {
            display_name = Path.get_basename (file.get_parse_name ());
          }

          bool check_size = min_size > 0 || max_size < int64.MAX;
          /* Don't check existence on non-native files as http:// and
           * friends are *very* expensive to query */
          FileType file_type = FileType.UNKNOWN;
          if (file.is_native ())
          {
            // hidden files should be ignored
            try
            {
              FileInfo info = file.query_info (check_size ?
                ATTRS_TYPE_SIZE_HIDDEN : ATTRS_TYPE_HIDDEN, 0, null);
              string? path = file.get_path ();
              if (path != null && path.contains ("/."))
                continue;
              if (check_size &&
                (info.get_size () < min_size || info.get_size () > max_size))
              {
                continue;
              }
              file_type = info.get_file_type ();
            }
            catch (GLib.Error e)
            {
              // as error occurred file must be missing therefore ignoring it
              continue;
            }
          }
          string icon = Utils.get_icon_for_uri (uri, mimetype);
          string comment = file.get_parse_name ();

          var is_dir = file_type == FileType.DIRECTORY;
          if (is_dir && ResultFlags.SKIP_FOLDERS in flags) continue;

          if (category_override >= 0)
            category_id = category_override;
          else
            category_id = is_dir ? Categories.FOLDERS : Categories.RECENT;

          results.append (uri, icon, category_id, ResultType.PERSONAL, mimetype,
                          display_name, comment, uri, empty_asv);

        }
      }

      if (extracted_directories != null)
      {
        foreach (var dir_uri in extracted_directories)
        {
          var f = File.new_for_uri (dir_uri);
          var display_name = Path.get_basename (f.get_parse_name ());
          string mimetype = "inode/directory";
          string icon = Utils.get_icon_for_uri (dir_uri, mimetype);
          if (category_override >= 0)
            category_id = category_override;
          else
            category_id = Categories.FOLDERS;
          results.append (dir_uri, icon, category_id, ResultType.PERSONAL,
                          mimetype, display_name, "", dir_uri, empty_asv);
        }
      }
    }
  }
} /* namespace */
