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
  public class SoftwareCenterDataProviderProxy: GLib.Object
  {
    private HashTable<string, Gee.ArrayList<SoftwareCenterData.AppInfo?>> data;

    public SoftwareCenterDataProviderProxy ()
    {
      data = new HashTable<string, Gee.ArrayList<SoftwareCenterData.AppInfo?>> (str_hash, str_equal);
      data["unity-top-rated"] = new Gee.ArrayList<SoftwareCenterData.AppInfo?> ();
      data["unity-whats-new"] = new Gee.ArrayList<SoftwareCenterData.AppInfo?> ();
    }

    public void mock_add (string category, string appname, string pkgname)
    {
      var app = SoftwareCenterData.AppInfo () {
        application_name = appname,
        package_name = pkgname,
        icon = null,
        desktop_file = null
      };
      data[category].add (app);
    }

    public void mock_clear (string category)
    {
      data[category].clear ();
    }

    public virtual async SoftwareCenterData.AppInfo?[] get_items_for_category (string category_name) throws Error
    {
      return data[category_name].to_array ();
    }
  }
}
