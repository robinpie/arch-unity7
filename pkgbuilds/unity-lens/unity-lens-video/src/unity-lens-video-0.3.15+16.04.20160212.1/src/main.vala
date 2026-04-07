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
 */

using GLib;
using Config;

namespace Unity.VideoLens {

  static Application? app = null;
  static Daemon? daemon = null;

  public static int main (string[] args)
  {
    GLib.Environment.set_prgname ("unity-video-lens");

    /* Sort up locale to get translations but also sorting and
     * punctuation right */
    GLib.Intl.textdomain (Config.PACKAGE);
    GLib.Intl.bindtextdomain (Config.PACKAGE, Config.LOCALEDIR);
    GLib.Intl.bind_textdomain_codeset (Config.PACKAGE, "UTF-8");
    GLib.Intl.setlocale(GLib.LocaleCategory.ALL, "");

    try
    {
      app = Extras.dbus_own_name ("net.launchpad.scope.LocalVideos", () =>
      {
        daemon = new Daemon ();
      });
    }
    catch (Error e)
    {
      warning ("Failed to start video lens daemon: %s\n", e.message);
      return 1;
    }

    if (app == null)
    {
      warning ("Another instance of the Unity Videos Lens " +
             "already appears to be running.\nBailing out.\n");
      return 2;
    }

    return app.run ();
  }

} /* namespace */
