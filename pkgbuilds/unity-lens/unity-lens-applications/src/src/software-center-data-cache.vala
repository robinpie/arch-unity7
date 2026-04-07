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

namespace Unity.ApplicationsLens {

public class SoftwareCenterDataCache: SoftwareCenterDataProviderProxy
{
  public int64 category_items_lifetime { get; set; }
  private HashTable<string, int64?> category_items_last_update;
  private HashTable<string, Gee.ArrayList<SoftwareCenterData.AppInfo?>> category_items_cached;

  public SoftwareCenterDataCache (int64 category_items_lifetime)
  {
    category_items_last_update = new HashTable<string, int64?> (str_hash, str_equal);
    category_items_cached = new HashTable<string, Gee.ArrayList<SoftwareCenterData.AppInfo?>> (str_hash, str_equal);
    this.category_items_lifetime = category_items_lifetime;
  }

  internal static bool outdated (ref int64 last_update, int64 lifetime)
  {
    var current_time = new GLib.DateTime.now_utc ();
    int64 current_unix_time = current_time.to_unix ();
    if (current_unix_time > last_update + lifetime)
    {
      last_update = current_unix_time;
      return true;
    }
    return false;
  }

  public override async SoftwareCenterData.AppInfo?[] get_items_for_category (string category_name) throws Error
  {
    int64 last_update = 0;
    if (category_items_last_update.contains (category_name))
    {
      last_update = category_items_last_update[category_name];
    }

    if (outdated (ref last_update, category_items_lifetime))
    {
      category_items_last_update[category_name] = last_update;

      var data = yield base.get_items_for_category (category_name);
      var results = new Gee.ArrayList<SoftwareCenterData.AppInfo?> ();
      foreach (var item in data)
      {
        results.add (item);
      }
      category_items_cached[category_name] = results;
      return data;
    }
    return category_items_cached[category_name].to_array ();
  }
}

}
