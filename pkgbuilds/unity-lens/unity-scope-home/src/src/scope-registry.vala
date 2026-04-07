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

namespace Unity.HomeScope
{
  /**
   * Wrapper for Protocol.ScopeRegistry with some high-level helpers.
   */
  public class ScopeRegistry
  {
    private static ScopeRegistry reg;
    
    private SList<Unity.Protocol.ScopeRegistry.ScopeRegistryNode> registry = new SList<Unity.Protocol.ScopeRegistry.ScopeRegistryNode> ();

    private HashTable<string, string> subscope_to_master = new HashTable<string, string> (str_hash, str_equal); //maps subscope id to master scope id
    private HashTable<string, unowned Protocol.ScopeRegistry.ScopeRegistryNode?> master_to_node = new HashTable<string, unowned Protocol.ScopeRegistry.ScopeRegistryNode?> (str_hash, str_equal); //maps master scope id to node data

    public bool scopes_ready { get; internal set; default = false; } //whether scopes tree has been processed on startup
    public GLib.SList<Protocol.ScopeRegistry.ScopeRegistryNode?>? scopes
    {
      get
      {
        return registry;
      }
    }

    public static ScopeRegistry instance ()
    {
      if (reg == null)
        reg = new ScopeRegistry ();
      return reg;
    }

    private ScopeRegistry ()
    {
    }

    public unowned Protocol.ScopeRegistry.ScopeMetadata? get_master_scope_info (string scope_id)
    {
      var node = master_to_node.lookup (scope_id);
      if (node != null)
        return node.scope_info;
      return null;
    }

    public unowned Protocol.ScopeRegistry.ScopeRegistryNode? get_master_scope_node (string scope_id)
    {
      return master_to_node.lookup (scope_id);
    }

    public bool is_master (string scope_id)
    {
      return master_to_node.contains (scope_id);
    }

    public string? get_master_scope_id (string subscope_id)
    {
      return subscope_to_master.lookup (subscope_id);
    }

    public async void find_scopes ()
    {
      debug ("Searching for scopes");

      Protocol.ScopeRegistry tmp_registry = null;
      try
      {
        tmp_registry = yield Protocol.ScopeRegistry.find_scopes (null);
        debug ("Scope discovery done");
        scopes_ready = true;
      }
      catch (Error e)
      {
        error ("Scope discovery failed: %s", e.message);
      }

      create_lookups (tmp_registry);
    }

    private void create_lookups (Protocol.ScopeRegistry registry)
    {
      foreach (var node in registry.scopes)
      {
        if (node.scope_info.id != "home.scope")
        {
          var sub_scopes = new GLib.SList<Protocol.ScopeRegistry.ScopeMetadata?> ();
          foreach (var sub_scope in node.sub_scopes)
          {
            // ignore subscope if query_binary is missing; this effecitvely removes it from
            // the registry as seen by home scope.
            if (!has_binary (sub_scope.query_binary))
            {
              debug ("Binary %s missing for %s, disabling", sub_scope.query_binary, sub_scope.id);
              continue;
            }
            sub_scopes.append (sub_scope);
            subscope_to_master.insert (sub_scope.id, node.scope_info.id);
          }

          node.sub_scopes = (owned)sub_scopes; //replace original list of subscopes for this master node

          // FIXME: we could disable master scope if all subscopes are disabled, however this poses issues
          // with remote scopes; we would need to wait for remote-scopes query to finish to know if
          // if this master is needed for remote results.

          // sort master scopes by scope_id
          this.registry.insert_sorted (node, (node_a, node_b) => { return strcmp (node_a.scope_info.id, node_b.scope_info.id); });
          master_to_node.insert (node.scope_info.id, node);
        }
        else
        {
          debug ("Skipping scope %s", node.scope_info.id);
        }
      }
    }

    public Gee.Set<string> flatten ()
    {
      if (!scopes_ready)
        error ("Scopes registry not ready");
      
      var scopes_set = new Gee.TreeSet<string> ();
      foreach (var node in registry)
      {
        scopes_set.add (node.scope_info.id);
        foreach (var sub_scope in node.sub_scopes)
        {
          scopes_set.add (sub_scope.id);
        }
      }
      return scopes_set;
    }

    private bool has_binary (string? binary)
    {
      if (binary != null && binary != "" &&
          GLib.Environment.find_program_in_path (binary) == null)
        return false;
      return true;
    }
  }
}
