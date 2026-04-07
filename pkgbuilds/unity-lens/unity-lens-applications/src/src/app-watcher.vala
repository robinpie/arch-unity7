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
 * Authored by Michal Hruby <michal.hruby@canonical.com>
 *
 */

using Unity;
using Gee;
 
namespace Unity.ApplicationsLens
{ 

  public class AppWatcher : Object
  {
    private string[] prefixes;
    Gee.Set<string> running_apps;

    public AppWatcher ()
    {
      Object ();
    }

    public bool has_app_id (string desktop_id)
    {
      return desktop_id in running_apps;
    }

    public signal void running_applications_changed ();

    construct
    {
      running_apps = new HashSet<string> ();
      foreach (unowned string data_dir in Environment.get_system_data_dirs ())
      {
        prefixes += Path.build_path (Path.DIR_SEPARATOR_S,
                                     data_dir,
                                     "applications",
                                     Path.DIR_SEPARATOR_S, null);
      }

      try
      {
        var connection = Bus.get_sync (BusType.SESSION);
        connection.signal_subscribe ("org.ayatana.bamf",
                                     "org.ayatana.bamf.matcher",
                                     "RunningApplicationsChanged",
                                     "/org/ayatana/bamf/matcher",
                                     null,
                                     DBusSignalFlags.NONE,
                                     this.running_apps_changed);
        connection.call.begin ("org.ayatana.bamf",
                               "/org/ayatana/bamf/matcher",
                               "org.ayatana.bamf.matcher",
                               "RunningApplicationsDesktopFiles",
                               null,
                               null,
                               0,
                               -1,
                               null,
                               this.got_running_apps);
      }
      catch (Error err)
      {
        warning ("Unable to get running applications: %s", err.message);
      }
    }

    private void got_running_apps (Object? src_obj,
                                   AsyncResult res)
    {
      var connection = src_obj as DBusConnection;
      try
      {
        Variant reply = connection.call.end (res);
        string[] paths = (string[]) reply.get_child_value (0);

        foreach (var p in paths) running_apps.add (extract_desktop_id (p));
        this.running_applications_changed ();
      }
      catch (Error e)
      {
        warning ("%s", e.message);
      }
    }

    private void running_apps_changed (DBusConnection connection,
                                       string? sender_name,
                                       string object_path,
                                       string interface_name,
                                       string signal_name,
                                       Variant parameters)
    {
      if (!parameters.is_of_type (new VariantType ("(asas)")))
      {
        warning ("RunningApplicationsChanged signal has incorrect type!");
        return;
      }

      Variant opened_apps_variant;
      Variant closed_apps_variant;
      parameters.get ("(@as@as)", out opened_apps_variant,
                                  out closed_apps_variant);
      string[] opened_paths = (string[]) opened_apps_variant;
      string[] closed_paths = (string[]) closed_apps_variant;

      foreach (unowned string closed in closed_paths)
        running_apps.remove (extract_desktop_id (closed));
      foreach (unowned string opened in opened_paths)
        running_apps.add (extract_desktop_id (opened));

      this.running_applications_changed ();
    }

    private string extract_desktop_id (string path)
    {
      if (!path.has_prefix ("/")) return path;

      /* fdo menu-spec compliant desktop id extraction */
      foreach (unowned string prefix in prefixes)
      {
        if (path.has_prefix (prefix))
        {
          string without_prefix = path.substring (prefix.length);
          if (Path.DIR_SEPARATOR_S in without_prefix)
            return without_prefix.replace (Path.DIR_SEPARATOR_S, "-");

          return without_prefix;
        }
      }

      return Path.get_basename (path);
    }
  }

}
