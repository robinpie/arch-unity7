/*
 * Copyright (C) 2012 Canonical Ltd
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
 * Authored by Pawel Stolowski <pawel.stolowski@canonical.com>
 * based on python code by David Calle <davidc@framli.eu>
 *
 */

using Config;

namespace Unity.VideoLens
{
  public class VideoScope : Unity.DeprecatedScope
  {
    private static const int MAX_ZG_EVENTS = 24;
    private static const int CAT_INDEX_MY_VIDEOS = 0;
    private static const int CAT_INDEX_ONLINE = 1;
    private static const int REFRESH_TIMEOUT = 30;

    private static string cache_directory;

    private string videos_folder;
    private Unity.Extras.PreviewPlayer preview_player;
    private Thumbnailer thumbnailer;
    private Locate locate;
    private BlacklistTracker blacklist_tracker;

    private Variant empty_asv;

    public VideoScope ()
    {
      Object (dbus_path: "/net/launchpad/scope/localvideos", id: "video-local.scope");
      empty_asv = new Variant.array (VariantType.VARDICT.element (), {});

      videos_folder = GLib.Environment.get_user_special_dir (GLib.UserDirectory.VIDEOS);
      cache_directory = GLib.Environment.get_user_cache_dir () + "/unity-lens-video";

      // create cache directory
      try
      {
        var cache_dir = GLib.File.new_for_path (cache_directory);
        if (! cache_dir.query_exists (null))
          cache_dir.make_directory (null);
      }
      catch (Error e)
      {
        error ("Failed to create cache directory: %s", e.message);
      }

      blacklist_tracker = new BlacklistTracker ();
      thumbnailer = new Thumbnailer (cache_directory);
      locate = new Locate (cache_directory, videos_folder);

      search_in_global = true;
      search_hint = _("Search videos");
      visible = true;
      sources.add_option ("local", _("My videos"), null);
      populate_categories ();

      GLib.Timeout.add_seconds (REFRESH_TIMEOUT, refresh_results);

      search_changed.connect ((search, search_type, cancellable) =>
      {
        dispatch_search (search, search_type, cancellable);
      });
    }

    private void populate_categories ()
    {
      var categories = new Unity.CategorySet ();
      var icon_dir = File.new_for_path (Config.ICON_PATH);

      categories.add(new Unity.Category ("local", _("My videos"), new FileIcon (icon_dir.get_child ("group-videos.svg")), Unity.CategoryRenderer.VERTICAL_TILE));
      categories.add(new Unity.Category ("online", _("Online"), new FileIcon (icon_dir.get_child ("group-internet.svg")), Unity.CategoryRenderer.VERTICAL_TILE));

      this.categories = categories;
    }

    private bool refresh_results ()
    {
      debug ("Queuing new search because of timeout");
      queue_search_changed (Unity.SearchType.DEFAULT);
      return true;
    }

    private async void dispatch_search (DeprecatedScopeSearch search, SearchType search_type, GLib.Cancellable cancellable)
    {
      var search_string = search.search_string.strip ();
      var search_status = search;
      var model = search.results_model;
      debug ("Search changed to '%s'", search_string);

      if (source_activated ("local"))
      {
        if (search_type == Unity.SearchType.GLOBAL)
        {
          if (search_string == "")
          {
            model.clear ();
            if (search != null)
              search_status.finished ();
            debug ("Global view without search string : hide");
          }
          else
          {
            update_results_model (search_string, model, "global", cancellable, search_status);
          }
        }
        else
        {
          if (search_string == null || search_string == "")
          {
            try
            {
              zg_call (cancellable, search_status);
            }
            catch (GLib.Error e)
            {
              warning ("Failed to call zeitgeist: %s", e.message);
            }
          }
          else
          {
            update_results_model (search_string, model, "lens", cancellable, search_status);
          }
        }
      }
    }

