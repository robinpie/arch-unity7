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

    Test.add_data_func ("/Unit/DesktopIdLookupWithMangling", test_desktop_file_lookup_with_unmangling);
    Test.add_data_func ("/Unit/DesktopIdLookupWithoutMangling", test_desktop_file_lookup_without_unmangling);

    Test.run ();
    return 0;
  }

  internal static void test_desktop_file_lookup_with_unmangling ()
  {
    var lookup = new SoftwareCenterUtils.MangledDesktopFileLookup ();
    string desktop_file = "/foo/bar/kde4:kde4__KCharSelect.desktop";
    var result = lookup.extract_desktop_id (desktop_file, true /* unmangle */);
    
    assert (result == "kde4-KCharSelect.desktop");
    assert (lookup.contains (result) == true);
    assert (lookup.get (result) == "kde4__KCharSelect.desktop");
  }

  internal static void test_desktop_file_lookup_without_unmangling ()
  { 
    var lookup = new SoftwareCenterUtils.MangledDesktopFileLookup ();
    string desktop_file = "/foo/bar/evince.desktop";
    var result = lookup.extract_desktop_id (desktop_file, false);
    assert (result == "evince.desktop");
    assert (lookup.contains (result) == false);
  }
}