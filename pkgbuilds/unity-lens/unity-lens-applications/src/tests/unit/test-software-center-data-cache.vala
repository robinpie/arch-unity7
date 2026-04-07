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
  public static const string TOP_RATED = "unity-top-rated";

  public static int main (string[] args)
  {
    Test.init (ref args);

    Test.add_data_func ("/Unit/ItemsForCategoryCache", test_category_cache);

    Test.run ();
    return 0;
  }

  public bool has_app (SoftwareCenterData.AppInfo?[] items, string appname, string pkgname)
  {
    foreach (var item in items)
    {
      if (item.application_name == appname && item.package_name == pkgname)
        return true;
    }
    return false;
  }

  internal static bool run_with_timeout (MainLoop ml, uint timeout_ms = 5000)
  {
    bool timeout_reached = false;
    var t_id = Timeout.add (timeout_ms, () =>
    {
      timeout_reached = true;
      debug ("Timeout reached");
      ml.quit ();
      return false;
    });

    ml.run ();

    if (!timeout_reached) Source.remove (t_id);

    return !timeout_reached;
  }

  internal static SoftwareCenterData.AppInfo?[] get_items (SoftwareCenterDataCache cache, string category)
  {
    var ml = new MainLoop ();
    SoftwareCenterData.AppInfo?[] items = null;
    cache.get_items_for_category.begin (category, (obj, result) =>
      {
        items = cache.get_items_for_category.end (result);
        ml.quit ();
      });
    assert (run_with_timeout (ml));
    return items;
  }

  internal static void test_category_cache ()
  {
    var cache = new SoftwareCenterDataCache (1); // lifetime of cache items = 1s

    // add items to mock base class
    cache.mock_add (TOP_RATED, "Evince", "evince");

    // at this point items are fetched by cache and kept for 1s
    var items = get_items (cache, TOP_RATED);
    assert (items.length == 1);

    // remove & add new items to the mock, but get_items_for_category should still
    // return old cached values.
    cache.mock_clear (TOP_RATED);
    cache.mock_add (TOP_RATED, "Blender", "blender");
    cache.mock_add (TOP_RATED, "Firefox", "firefox");
    cache.mock_add (TOP_RATED, "Thunderbird", "thunderbird");
    var items_2 = get_items (cache, TOP_RATED);
    assert (items_2.length == 1);

    assert (has_app (items_2, "Evince", "evince") == true);

    // wait 3s for cache items to expire
    GLib.Thread.usleep (2 * 1000000);

    // cache will fetch new items now.
    var items_3 = get_items (cache, TOP_RATED);
    assert (items_3.length == 3);
    assert (has_app (items_3, "Blender", "blender") == true);
    assert (has_app (items_3, "Firefox", "firefox") == true);
    assert (has_app (items_3, "Thunderbird", "thunderbird") == true);
  }
}
