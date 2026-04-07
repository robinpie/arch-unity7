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

using Gst;
using GLib;

namespace PreviewPlayer
{
  public uint INACTIVITY_TIMEOUT_MSECS = 60*1000; //player will quit after that many milliseconds after Close() DBus call
  
  public static void main (string[] args)
  {
    GLib.Environment.set_prgname ("music-preview-player");
      
    PreviewPlayerService service = null;
    PreviewPlayer player = null;
    
    Gst.init (ref args);
    
    var app = new GLib.Application ("com.canonical.Unity.Lens.Music.PreviewPlayer", GLib.ApplicationFlags.IS_SERVICE);
    app.set_inactivity_timeout (INACTIVITY_TIMEOUT_MSECS);
    
    try
    {
      var conn = GLib.Bus.get_sync (BusType.SESSION, null);
      player = new PreviewPlayer ();
      service = new PreviewPlayerService (player);
      conn.register_object ("/com/canonical/Unity/Lens/Music/PreviewPlayer", service);
      
      app.hold();
      app.run(args);
    }
    catch (PreviewPlayerError e)
    {
      stderr.printf ("Couldn't create preview player: %s\n", e.message);
    }
    catch (GLib.IOError e)
    {
      stderr.printf ("Couldn't register dbus object: %s\n", e.message);
    }
  }
}