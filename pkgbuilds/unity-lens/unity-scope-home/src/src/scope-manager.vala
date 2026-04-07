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

namespace Unity.HomeScope {

public class ScopeManager: Object
{
  public signal void disabled_scopes_changed ();

  private PreferencesManager preferences = PreferencesManager.get_default ();
  private MasterScopesManager master_scopes_mgr = new MasterScopesManager ();
  private Gee.Set<string> client_scopes = new Gee.HashSet<string> ();
  private Gee.Set<string> disabled_scopes_lut = new Gee.HashSet<string> ();

  private Settings gp_settings;
  private const string HOMELENS_DEFAULT_VIEW = "home-lens-default-view";
  private const string DISABLED_SCOPES_KEY = "disabled-scopes";
  private const string REMOTE_CONTENT_KEY = "remote-content-search";

  public ScopeManager ()
  {
    gp_settings = new Settings ("com.canonical.Unity.Lenses");
    gp_settings.bind (HOMELENS_DEFAULT_VIEW, this, "home_lens_default_view", SettingsBindFlags.DEFAULT); // bind get/set

    update_disabled_scopes ();

    preferences.notify[DISABLED_SCOPES_KEY].connect ((obj, pspec) =>
    {
      update_disabled_scopes ();
      disabled_scopes_changed ();
    });

    update_remote_content_search ();
    preferences.notify[REMOTE_CONTENT_KEY].connect ((obj, pspec) =>
    {
      update_remote_content_search ();
    });
  }

  private void update_remote_content_search ()
  {
    remote_content_search = (preferences.remote_content_search == Unity.PreferencesManager.RemoteContent.ALL);
  }

  private void update_disabled_scopes ()
  {
    disabled_scopes_lut.clear ();
    foreach (var scope_id in preferences.disabled_scopes)
    {
      disabled_scopes_lut.add (scope_id);
    }
  }

  public string[] home_lens_default_view { get; set; default = new string[0]; }
  public string[] disabled_scopes
  {
    get
    {
      return preferences.disabled_scopes;
    }
  }

  public bool remote_content_search { get; internal set; }

  public void start_master_scopes ()
  {
    master_scopes_mgr.start ();
  }

  public bool is_disabled (string scope_id)
  {
    return disabled_scopes_lut.contains (scope_id);
  }

  /**
   * Returns true if scope is a client scope; shouldn't be called for master scopes.
   */
  public bool is_client_scope (string scope_id)
  {
    var reg = ScopeRegistry.instance ();
    if (client_scopes.size == 0)
    {
      foreach (var id in reg.flatten ())
      {
        if (!reg.is_master (id)) // ignore master scopes
          client_scopes.add (id);
      }
    }
    return client_scopes.contains (scope_id);
  }

  /**
   * Get ids of scopes with always-search flag on
   */
  public Gee.Set<string> get_always_search_scopes ()
  {
    var scopes = new Gee.TreeSet<string> ();
    foreach (string scope in preferences.always_search)
    {
      // only return scope if its in registry
      foreach (var topscope in ScopeRegistry.instance ().scopes)
      {
        if (topscope.scope_info.id == scope)
        {
          scopes.add (scope);
          break;
        }

        foreach (var subscope in topscope.sub_scopes)
        {
          if (subscope.id == scope)
          {
            scopes.add (scope);
            break;
          } 
        }
      }
    }
    return scopes;
  }

  
}

}
