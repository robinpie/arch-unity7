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

namespace Unity.HomeScope
{
  /**
   * Helper scope registry that contains both local and remote scopes.
   */
  public class MetaScopeRegistry
  {
    private static MetaScopeRegistry mreg = null;
    private Gee.HashMap<string, Gee.Set<string>> scopes = new Gee.HashMap<string, Gee.Set<string>> ();

    public static MetaScopeRegistry instance ()
    {
      if (mreg == null)
        mreg = new MetaScopeRegistry ();
      return mreg;
    }

    public void update (ScopeRegistry local_registry, RemoteScopeRegistry? remote_registry)
    {
      scopes.clear ();

      foreach (var node in local_registry.scopes)
      {
        var subscopes = new Gee.TreeSet<string> ();
        if (node.sub_scopes != null)
        {
          foreach (var scope in node.sub_scopes)
          {
            subscopes.add (scope.id);
          }
        }
        scopes[node.scope_info.id] = subscopes;
      }

      if (remote_registry != null)
      {
        foreach (var scope in remote_registry.get_scopes ())
        {
          var master_id = SearchUtil.get_master_id_from_scope_id (scope.scope_id);
          if (scopes.has_key (master_id))
          {
            scopes[master_id].add (scope.scope_id);
          }
          else
          {
            warning ("Master scope for %s doesn't exist", scope.scope_id);
          }
        }
      }
      debug ("Meta registry updated with %u master scopes", scopes.size);
    }

    public bool is_master (string scope_id)
    {
      return scopes.has_key (scope_id);
    }

    public Gee.Set<string>? get_subscopes (string master_scope_id)
    {
      if (!scopes.has_key (master_scope_id))
        return null;
      return scopes[master_scope_id];
    }

    public bool has_subscopes (string master_scope_id)
    {
      if (!scopes.has_key (master_scope_id))
        return false;
      return scopes[master_scope_id].size > 0;
    }
  }
}
