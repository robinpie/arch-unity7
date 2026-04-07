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

namespace PreviewPlayer
{
  [DBus (name = "com.canonical.Unity.Lens.Music.PreviewPlayer")]
  public class PreviewPlayerService : Object
  {
    private PreviewPlayer player;
    private bool released;
    
    public PreviewPlayerService (PreviewPlayer preview_player)
    {
      released = false;
      player = preview_player;
      player.progress.connect (on_progress_change);
    }
    
    private void on_progress_change (string uri, uint32 state, double value)
    {
      progress (uri, state, value);
    }
    
    public void play (string uri)
    {
      if (released)
      {
        released = false;
        var app = GLib.Application.get_default ();
        app.hold ();
      }
      player.play (uri);
    }
    
    public void pause ()
    {
      player.pause ();
    }

    public void resume ()
    {
      player.resume ();
    }

    public void pause_resume ()
    {
      player.pause_resume ();
    }
    
    public void stop ()
    {
      player.stop ();
    }

    public void close ()
    {
      if (released)
        return;
      released = true;
      player.stop ();
      var app = GLib.Application.get_default ();
      app.release ();

    }

    public HashTable<string, Variant> video_properties (string uri)
    {
      if (released)
      {
        released = false;
        var app = GLib.Application.get_default ();
        app.hold ();
      }
      return player.get_video_file_props (uri);
    }

    public signal void progress (string uri, uint32 state, double value);
  }
}