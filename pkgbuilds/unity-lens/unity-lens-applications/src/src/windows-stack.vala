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
  public class WinStack
  {
    public struct Window
    {
      uint window_id;
      string app_id;

      public Window (uint window_id, string app_id)
      {
        this.window_id = window_id;
        this.app_id = app_id;
      }
    }
    
    private Gee.LinkedList<Window?> winstack = new Gee.LinkedList<Window?> ();

    public signal void updated ();
    
    public WinStack ()
    {
    }

    private void add_window (uint win_id, string app_id)
    {
      var win = Window (win_id, app_id);
      if (!winstack.contains (win))
        winstack.add (win);
    }

    private void insert_window (uint win_id, string app_id)
    {
      var win = Window (win_id, app_id);
      winstack.insert (0, win);
    }

    private void remove_window (uint win_id, string app_id)
    {
      var it = winstack.iterator ();
      while (it.next ())
      {
        var win = it.get ();
        if (win.app_id == app_id && win.window_id == win_id)
        {
          it.remove ();
          return;
        }
      }
    }
    
    public void from_win_stack (WindowInfo[] windows)
    {
      debug ("Adding %u windows", windows.length);
      foreach (var win in windows)
      {
        add_window (win.window_id, win.app_id);
      }
    }

    public void on_window_created (uint win_id, string app_id)
    {
      debug ("Window created: %s", app_id);
      add_window (win_id, app_id);
      updated ();
    }

    public void on_window_destroyed (uint win_id, string app_id)
    {
      debug ("Window destroyed: %s", app_id);
      remove_window (win_id, app_id);
      updated ();
    }

    public void on_focused_window_changed (uint win_id, string app_id, uint stage)
    {
      debug ("Focused window: %s", app_id);
      remove_window (win_id, app_id);
      insert_window (win_id, app_id);
      updated ();
    }

    public Gee.Iterator<Window?> iterator ()
    {
      var it = winstack.iterator ();
      return it;
    }
  }
}