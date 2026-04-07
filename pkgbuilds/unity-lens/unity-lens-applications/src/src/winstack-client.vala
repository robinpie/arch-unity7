/* -*- mode: vala; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 * Copyright (C) 2013 Canonical Ltd
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
 *
 */

namespace Unity.ApplicationsLens {

  static const string WINSTACK_DBUS_NAME = "com.canonical.Unity.WindowStack";
  static const string WINSTACK_DBUS_PATH = "/com/canonical/Unity/WindowStack";

  public struct WindowInfo
  {
    uint window_id;
    string app_id;
    bool has_focus;
    uint stage;
  }

  /* Only expose what's required by the scope, e.g. GetAppIdFromPid is not implemented */
  [DBus (name = "com.canonical.Unity.WindowStack")]
  public interface WindowStackService: GLib.Object
  {
    public signal void window_created (uint window_id, string app_id);
    public signal void window_destroyed (uint window_id, string app_id);
    public signal void focused_window_changed (uint window_id, string app_id, uint stage);
    
    public abstract async WindowInfo[] get_window_stack () throws Error;      
  }
  
  public class WindowStackProxy: GLib.Object
  {
    public signal void window_created (uint window_id, string app_id);
    public signal void window_destroyed (uint window_id, string app_id);
    public signal void focused_window_changed (uint window_id, string app_id, uint stage);
    
    private WindowStackProxy ()
    {
    }

    public static WindowStackProxy get_proxy () throws Error
    {
      var proxy = new WindowStackProxy ();
      proxy.service = Bus.get_proxy_sync (BusType.SESSION, WINSTACK_DBUS_NAME, WINSTACK_DBUS_PATH);
      proxy.service.window_created.connect ((win_id, app_id) =>
      {
        proxy.window_created (win_id, app_id);
      });
      proxy.service.window_destroyed.connect ((win_id, app_id) =>
      {
        proxy.window_destroyed (win_id, app_id);
      });
      proxy.service.focused_window_changed.connect ((win_id, app_id, stage) =>
      {
        proxy.focused_window_changed (win_id, app_id, stage);
      });
      return proxy;
    }

    public async WindowInfo[] get_window_stack () throws Error
    {
      var res = yield service.get_window_stack();
      return res;
    }

    public WindowInfo[] get_window_stack_sync () throws Error
    {
      WindowInfo[] win = {};
      MainLoop ml = new MainLoop ();
      Error? error = null;
      service.get_window_stack.begin ((obj, res) =>
      {
        try
        {
          win = service.get_window_stack.end (res);
        }
        catch (Error e)
        {
          error = e;
        }
        finally
        {
          ml.quit ();
        }
      });
      ml.run ();
      if (error != null)
        throw error;
      return win;
    }

    private WindowStackService service;
  }
}
