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

  public class KeywordSearch
  {
    private Regex regex;

    // map keywords to associated scope_id, based on Keywords definition in .scope file
    private HashTable<string, Gee.Set<string>?> keyword_to_scope_id = new HashTable<string, Gee.Set<string>?> (str_hash, str_equal);

    internal uint num_of_mappings
    {
      get
      {
        return keyword_to_scope_id.get_keys ().length ();
      }
    }

    public KeywordSearch ()
    {
      try
      {
        regex = new Regex ("^\\s*(\\w+):\\s*(.*)$");
      }
      catch (Error e) // this shouldn't really happen
      {
        critical ("Error in regular expression: %s", e.message);
      }
    }

    public void index_keywords (string scope_id, GLib.SList<string> keywords)
    {
      foreach (var kw in keywords)
      {
        kw = kw.down ();
        if (!keyword_to_scope_id.contains (kw))
        {
          keyword_to_scope_id.insert (kw, new Gee.TreeSet<string> ());
        }
        keyword_to_scope_id[kw].add (scope_id);
        debug ("Indexing %s -> %s", kw, scope_id);
      }
    }

    public void rebuild ()
    {
      debug ("Rebuilding keyword - scope id lookup");

      foreach (var node in ScopeRegistry.instance ().scopes)
      {
        index_keywords (node.scope_info.id, node.scope_info.keywords); //add master scope keywords
        foreach (var scope in node.sub_scopes) // add subscopes keywords
        {
          index_keywords (scope.id, scope.keywords);
        }
      }
    }

    public unowned Gee.Set<string>? process_query (string search_query, out string new_search_query)
    {
      MatchInfo info;
      new_search_query = null;
      if (regex.match (search_query, 0, out info))
      {
        if (info != null)
        {
          var keyword = info.fetch (1);
          if (keyword != null)
          {
            var query_part = info.fetch (2);
            unowned Gee.Set<string>? scopes = keyword_to_scope_id.lookup (keyword.down ());
            if (scopes != null)
            {
              new_search_query = query_part != null ? query_part : "";
              return scopes;
            }
          }
        }
      }
      return null;
    }
  }
}
