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

using GLib;
using Config;

namespace Unity.FilesLens {

  static const string DBUS_NAME = "com.canonical.Unity.Scope.LocalFiles";
  static Application? app = null;
  static Daemon? daemon = null;

  /* Check if a given well known DBus is owned.
   * WARNING: This does sync IO!  */
  public static bool dbus_name_has_owner (string name)
  {
    try {
      bool has_owner;
      DBusConnection bus = Bus.get_sync (BusType.SESSION);
      Variant result = bus.call_sync ("org.freedesktop.DBus",
                                      "/org/freedesktop/dbus",
                                      "org.freedesktop.DBus",
                                      "NameHasOwner",
                                      new Variant ("(s)", name),
                                      new VariantType ("(b)"),
                                      DBusCallFlags.NO_AUTO_START,
                                      -1);
      result.get ("(b)", out has_owner);
      return has_owner;
    } catch (IOError e) {
      warning ("Unable to decide whether '%s' is running: %s", name, e.message);
    }
    
    return false;
  }

  public static int main (string[] args)
  {
    /* Sort up locale to get translations but also sorting and
     * punctuation right */
    GLib.Intl.textdomain (Config.PACKAGE);
    GLib.Intl.bindtextdomain (Config.PACKAGE, Config.LOCALEDIR);
    GLib.Intl.bind_textdomain_codeset (Config.PACKAGE, "UTF-8");
    GLib.Intl.setlocale(GLib.LocaleCategory.ALL, "");
  
    /* Make sure the desktop appinfos are picked up correctly */
    DesktopAppInfo.set_desktop_env ("GNOME");
    
    /* Sub processes inheriting our environment may be confused if they
     * see a starter bus set */
    Environment.unset_variable ("DBUS_STARTER_ADDRESS");
    Environment.unset_variable ("DBUS_STARTER_BUS_TYPE");
  
    /* Workaround for https://bugzilla.gnome.org/show_bug.cgi?id=640714 
     * GApplication.register() call owns our DBus name in a sync manner
     * making it race against GDBus' worker thread to export our
     * objects on the bus before/after owning our name and receiving
     * method calls on our objects (which may not yet be up!)*/
    if (dbus_name_has_owner (DBUS_NAME))
      {
        print ("Another instance of the Unity Files Daemon " +
               "already appears to be running.\nBailing out.\n");
        return 2;
      }
    
    /* Now register our DBus objects *before* acquiring the name!
     * See above for reasons */
    daemon = new Daemon ();
  
    /* Use GApplication directly for single instance app functionality */
    app = new Application (DBUS_NAME, ApplicationFlags.IS_SERVICE);
    try {
      app.register ();
    } catch (Error e) {
      /* FIXME: We get this error if another daemon is already running,
       * but it uses a generic error so we can't detect this reliably... */
      print ("Failed to start files daemon: %s\n", e.message);
      return 1;
    }
    
    if (app.get_is_remote ())
      {
        print ("Another instance of the Unity Files Daemon " +
               "already appears to be running.\nBailing out.\n");
        return 2;
      }
    
    /* Hold()ing the app makes sure the GApplication doesn't exit */    
    app.hold();
    return app.run ();
  }

} /* namespace */
