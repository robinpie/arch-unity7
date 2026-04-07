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

/**
 * Parser for results of /remote-scopes server request
 */
public class RemoteScopesParser
{
  public RemoteScopeInfo[] parse (Json.Parser parser) throws Error
  {
    var scopes = parser.get_root ().get_array ();
    RemoteScopeInfo[] res = new RemoteScopeInfo [0];

    scopes.foreach_element ((_array, _index, _node) =>
    {
      var obj = _node.get_object ();
      unowned string id = obj.get_string_member ("id");
      if (id == null) return;

      unowned string name = null;
      unowned string description = null;
      unowned string icon_hint = null;
      unowned string screenshot = null;
      if (obj.has_member ("name"))
        name = obj.get_string_member ("name");
      if (obj.has_member ("description"))
        description = obj.get_string_member ("description");
      if (obj.has_member ("icon"))
        icon_hint = obj.get_string_member ("icon");
      if (obj.has_member ("screenshot"))
        screenshot = obj.get_string_member ("screenshot");

      string[] keywords = {};
      if (obj.has_member ("keywords"))
      {
        var keywords_arr = obj.get_array_member ("keywords");
        foreach (unowned Json.Node element in keywords_arr.get_elements ())
        {
          unowned string keyword = element.get_string ();
          if (keyword != null) keywords += keyword;
        }
      }

      RemoteScopeInfo info = RemoteScopeInfo ()
      {
        scope_id = id,
        name = name,
        description = description,
        icon_hint = icon_hint,
        screenshot_url = screenshot,
        keywords = (owned) keywords
      };
      res += (owned) info;
    });
    
    return res;
  }
}

}
