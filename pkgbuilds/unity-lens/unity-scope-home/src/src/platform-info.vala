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

namespace Unity.HomeScope.SmartScopes {

  public class PlatformInfo: Object
  {
    public string platform { get; set; }
    public string locale { get; set; }
    public string[] added_scopes { get; set; }
    public string[] removed_scopes { get; set; }
    public string build_id { get; set; }
    public string country_code { get; set; }
    public string network_code { get; set; }

    public bool is_ready { get; private set; default = true; }

    private PlatformInfo ()
    {
    }

    public PlatformInfo.with_data (string platform, string? locale, string[] added, string[] removed)
    {
      this.platform = platform;
      this.locale = locale;
      this.added_scopes = added;
      this.removed_scopes = removed;
    }

    public static PlatformInfo gather_platform_info (ClientScopesInfo? client_scopes_info)
    {
      var info = new PlatformInfo ();
      info.platform = get_release_string ();

      var languages = GLib.Intl.get_language_names ();
      info.locale = languages[0]; //list is sorted from most desirable to least desirable, pick the first one

      if (client_scopes_info != null)
      {
        info.added_scopes = client_scopes_info.get_added_scopes ().to_array ();
        info.removed_scopes = client_scopes_info.get_removed_scopes ().to_array ();
      }

      /* TODO: switch to using hybris directly once it's on the desktop */
      if (Environment.find_program_in_path ("getprop") != null)
      {
        try
        {
          string process_stdout;
          int exit_status;
          Process.spawn_command_line_sync ("getprop ro.build.id",
                                           out process_stdout, null,
                                           out exit_status);
          var stripped = process_stdout.strip ();
          if (exit_status == 0 && stripped != null && stripped.length > 0)
          {
            info.build_id = stripped;
          }
        }
        catch (Error err)
        {
          warning ("Error getting build ID: %s", err.message);
        }
      }

      // get the country_code and network_code from the sim card, note that
      // this is done asynchronously, so when this function returns, the props
      // won't be set yet
      info.read_sim_properties.begin ();

      return info;
    }

    private async void read_sim_properties ()
    {
      try
      {
        is_ready = false;
        if (Environment.get_variable ("HOME_SCOPE_IGNORE_OFONO") == null)
        {
            var connection = yield Bus.get (BusType.SYSTEM, null);
            var reply = yield connection.call ("org.ofono", "/ril_0",
                                               "org.ofono.SimManager",
                                               "GetProperties", null,
                                               new VariantType ("(a{sv})"),
                                               DBusCallFlags.NONE, -1, null);
            reply = reply.get_child_value (0);
            var mcc_v = reply.lookup_value ("MobileCountryCode", VariantType.STRING);
            var mnc_v = reply.lookup_value ("MobileNetworkCode", VariantType.STRING);
            if (mcc_v != null) country_code = mcc_v.get_string ();
            if (mnc_v != null) network_code = mnc_v.get_string ();
        }
      }
      catch (Error err)
      {
        warning ("Unable to read SIM properties: %s", err.message);
      }
      finally
      {
        is_ready = true;
      }
    }

    internal static string get_release_string ()
    {
      // obtain platform version from lsb_release.
      // the form-factor can be different per-search, so that is not saved here
      // ubuntu_version tool needs to be used instead when it's available.
      try
      {
        string data;
        if (GLib.Process.spawn_sync (null, {"lsb_release", "-s", "-r"}, null, SpawnFlags.SEARCH_PATH , null, out data, null, null))
        {
          if (data != null)
            return data.strip ();
        }
      }
      catch (Error e)
      {
        warning ("Failed to process lsb-release info: %s", e.message);
      }
      return "unknown";
    }
  }
}
