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
   * Parses /usr/share/unity/client-scopes.json that provides list of scopes installed by default.
   * Computes diff between that list and scopes from the registry to provide lists of added / removed scopes.
   */
  public class ClientScopesInfo
  {
    public static const string CLIENT_SCOPES_FILE = Config.PKGDATADIR + "/client-scopes.json";

    private Gee.ArrayList<string> added_scopes = new Gee.ArrayList<string> ();
    private Gee.ArrayList<string> removed_scopes = new Gee.ArrayList<string> ();

    public ClientScopesInfo (Gee.Set<string> default_scopes, Gee.Set<string> installed_scopes)
    {
      compute_diff (default_scopes, installed_scopes);
    }
    
    public static ClientScopesInfo from_file (string path, Gee.Set<string> installed_scopes) throws Error
    {
      var parser = new Json.Parser ();
      parser.load_from_file (path);
      var default_scopes = ClientScopesInfo.load (parser.get_root ());
      var cl = new ClientScopesInfo (default_scopes, installed_scopes);
      return cl;
    }

    public static ClientScopesInfo from_data (string json_data, Gee.Set<string> installed_scopes) throws Error
    {
      var parser = new Json.Parser ();
      parser.load_from_data (json_data);
      var default_scopes = ClientScopesInfo.load (parser.get_root ());
      var cl = new ClientScopesInfo (default_scopes, installed_scopes);
      return cl;
    }

    public Gee.ArrayList<string> get_added_scopes ()
    {
      return added_scopes;
    }
    
    public Gee.ArrayList<string> get_removed_scopes ()
    {
      return removed_scopes;
    }

    private void compute_diff (Gee.Set<string> default_scopes, Gee.Set<string> installed_scopes)
    {
      foreach (var scope_id in default_scopes)
      {
        // home.scope is explicitly ignored in the registry, so add it here so that it's not
        // considered removed and sent in removed_scopes list.
        if (!installed_scopes.contains (scope_id) && scope_id != "home.scope")
          removed_scopes.add (scope_id);
      }
      
      foreach (var scope_id in installed_scopes)
      {
        if (!default_scopes.contains (scope_id))
          added_scopes.add (scope_id);
      }
    }

    private static Gee.Set<string> load (Json.Node node) throws Error
    {
      var default_scopes = new Gee.TreeSet<string> ();

      var dict = node.get_object ();
      dict.foreach_member ((_obj, _name, _node) =>
      {
        // _name is package name, ignore it as we only care about scope ids
        Json.Array pkg_scopes = _node.get_array ();
        pkg_scopes.foreach_element ((_array, _index, _pnode) =>
        {
          default_scopes.add (_pnode.get_string ());
        });
      });
      return default_scopes;
    }
  }
}
