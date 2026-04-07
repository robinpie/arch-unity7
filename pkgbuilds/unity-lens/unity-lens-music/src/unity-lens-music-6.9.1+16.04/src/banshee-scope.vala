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

  public class BansheeScopeProxy : SimpleScope
  {
    private BansheeCollection collection;
    private Unity.MusicPreview? music_preview;

    public BansheeScopeProxy ()
    {
      base ();

      scope = new Unity.DeprecatedScope ("/com/canonical/unity/scope/banshee", "music-banshee.scope");
      scope.search_in_global = true;
      scope.search_hint = _("Search music");
      scope.visible = true;

      scope.activate_uri.connect (activate);
      scope.preview_uri.connect (preview);
      scope.sources.add_option ("banshee", _("Banshee"), null);

      base.initialize ();

      try {
	collection = new BansheeCollection ();
      } catch (DatabaseError error) {
	printerr ("Failed to open the Banshee collection database\n");
	return;
      }
    }

    protected override int num_results_without_search { get {return 100; } }
    protected override int num_results_global_search { get { return 20; } }
    protected override int num_results_lens_search { get { return 50; } }

    public Unity.Preview preview (string uri)
    {
      music_preview = null;
      GLib.Icon? icon_file = null;
      GLib.Icon? missing_icon_file = new GLib.FileIcon (File.new_for_path (ALBUM_MISSING_PREVIEW_ICON_PATH));

      if (Uri.parse_scheme (uri) == "album")
      {
        string[] split = uri.split ("/"); //FIXME: there must be more reliable way
        if (split.length >= 4)
        {
          string artist = split[2];
          string title = split[3];

          foreach (unowned Track track in collection.get_album_tracks_detailed (title, artist))
          {
            if (music_preview == null)
            {
              if (track.artwork_path != null && track.artwork_path != "" && FileUtils.test(track.artwork_path, FileTest.EXISTS))
                icon_file = new GLib.ThemedIcon(track.artwork_path);
              else
                icon_file = missing_icon_file;
              music_preview = new Unity.MusicPreview (title, artist, icon_file);
            }
            var tm = new Unity.TrackMetadata();
            tm.uri = track.uri;
            tm.track_no = track.track_number;
            tm.title = track.title ?? "";
            tm.length = track.duration;
            music_preview.add_track(tm);
          }
        }
        else
        {
          warning ("Invalid uri: %s", uri);
        }
      }
      else // preview single track
      {
        Track? track = collection.get_album_track (uri);
        if (track != null)
        {
          if (track.artwork_path != null && track.artwork_path != "" && FileUtils.test(track.artwork_path, FileTest.EXISTS))
            icon_file = new GLib.ThemedIcon(track.artwork_path);
          else
            icon_file = missing_icon_file;
          music_preview = new Unity.MusicPreview (track.title, track.artist, icon_file);
          var tm = new Unity.TrackMetadata();
          tm.uri = track.uri;
          tm.track_no = track.track_number;
          tm.title = track.title ?? "";
          tm.length = track.duration;
          music_preview.add_track(tm);
        }
      }

      if (preview != null)
      {
        var play_action = new Unity.PreviewAction ("play", _("Play"), null);
        play_action.activated.connect (play_in_banshee);
        music_preview.add_action (play_action);

        var show_folder_action = new Unity.PreviewAction ("show_in_folder", _("Show in Folder"), null);
        show_folder_action.activated.connect (show_in_folder);
        music_preview.add_action (show_folder_action);
      }

      return music_preview;
    }

    private Unity.ActivationResponse play_in_banshee (string uri)
    {
      return activate (uri);
    }

    private Unity.ActivationResponse show_in_folder (string uri)
    {
      string? file_uri = null;

      if (Uri.parse_scheme (uri) == "album")
      {
        string[] split = uri.split ("/");
        if (split.length >= 4)
        {
          string artist = split[2];
          string title = split[3];
          var album = new Album ()
          {
            title = title,
            artist = artist
          };
          var tracks = collection.get_track_uris (album);
          if (tracks.length > 0)
          {
            file_uri = tracks[0];
          }
          else
          {
            warning ("Album '%s' has no tracks, unable to get uri", uri);
          }
        }
        else
        {
          warning ("Invalid uri: %s", uri);
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


    /**
     * Tells banshee to play the selected uri(s)
     */
    public Unity.ActivationResponse activate (string uri)
    {
      string[] exec = {"banshee", "--play-enqueued"};

      try {
        if (Uri.parse_scheme (uri) == "album")
          {
            debug (@"searching for tracks for $uri");
            string[] split = uri.split ("/");
            string artist = split[2];
            string title = split[3];

            Album album = new Album ();
            album.artist = artist;
            album.title = title;
            // FIXME there must be a better way..
            foreach (string track in collection.get_track_uris (album))
              exec += track;
          }
        else
          {
            exec += uri;
          }

        exec += null;

        debug (@"Spawning banshee '%s'", string.joinv (" ", exec));
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

      collection.search (search, search_type, filters,
                         max_results, category_override);
    }
  }
}
