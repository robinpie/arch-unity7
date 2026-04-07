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

namespace PreviewPlayer {
  
  static const string DASH_DBUS_NAME = "com.canonical.Unity";
  static const string DASH_DBUS_PATH = "/com/canonical/Unity/Dash";

  [DBus (name = "com.canonical.Unity.Dash")]
  public interface DashInterface: GLib.Object
  {
    public abstract async void hide_dash () throws Error;
  }
  
  public class DashProxy: GLib.Object
  {
    public async void connect_to () throws Error
    {
      _dash_interface = Bus.get_proxy_sync (BusType.SESSION, DASH_DBUS_NAME, DASH_DBUS_PATH);
    }

    public async void hide_dash () throws Error
    {
      if (_dash_interface == null)
      {
        yield connect_to ();
      }
      yield _dash_interface.hide_dash ();
    }
    
    private DashInterface _dash_interface;
  }
}
