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
  
  static const string LAUNCHER_DBUS_NAME = "com.canonical.Unity.Launcher";
  static const string LAUNCHER_DBUS_PATH = "/com/canonical/Unity/Launcher";

  [DBus (name = "com.canonical.Unity.Launcher")]
  public interface LauncherService: GLib.Object
  {
	public abstract async void add_launcher_item_from_position (string title, string icon, int icon_x, int icon_y, int icon_size, string desktop_file, string aptdaemon_task) throws IOError;
  }

  public class LauncherProxy: GLib.Object
  {
	public void connect_to_launcher () throws IOError
	{
	  _launcher_service = Bus.get_proxy_sync (BusType.SESSION, LAUNCHER_DBUS_NAME, LAUNCHER_DBUS_PATH);
	}

    public async void add_launcher_item_from_position (string title, string icon, int icon_x, int icon_y, int icon_size, string desktop_file, string aptdaemon_task) throws IOError
    {
      yield _launcher_service.add_launcher_item_from_position (title, icon, icon_x, icon_y, icon_size, desktop_file, aptdaemon_task);
    }

	private LauncherService _launcher_service;
  }

}