    private void update_results_model (string search_string, Dee.Model model, string cat, GLib.Cancellable? cancellable, DeprecatedScopeSearch search, bool clear_model = true)
    {
      var home_folder = GLib.Environment.get_home_dir ();

      if (videos_folder != home_folder)
      {
        locate.updatedb ();
      }

      var result_list = locate.run_locate (search_string, thumbnailer, video_filter);
      if (result_list != null)
      {
        GLib.Idle.add (() =>
        {
          result_list.sort ((a, b) => { return a.lc_title.collate (b.lc_title); });
          add_results (search, model, cat, cancellable, result_list, search_string, clear_model);
          return false;
        });
      }
    }

    internal void add_results (DeprecatedScopeSearch search_status, Dee.Model model, string cat, GLib.Cancellable? cancellable, Gee.ArrayList<VideoFile?> result_list, string search, bool clear_model)
    {
      if (cancellable != null && !cancellable.is_cancelled ())
      {
        var results_model = search_status.results_model;
        if (clear_model)
          results_model.clear ();

        foreach (var video in result_list)
        {
          results_model.append (video.uri, video.icon, video.category, ResultType.PERSONAL, "text/html", video.title, video.comment, video.uri, empty_asv);
        }

        if (search_status != null)
        {
          debug ("Search finished");
          search_status.finished ();
        }
      }
    }

    private async void zg_call (GLib.Cancellable? cancellable, DeprecatedScopeSearch search_status) throws Error
    {
      bool active = sources.get_option ("local").active;
      bool filtering = sources.filtering;
      string uri = active && filtering ? "file:*" : "*";

      var time_range = new Zeitgeist.TimeRange.to_now ();
      var event_template = new Zeitgeist.Event ();
      var subject = new Zeitgeist.Subject.full (uri, Zeitgeist.NFO_VIDEO, "", "", "", "", "");
      event_template.add_subject (subject);

      var templates = new PtrArray.sized (1);
      templates.add ((event_template as GLib.Object).ref());
      var results = yield Zeitgeist.Log.get_default ().find_events (time_range, templates, Zeitgeist.StorageState.ANY, MAX_ZG_EVENTS, Zeitgeist.ResultType.MOST_RECENT_SUBJECTS, cancellable);
      process_zg_events (results, cancellable, search_status);
    }

    internal bool video_filter (string path)
    {
      try
      {
        if (Utils.is_video (path) && !Utils.is_hidden (path))
        {
          var file = File.new_for_path (path);
          if (!is_blacklisted(file.get_uri ()))
            return true;
        }
      }
      catch (Error e)
      {
        warning ("Failed to get properties of '%s': %s", path, e.message);
      }
      return false;
    }

    internal void process_zg_events (Zeitgeist.ResultSet events, GLib.Cancellable cancellable, DeprecatedScopeSearch search_status)
    {
      var result_list = new Gee.ArrayList<VideoFile?> ();

      foreach (var event in events)
      {
        if (cancellable.is_cancelled ())
          return;

        var event_uri = event.get_subject (0).get_uri ();
        if (event_uri.has_prefix ("file://"))
        {
          try
          {
            // If the file is local, we use the same methods
            // as other result items.
            var file = GLib.File.new_for_uri (event_uri);
            var path = file.get_path ();
            if (video_filter (path))
            {
              var finfo = file.query_info (GLib.FileAttribute.STANDARD_DISPLAY_NAME, GLib.FileQueryInfoFlags.NONE, null);
              VideoFile video = VideoFile ()
              {
                title = finfo.get_display_name (),
                lc_title = finfo.get_display_name ().down (),
                comment = "",
                uri = event_uri,
                icon = thumbnailer.get_icon (path),
                category = CAT_INDEX_MY_VIDEOS
              };
              result_list.add (video);
            }
          }
          catch (GLib.Error e)
          {
            warning ("Failed to access file '%s': %s", event_uri, e.message);
          }
        }
        else if (event_uri.has_prefix ("http"))
        {
          // If the file is distant, we take
          // all we need from Zeitgeist
          // this one can be any unicode string:
          VideoFile video = VideoFile ()
          {
            title = event.get_subject (0).get_text (),
            comment = "",
            uri = event_uri,
            icon = event.get_subject (0).get_storage (),
            category = CAT_INDEX_ONLINE
          };
          result_list.add (video);
        }
      }

      var results_model = search_status.results_model;
      results_model.clear ();
      foreach (var video in result_list)
      {
        results_model.append (video.uri, video.icon, video.category, ResultType.PERSONAL, "text/html", video.title, video.comment, video.uri, empty_asv);
      }

      update_results_model ("", search_status.results_model, "lens", cancellable, search_status, false);
    }

