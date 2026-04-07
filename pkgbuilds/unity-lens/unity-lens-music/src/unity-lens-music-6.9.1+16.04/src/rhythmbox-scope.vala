/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Authored by Alex Launi <alex.launi@canonical.com>
 *
 */

using GLib;

namespace Unity.MusicLens {

  public class RhythmboxScope : SimpleScope
  {
    private RhythmboxCollection collection;
    private bool db_ready;
    private FileMonitor rb_xml_monitor;
    private Unity.MusicPreview? music_preview;

    public RhythmboxScope ()
    {
      base ();

      scope = new Unity.DeprecatedScope ("/com/canonical/unity/scope/rhythmbox", "music-rhythmbox.scope");
      scope.search_in_global = true;
      scope.search_hint = _("Search music");
      scope.visible = true;

      scope.activate_uri.connect (activate);
      scope.preview_uri.connect (preview);
      scope.sources.add_option ("rhythmbox", _("Rhythmbox"), null);

      base.initialize ();
      collection = new RhythmboxCollection ();
      db_ready = false;
    }

    protected override int num_results_without_search { get {return 100; } }
    protected override int num_results_global_search { get { return 20; } }
    protected override int num_results_lens_search { get { return 50; } }

    private async void prepare_rhythmbox_play_queue (string[] exec_args)
    {
      var main_exec = exec_args; // copy arguments for thread's benefit
      try {
        new Thread<void*>.try ("rhythmbox-thread", () =>
        {
          try
          {
            Process.spawn_command_line_sync ("rhythmbox-client --pause");
            Process.spawn_command_line_sync ("rhythmbox-client --clear-queue");
            Process.spawn_sync (null, main_exec, null, SpawnFlags.SEARCH_PATH,
                                null, null);
            Process.spawn_command_line_sync ("rhythmbox-client --next");
            // make sure we're playing
            Process.spawn_command_line_sync ("rhythmbox-client --play");
          }
          catch (Error err)
          {
            warning ("%s", err.message);
          }
          Idle.add (prepare_rhythmbox_play_queue.callback);
          return null;
        });
      }
      catch (Error err)
      {
        warning ("Could not create rhythmbox-thread: %s", err.message);
      }
      yield;
    }

    public Unity.Preview preview (string uri)
    {
      music_preview = null;
      GLib.Icon? icon_file = null;
      GLib.Icon? missing_icon_file = new GLib.FileIcon (File.new_for_path (ALBUM_MISSING_PREVIEW_ICON_PATH));

      if (Uri.parse_scheme (uri) == "album")
      {
        string album_key = uri.substring (8);

        foreach (unowned Track track in collection.get_album_tracks_detailed (album_key))
        {
          if (music_preview == null)
          {
            var artwork_path = track.artwork_path;
            if (artwork_path == null || artwork_path == "")
              icon_file = missing_icon_file;
            else
              icon_file = new GLib.FileIcon(File.new_for_path (artwork_path));

            music_preview = new Unity.MusicPreview (track.album, track.album_artist, icon_file);

            var play_action = new Unity.PreviewAction ("play_album", _("Play"), null);
            play_action.activated.connect (play_album_preview);
            music_preview.add_action (play_action);

            var show_folder_action = new Unity.PreviewAction ("show_in_folder", _("Show in Folder"), null);
            show_folder_action.activated.connect (show_in_folder);
            music_preview.add_action (show_folder_action);
          }
          var tm = new Unity.TrackMetadata();
          tm.uri = track.uri;
          tm.track_no = track.track_number;
          tm.title = track.title;
          tm.length = track.duration;
          music_preview.add_track(tm);
        }
      }
      else if (Uri.parse_scheme (uri) == "file") // preview single track, ignore radio stations
      {
        Track? track = collection.get_album_track (uri);
        if (track != null)
        {
          var artwork_path = track.artwork_path;
          if (artwork_path == null || artwork_path == "")
            icon_file = missing_icon_file;
          else
            icon_file = new GLib.FileIcon(File.new_for_path (artwork_path));
          music_preview = new Unity.MusicPreview (track.title, track.album_artist, icon_file);

          var play_action = new Unity.PreviewAction ("play_track", _("Play"), null);
          play_action.activated.connect (play_track_in_rhythmbox);
          music_preview.add_action (play_action);

          var show_folder_action = new Unity.PreviewAction ("show_in_folder", _("Show in Folder"), null);
          show_folder_action.activated.connect (show_in_folder);
          music_preview.add_action (show_folder_action);

          var tm = new Unity.TrackMetadata();
          tm.uri = track.uri;
          tm.track_no = track.track_number;
          tm.title = track.title;
          tm.length = track.duration;
          music_preview.add_track(tm);
        }
      }

      return music_preview;
    }

