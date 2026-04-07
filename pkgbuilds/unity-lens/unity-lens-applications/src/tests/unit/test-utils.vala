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

namespace Unity.ApplicationsLens
{
  public static int main (string[] args)
  {
    Test.init (ref args);

    Test.add_data_func ("/Unit/GetDesktopIdForActor", test_get_desktop_id_for_actor);
    Test.add_data_func ("/Unit/IsSearchEmpty", test_is_search_empty);
    Test.add_data_func ("/Unit/PreprocessString", test_preprocess_string);
    Test.add_data_func ("/Unit/Uncamelcase", test_uncamelcase);
    Test.add_data_func ("/Unit/SubstTilde", test_subst_tilde);

    Test.run ();
    return 0;
  }

  internal static void test_get_desktop_id_for_actor ()
  {
    assert (Utils.get_desktop_id_for_actor ("application://foobar") == "foobar");
    assert (Utils.get_desktop_id_for_actor ("app://foobar") == "foobar");
    assert (Utils.get_desktop_id_for_actor ("/etc/passwd") == "passwd");
    assert (Utils.get_desktop_id_for_actor ("foo.desktop") == "foo.desktop");
    assert (Utils.get_desktop_id_for_actor ("foo") == "foo");
  }

  internal static void test_is_search_empty ()
  {
    assert (Utils.is_search_empty (null) == true);
    assert (Utils.is_search_empty ("") == true);
    assert (Utils.is_search_empty (" ") == false);
  }

  internal static void test_preprocess_string ()
  {
    assert (Utils.preprocess_string ("AisleRiot Solitaire") == "AisleRiot Solitaire\nAisle Riot Solitaire");
    assert (Utils.preprocess_string ("FooBar") == "FooBar\nFoo Bar");
    assert (Utils.preprocess_string ("Foo Bar") == "Foo Bar");
  }

  internal static void test_uncamelcase ()
  {
    assert (Utils.uncamelcase ("FooBar") == "Foo Bar");
    assert (Utils.uncamelcase ("fooBar") == "foo Bar");
    assert (Utils.uncamelcase ("Foo Bar") == "Foo Bar");
    assert (Utils.uncamelcase ("foobar") == "foobar");
    assert (Utils.uncamelcase ("FOoBar") == "FOo Bar");
  }

  internal static void test_subst_tilde ()
  {
    assert (Utils.subst_tilde ("foo") == "foo");

    var home = Environment.get_home_dir ();
    var path = Utils.subst_tilde ("~");
    assert (path == home);

    path = Utils.subst_tilde ("~/foo/bar.txt");
    assert (path == home + "/foo/bar.txt");

    // test ~username expansion
    var username = GLib.Environment.get_user_name ();
    assert (username.length > 0); // sanity check to ensure we're really testing ~username expansion
    path = Utils.subst_tilde ("~" + GLib.Environment.get_user_name ());
    assert (path == home);
  }
}