    private Unity.ActivationResponse show_in_folder (string uri)
    {
      try
      {
        Unity.Extras.show_in_folder (uri);
        return new Unity.ActivationResponse (Unity.HandledType.HIDE_DASH);
      }
      catch (GLib.Error e)
      {
        warning ("Failed to show in folder '%s': %s", uri, e.message);
      }
      return new Unity.ActivationResponse (Unity.HandledType.NOT_HANDLED);
    }

    public override async Preview? preview_result (ScopeResult result)
    {
      debug ("Preview uri: %s", result.uri);

      Unity.MoviePreview preview = null;
      string subtitle = "";
      int64 file_size = 0;
      bool local_video = result.uri.has_prefix ("file://");

      if (local_video)
      {
        var file = GLib.File.new_for_uri (result.uri);
        try
        {
          var finfo = file.query_info (GLib.FileAttribute.TIME_MODIFIED + "," + GLib.FileAttribute.STANDARD_SIZE, GLib.FileQueryInfoFlags.NONE, null);
          file_size = finfo.get_size ();
          var tval = finfo.get_modification_time ();
          var dt = new DateTime.from_timeval_local (tval);
          subtitle = dt.format ("%x, %X");
        }
        catch (Error e)
        {
          // empty subtitle
        }
      }

      preview = new Unity.MoviePreview (result.title, subtitle, result.comment, null);
      preview.set_rating (-1.0f, 0);
      var play_video = new Unity.PreviewAction ("play", _("Play"), null);
      preview.add_action (play_video);
      preview.image_source_uri = result.icon_hint;

      if (local_video)
      {
        var show_folder = new Unity.PreviewAction ("show-in-folder", _("Show in Folder"), null);
        show_folder.activated.connect (show_in_folder);
        preview.add_action (show_folder);
      }

      // we may get remote uris from zeitgeist - fetch details for local files only
      if (local_video)
      {
        preview.image_source_uri = result.uri;

        if (preview_player == null)
          preview_player = new Unity.Extras.PreviewPlayer ();

        try
        {
          var props = yield preview_player.video_properties (result.uri);
          if ("width" in props && "height" in props && "codec" in props)
          {
            var width = props["width"].get_uint32 ();
            var height = props["height"].get_uint32 ();
            var codec = props["codec"].get_string ();
            string dimensions = "%u*%u".printf (width, height);
            if (width > 0 && height > 0)
            {
              var gcd = Utils.gcd (width, height);
              dimensions += ", %u:%u".printf (width / gcd, height / gcd);
            }

            preview.add_info (new Unity.InfoHint ("format", _("Format"), null, codec));
            preview.add_info (new Unity.InfoHint ("dimensions", _("Dimensions"), null, dimensions));
            preview.add_info (new Unity.InfoHint ("size", _("Size"), null, GLib.format_size (file_size)));
          }
        }
        catch (Error e)
        {
          warning ("Couldn't get video details: %s", e.message);
        }
        try
        {
          yield preview_player.close();
        }
        catch (Error e)
        {
          warning ("Failed to close preview player: %s", e.message);
        }
      }

      return preview;
    }

    private bool is_blacklisted (string uri)
    {
      foreach (var blacklisted_uri in blacklist_tracker.get_blacklisted_uris ())
      {
        if (uri.has_prefix (blacklisted_uri))
          return true;
      }
      return false;
    }

    private bool source_activated (string source)
    {
      // Return the state of a sources filter option
      var active = sources.get_option (source).active;
      var filtering = sources.filtering;
      return (active && filtering) || (!active && !filtering);
    }

  }
}
