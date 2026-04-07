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

  class PurchaseInfoHelper
  {
    public struct AppInfo
    {
      string formatted_price;
      bool paid;
    }

    private HashTable<string, AppInfo?> data;

    public PurchaseInfoHelper ()
    {
      data = new HashTable<string, AppInfo?> (str_hash, str_equal);
    }

    public void from_pkgresults (Unity.Package.SearchResult results)
    {
      foreach (unowned Unity.Package.PackageInfo pkg in results.results)
      {
        var app_info = from_pkgresult (pkg);
        data.insert (pkg.application_name + "/" + pkg.package_name, app_info);
      }
    }

    private AppInfo from_pkgresult (Unity.Package.PackageInfo pkg)
    {
      bool purchased = (pkg.price != null && pkg.price != "" && !pkg.needs_purchase);
      var app_info = AppInfo ()
      {
        formatted_price = pkg.price,
        paid = purchased
      };
      return app_info;
    }

    public GLib.SList<string> create_pkgsearch_query (SoftwareCenterData.AppInfo?[] results)
    {
      var params = new GLib.SList<string> ();
      foreach (var res in results)
      {
        params.append (res.application_name);
        params.append (res.package_name);
      }
      return params;
    }

    public AppInfo? find (string application_name, string package_name)
    {
      string key = application_name + "/" + package_name;
      if (data.contains (key))
      {
        return data[key];
      }
      return null;
    }
  }
}