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
  
  static const string SCDP_DBUS_NAME = "com.ubuntu.SoftwareCenterDataProvider";
  static const string SCDP_DBUS_PATH = "/com/ubuntu/SoftwareCenterDataProvider";

  [DBus (name = "com.ubuntu.SoftwareCenterDataProvider")]
  public interface SoftwareCenterDataProviderService: GLib.Object
  {
    public abstract async HashTable<string, Variant> get_app_details (string appname, string pkgname) throws Error;
    public abstract async string[] get_available_categories () throws Error;
    public abstract async string[] get_available_subcategories () throws Error;
    public abstract async SoftwareCenterData.AppInfo?[] get_items_for_category (string category_name) throws Error;
  }

  public class SoftwareCenterDataProviderProxy: GLib.Object
  {
    public SoftwareCenterDataProviderProxy ()
    {
      Bus.watch_name (BusType.SESSION, SCDP_DBUS_NAME, BusNameWatcherFlags.NONE, null, on_sc_dbus_name_vanished);
    }

    private async void connect_to () throws Error
    {
      _service = yield Bus.get_proxy (BusType.SESSION, SCDP_DBUS_NAME, SCDP_DBUS_PATH);
    }

    private void on_sc_dbus_name_vanished (DBusConnection conn, string name)
    {
      _service = null;
    }

    public async SoftwareCenterData.AppDetailsData get_app_details (string appname, string pkgname) throws Error
    {
      if (_service == null)
        yield connect_to ();

      HashTable<string, Variant> data = yield _service.get_app_details (appname, pkgname);
      var details = new SoftwareCenterData.AppDetailsData (data);
      return details;
    }

    public virtual async SoftwareCenterData.AppInfo?[] get_items_for_category (string category_name) throws Error
    {
      if (_service == null)
        yield connect_to ();

      SoftwareCenterData.AppInfo?[] data = yield _service.get_items_for_category (category_name);
      return data;
    }

    private SoftwareCenterDataProviderService _service;
  }
}
