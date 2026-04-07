/*
 * Copyright (C) 2010 Canonical Ltd
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
 * Authored by Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 *
 */
using Dee;
using Config;
using Gee;

namespace Unity.MusicLens {
  
  public class Daemon : GLib.Object
  {
    private RhythmboxScope rb;
     
    construct
    {
      var app_check = new DesktopAppInfo ("org.gnome.Rhythmbox3.desktop");
      if (app_check != null)
      {
        rb = new RhythmboxScope ();
        try {
          rb.scope.export ();
        } catch (GLib.IOError e) {
          stdout.printf ("error %s\n", e.message);
        }
      }
    }
  }
} /* namespace */
