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

  public static int main (string[] args)
  {
    GLib.Environment.set_prgname ("unity-remote-video-scope");

    /* Sort up locale to get translations but also sorting and
     * punctuation right */
    GLib.Intl.textdomain (Config.PACKAGE);
    GLib.Intl.bindtextdomain (Config.PACKAGE, Config.LOCALEDIR);
    GLib.Intl.bind_textdomain_codeset (Config.PACKAGE, "UTF-8");
    GLib.Intl.setlocale(GLib.LocaleCategory.ALL, "");

    var scope = new RemoteVideoScope ();
    var exporter = new Unity.ScopeDBusConnector (scope);
    try
    {
      exporter.export ();
    }
    catch (GLib.Error e)
    {
      error ("Cannot export scope to DBus: %s", e.message);
    }
    Unity.ScopeDBusConnector.run ();

    return 0;
  }

} /* namespace */
