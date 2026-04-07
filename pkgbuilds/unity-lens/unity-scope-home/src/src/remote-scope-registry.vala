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
 * Authored by Michal Hruby <michal.hruby@canonical.com>
 *
 */

namespace Unity.HomeScope
{
  /**
   * Wrapper for information about remote scopes which can share the data
   * in a Dee.SharedModel.
   */
  public class RemoteScopeRegistry : Object
  {
    private const string SWARM_NAME = "com.canonical.Unity.SmartScopes.RemoteScopesModel";

    private Dee.Model scopes_model;
    private SmartScopes.RemoteScopeInfo[] remote_scopes;
    private Gee.TreeSet<string> remote_scopes_lut = new Gee.TreeSet<string> ();

    public RemoteScopeRegistry.for_scopes (SmartScopes.RemoteScopeInfo[] info)
    {
      Object ();

      remote_scopes = info;
      foreach (var scope in remote_scopes)
      {
        remote_scopes_lut.add (scope.scope_id);

        // also add a fake master scope id to the lookup
        var master_id = SearchUtil.get_master_id_from_scope_id (scope.scope_id);
        if (master_id != null)
            remote_scopes_lut.add (master_id);
      }
    }

    public Dee.Model create_model ()
    {
      if (scopes_model != null) return scopes_model;

      var peer = Object.new (typeof (Dee.Peer),
                             "swarm-name", SWARM_NAME,
                             "swarm-owner", true) as Dee.Peer;
      var access_mode = Dee.SharedModelAccessMode.LEADER_WRITABLE;
      var model = Object.new (typeof (Dee.SharedModel),
                              "peer", peer,
                              "back-end", new Dee.SequenceModel (),
                              "access-mode", access_mode) as Dee.Model;
      model.set_schema ("s", "s", "s", "s", "s", "as");
      model.set_column_names ("scope_id", "name", "description",
                              "icon", "screenshot_url", "keywords");

      foreach (unowned SmartScopes.RemoteScopeInfo info in remote_scopes)
      {
        var keywords_var = new Variant.strv (info.keywords);
        model.append (info.scope_id, info.name, info.description,
                      info.icon_hint, info.screenshot_url, keywords_var);
      }

      scopes_model = model;
      return scopes_model;
    }

    public bool has_scope (string scope_id)
    {
      return remote_scopes_lut.contains (scope_id);
    }

    /**
     * Get list of all remote scopes; note: this also includes master scopes.
     */
    public unowned SmartScopes.RemoteScopeInfo[] get_scopes ()
    {
      return remote_scopes;
    }
  }
}
