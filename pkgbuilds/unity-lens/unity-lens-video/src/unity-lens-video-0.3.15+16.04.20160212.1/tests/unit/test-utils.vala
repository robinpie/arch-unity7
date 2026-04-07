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
  public static int main (string[] args)
  {
    Test.init (ref args);
    Test.add_data_func ("/Utils/Gcd", test_gcd);
    Test.add_data_func ("/Utils/IsRegularFile", test_is_regular_file);
    Test.add_data_func ("/Utils/GetName", test_get_name);
    Test.add_data_func ("/Utils/IsVideo", test_is_video);
    Test.run ();
    return 0;
  }

  internal static void test_gcd ()
  {
    assert (Utils.gcd (1, 1) == 1);
    assert (Utils.gcd (2, 1) == 1);
    assert (Utils.gcd (10, 2) == 2);
    assert (Utils.gcd (2, 10) == 2);
    assert (Utils.gcd (20, 15) == 5);
  }

  internal static void test_is_regular_file ()
  {
    assert (Utils.is_regular_file ("/etc/passwd") == true);
    assert (Utils.is_regular_file ("/dev/null") == false);
    assert (Utils.is_regular_file ("/non-existing-file") == false);
  }

 internal static void test_get_name ()
 {
   assert (Utils.get_name ("/etc/passwd") == "passwd");
 }

 internal static void test_is_video ()
 {
   var video_dir = Config.TESTDATADIR;

   assert (Utils.is_video ("/etc/passwd") == false);
   assert (Utils.is_video (@"$video_dir/video1.avi") == true);
 }
}