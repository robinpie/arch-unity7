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
 *
 */

namespace Unity.VideoLens
{
  public class Locate
  {
    /* Filter used by locate handling loop; return value of false means file will be ignored */
    public delegate bool LocateFilter (string path);

    private static const int MAX_RESULTS = 100;
    private static const int MAX_LOCATE_RESULTS = 200;

    private string cache_db;
    private string videos_folder;
    public string locate_bin { get; set; default = "locate"; }
    public string updatedb_bin { get; set; default = "updatedb"; }

    public Locate (string cache_dir, string videos_dir)
    {
      cache_db = cache_dir + "/videos.db";
      videos_folder = videos_dir;
    }

    public Gee.ArrayList<VideoFile?>? run_locate (string search_string, Thumbnailer thumbnailer, LocateFilter? filter = null)
    {
      if (Utils.is_regular_file (cache_db))
      {
        string stdout;
        try
        {
          string query = locate_query_string (search_string);
          GLib.Process.spawn_command_line_sync (@"$locate_bin -l $MAX_LOCATE_RESULTS -id $cache_db $query", out stdout);
          var result_list = parse_locate_results (stdout, 100, thumbnailer, filter);
          return result_list;
        }
        catch (GLib.Error e)
        {
          warning ("Failed to run locate: %s", e.message);
        }
      }
      return null;
    }

    internal string locate_query_string (string search_string)
    {
      return videos_folder + "*" + search_string.replace (" ", "*") + "*";
    }

    public void updatedb ()
    {
      try
      {
        GLib.Process.spawn_command_line_sync (@"$updatedb_bin -o $cache_db -l 0 -U $videos_folder");
      }
      catch (GLib.Error e)
      {
        warning ("Can't create database, will retry: %s", e.message);
      }
    }

    public Gee.ArrayList<VideoFile?> parse_locate_results (string locate_output, int max, Thumbnailer thumbnailer, LocateFilter? filter)
    {
      var results_list = locate_output.split ("\n");
      var res = new Gee.ArrayList<VideoFile?> ();
      int video_count = 0;
      foreach (var video in results_list)
      {
        if (video_count >= MAX_RESULTS)
          break;

        try
        {
          if (Utils.is_video (video) && (filter == null || filter (video)))
          {
            video_count++;
            var name = Utils.get_name (video);
            VideoFile video_file = VideoFile ()
            {
              title = name,
              lc_title = name.down (),
              uri = "file://" + video,
              icon = thumbnailer.get_icon (video)
            };
            res.add (video_file);
          }
        }
        catch (Error e)
        {
          // silently ignore
        }
      }
      return res;
    }
  }
}