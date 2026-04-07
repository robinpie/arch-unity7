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

namespace Unity.ApplicationsLens.SoftwareCenterData
{
  public enum PackageState
  {
    UNINSTALLED,
    INSTALLED,
    NEEDS_PURCHASE,
    UNKNOWN
  }

  public struct AppInfo
  {
    string application_name;
    string package_name;
    string icon;
    string desktop_file;
  }

  public class AppDetailsData
  {
    public string name { get; set; }
    public string summary { get; set; }
    public string description { get; set; }
    public string version { get; set; }
    public string screenshot { get; set; }
    public string desktop_file { get; set; }
    public string license { get; set; }
    public string icon { get; set; }
    public string icon_url { get; set; }
    public string price { get; set; }
    public string raw_price { get; set; }
    public PackageState pkg_state { get; set; }
    public string installation_date { get; set; }
    public string website { get; set; }
    public int64 size { get; set; }
    public string hardware_requirements { get; set; }
    public bool is_desktop_dependency { get; set; }

    public AppDetailsData (HashTable<string, Variant> data)
    {
      from_ht (data);
    }

    public void from_ht (HashTable<string, Variant> data)
    {
      name = data["name"].get_string ();
      summary = data["summary"].get_string ();
      description = data["description"].get_string ();
      version = data["version"].get_string ();
      desktop_file = data["desktop_file"].get_string ();
      license = data["license"].get_string ();
      icon = data["icon_file_name"].get_string ();
      icon_url = data["icon_url"].get_string ();
      price = data["price"].get_string ();
      raw_price = data["raw_price"].get_string ();
      installation_date = data["installation_date"].get_string ();
      website = data["website"].get_string ();
      hardware_requirements = data["hardware_requirements"].get_string ();
      size = int64.parse (data["size"].get_string ());
      is_desktop_dependency = data["is_desktop_dependency"].get_boolean ();
      
      var state = data["pkg_state"].get_string ();

      switch (state)
      {
        case "installed": 
          pkg_state = PackageState.INSTALLED;
          break;
        case "uninstalled":
          pkg_state = PackageState.UNINSTALLED;
          break;
        case "needs_purchase":
          pkg_state = PackageState.NEEDS_PURCHASE;
          break;
        default:
          pkg_state = PackageState.UNKNOWN;
          break;
      }

      screenshot = null;

      if (data.contains ("screenshots"))
      {
        var screenshot_var = data["screenshots"].get_child_value (0).lookup_value ("large_image_url", VariantType.STRING);
        if (screenshot_var != null)
        {
          screenshot = screenshot_var.get_string ();
        }
      }
      if (screenshot == null)
      {
        screenshot = "";
      }
    }
  }
}