    private Unity.ActivationResponse show_in_folder (string uri)
    {
      string? file_uri = null;

      if (Uri.parse_scheme (uri) == "album")
      {
        string album_key = uri.substring (8);
        var tracks = collection.get_album_tracks (album_key);
        if (tracks.length () > 0)
        {
          file_uri = tracks.nth_data (0);
        }
        else
        {
          warning ("Album '%s' has no tracks, unable to get uri", uri);
        }
      }
      else //file
      {
        file_uri = uri;
      }

      if (file_uri != null)
      {
        Unity.Extras.show_in_folder.begin (file_uri);
        return new Unity.ActivationResponse (Unity.HandledType.HIDE_DASH);
      }

      return new Unity.ActivationResponse (Unity.HandledType.NOT_HANDLED);
    }

    private Unity.ActivationResponse play_track_in_rhythmbox (string uri)
    {
      return activate (uri);
    }

    private Unity.ActivationResponse play_album_preview (string uri)
    {
      return activate (uri);
    }

    /**
     * Tells rhythmbox to play the selected uri(s)
     */
    public Unity.ActivationResponse activate (string uri)
    {
      string[] exec = {"rhythmbox-client"};

      try
      {
        if (Uri.parse_scheme (uri) == "album")
        {
            // this will be a bit crazy, let's do it in a separate thread
            exec += "--enqueue";
            string album_key = uri.substring (8);
            foreach (unowned string track_uri in collection.get_album_tracks (album_key))
            {
                exec += track_uri;
            }
            exec += null;
            prepare_rhythmbox_play_queue.begin (exec);
            return new Unity.ActivationResponse (Unity.HandledType.HIDE_DASH);
        }
        else
        {
            exec += "--play-uri";
            exec += uri;
        }

        exec += null;

        debug ("Spawning rb '%s'", string.joinv (" ", exec));
        Process.spawn_async (null,
                 exec,
                 null,
                 SpawnFlags.SEARCH_PATH,
                 null,
                 null);

        return new Unity.ActivationResponse (Unity.HandledType.HIDE_DASH);
      } catch (SpawnError error) {
        warning ("Failed to launch URI %s", uri);
        return new Unity.ActivationResponse (Unity.HandledType.NOT_HANDLED);
      }
    }

    public override async void perform_search (DeprecatedScopeSearch search,
                                               SearchType search_type,
                                               owned List<FilterParser> filters,
                                               int max_results = -1,
                                               GLib.Cancellable? cancellable = null)
    {
      int category_override = -1;
      if (search_type == SearchType.GLOBAL)
      {
        category_override = Category.MUSIC;
        // the lens shouldn't display anything for empty search
        if (is_search_empty (search)) return;
      }

      if (!db_ready)
      {
        // parse the DB lazily
        var tdb_path = Path.build_filename (Environment.get_user_cache_dir (),
                                            "rhythmbox", "album-art",
                                            "store.tdb");
        collection.parse_metadata_file (tdb_path);

        var xml_path = Path.build_filename (Environment.get_user_data_dir (),
                                            "rhythmbox", "rhythmdb.xml");
        collection.parse_file (xml_path);
        if (rb_xml_monitor == null)
        {
          // re-parse the file if it changes
          File xml_file = File.new_for_path (xml_path);
          try
          {
            rb_xml_monitor = xml_file.monitor (FileMonitorFlags.NONE);
            rb_xml_monitor.changed.connect (() => { db_ready = false; });
          }
          catch (Error err)
          {
            warning ("%s", err.message);
          }
        }

        db_ready = true;
      }

      collection.search (search, search_type, filters,
                         max_results, category_override);
    }
  }
}
