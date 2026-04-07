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
 */

namespace Unity.VideoLens
{
  private Thumbnailer thumbnailer;
  private Locate locate;
  private string video_dir;
  private string user_videos_folder;

  public static int main (string[] args)
  {
    video_dir = Config.TESTDATADIR;
    user_videos_folder = GLib.Environment.get_user_special_dir (GLib.UserDirectory.VIDEOS);

    locate = new Locate ("cache", video_dir);
    thumbnailer = new Thumbnailer ();

    Test.init (ref args);
    Test.add_data_func ("/Locate/ParseLocateResults", test_parse);
    Test.add_data_func ("/Locate/LocateQueryString", test_query_string);
    Test.run ();
    return 0;
  }

  internal static void test_parse ()
  {
    string input = @"$video_dir/video1.avi\n$video_dir/video2.avi\n$video_dir/video1.mpg\n$video_dir/video2.mpg\n";
    var results = locate.parse_locate_results (input, 1000, thumbnailer, (path) => { return true; });
    assert (results.size == 4);

    /* Filter out all results */
    results = locate.parse_locate_results (input, 1000, thumbnailer, (path) => { return false; });
    assert (results.size == 0);
  }

  internal static void test_query_string ()
  {
    assert (locate.locate_query_string ("foo") == video_dir + "*foo*");
    assert (locate.locate_query_string ("foo bar") == video_dir + "*foo*bar*");
  }
}