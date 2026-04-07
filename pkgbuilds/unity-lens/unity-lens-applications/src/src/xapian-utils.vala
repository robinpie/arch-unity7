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
 */

namespace Unity.ApplicationsLens.XapianUtils
{
  private static const string[] option_types =
  {
    "accessories",
    "education",
    "game",
    "graphics",
    "internet",
    "fonts",
    "office",
    "media",
    "customization",
    "accessibility",
    "developer",
    "science-and-engineering",
    "scopes",
    "system"
  };

  /* For each section we have a set filtering query we use to restrict
   * Xapian queries to that type */
  internal static HashTable<string, string> type_queries; 

 /* Pre-populates the type queries so it's easier/faster to build our search */
  internal static void populate_type_queries ()
  {
    if (type_queries == null)
    {
      type_queries = new HashTable<string, string> (str_hash, str_equal);
      type_queries.insert ("all", "NOT category:XYZ");
    
      type_queries.insert ("accessories", "(category:Utility AND NOT category:Accessibility)");
      type_queries.insert ("education", "(category:Education AND NOT category:Science)");
      type_queries.insert ("game", "category:Game");
      type_queries.insert ("graphics", "category:Graphics");
      type_queries.insert ("internet", "category:Network");
      type_queries.insert ("fonts", "category:Fonts"); // FIXME: wtf?
      type_queries.insert ("office", "category:Office");
      type_queries.insert ("media", "category:AudioVideo");
      type_queries.insert ("customization", "category:Settings");
      type_queries.insert ("accessibility", "(category:Accessibility AND NOT category:Settings)");
      type_queries.insert ("developer", "category:Development"); // FIXME emacs.desktop should be added
      type_queries.insert ("science-and-engineering", "(category:Science OR category:Engineering)");
      type_queries.insert ("scopes", "(pkg_wildcard:unity_scope_* OR pkg_wildcard:unity_lens_*)");
      type_queries.insert ("system", "(category:System OR category:Security)");
    }
  }

  public static string extract_type_query (OptionsFilter? options)
  {
    if (options == null || !options.filtering) return "NOT category:XYZ";
    
    populate_type_queries ();

    string? result = null;
    foreach (unowned string type_id in option_types)
    {
      var option = options.get_option (type_id);
      if (option == null) continue;
      if (!option.active) continue;

      if (result == null) result = type_queries[type_id];
      else result += " OR " + type_queries[type_id];
    }

    return result == null ? "NOT category:XYZ" : "(%s)".printf (result);
  }

  public string prepare_zg_search_string (string search_string,
                                          OptionsFilter? options)
  {
    string s = search_string.strip ();
    
    if (!s.has_suffix ("*") && s != "")
      s = s + "*";

    if (s != "")
      s = @"app:($s)";
    else
      return extract_type_query (options);

    if (options == null || !options.filtering)
      return s;
    else
      return s + " AND " + extract_type_query (options);
  }

  public static string prepare_pkg_search_string (string? search_string,
                                                  OptionsFilter? options)
  {
    if (Utils.is_search_empty (search_string))
    {
      if (options == null || !options.filtering)
        return "(type:Application OR type:Scope)";
      else
        return "(type:Application OR type:Scope) AND " + extract_type_query (options);
    }
    else
    {
      var s = search_string;
      
      s = s.strip ();
      
      /* The Xapian query parser seems to handle hyphens in a special way,
       * namely that it forces the joined tokens into a phrase query
       * no matter if it appears as the last word in a query and we have
       * the PARTIAL flag set on the query parser. This makes 'd-f' not
       * match 'd-feet' etc. */
      s = s.delimit ("-", ' ');
      
      if (options == null || !options.filtering)
        return "(type:Application OR type:Scope) AND " + s;
      else
        return "(type:Application OR type:Scope) AND %s AND %s".printf (extract_type_query (options), s);
    }
  }
}
