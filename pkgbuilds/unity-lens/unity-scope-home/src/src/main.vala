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
 *
 */

using GLib;
using Config;

namespace Unity.HomeScope {

  static Application? app = null;
  static HomeScope? scope = null;
  static bool verbose_debug = false;

  void log_handler (string? domain, GLib.LogLevelFlags levels, string message)
  {
    var time_val = TimeVal ();
    string cur_time = time_val.to_iso8601 ().substring (11);
    stdout.printf ("%s %s: %s\n", cur_time, domain, message);
  }

  public static int main (string[] args)
  {
    GLib.Environment.set_prgname ("unity-scope-home");

    // if HOME_SCOPE_VERBOSE env var is set, enable custom log handler which prints out timestamps
    if (Environment.get_variable ("HOME_SCOPE_VERBOSE") != null)
    {
      verbose_debug = true;
      GLib.Log.set_handler ("unity-scope-home", GLib.LogLevelFlags.LEVEL_DEBUG, log_handler);
    }

    /* Sort up locale to get translations but also sorting and
     * punctuation right */
    GLib.Intl.textdomain (Config.PACKAGE);
    GLib.Intl.bindtextdomain (Config.PACKAGE, Config.LOCALEDIR);
    GLib.Intl.bind_textdomain_codeset (Config.PACKAGE, "UTF-8");
    GLib.Intl.setlocale(GLib.LocaleCategory.ALL, "");

    try
    {
      // read scope files before initializing dbus
      HomeScope.discover_scopes_sync ();

      app = Extras.dbus_own_name ("com.canonical.Unity.Scope.Home", () =>
      {
        scope = new HomeScope ();
      });
    }
    catch (Error e)
    {
      warning ("Failed to start home lens daemon: %s\n", e.message);
      return 1;
    }

    if (app == null)
    {
      warning ("Another instance of the Unity Home Lens already appears to be running.\nBailing out.\n");
      return 2;
    }

    return app.run ();
  }

} /* namespace */
