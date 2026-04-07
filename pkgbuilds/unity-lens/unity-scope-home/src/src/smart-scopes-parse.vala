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

  public errordomain ParseError
  {
    INVALID_JSON
  }

  public interface SingleResultParser: Object
  {
    public abstract Variant[] parse (string scope_id, Json.Object result_dict) throws ParseError;
  }

  public class SearchResponseHandler
  {
    private Json.Parser parser = new Json.Parser ();
    private HashTable<string, SingleResultParser> handlers = new HashTable<string, SingleResultParser> (str_hash, str_equal); 

    public SearchResponseHandler ()
    {
      var more_suggestions = new SmartScopes.MoreSuggestionsParser ();
      handlers["more_suggestions-amazon.scope"] = more_suggestions;
      handlers["more_suggestions-skimlinks.scope"] = more_suggestions;
      handlers["more_suggestions-u1ms.scope"] = more_suggestions;
      handlers["default"] = new SmartScopes.DefaultParser ();
    }

    public void parse_results_line (string line, SmartScopeResult result_cb, SmartScopeRecommendations recommend_cb) throws Error
    {
      if (parser.load_from_data (line))
      {
        try
        {
          parse_results_line_json (parser.get_root ().get_object (), result_cb, recommend_cb);
        }
        catch (Error e)
        {
          throw new ParseError.INVALID_JSON ("Error parsing JSON");
        }
      }
      else
      {
        warning ("Failed to parse JSON: '%s'", line);
      }
    }

    public void parse_results_line_json (Json.Object obj, SmartScopeResult result_cb, SmartScopeRecommendations recommend_cb) throws Error
    {
      if (obj.has_member ("type"))
      {
        var type = obj.get_string_member ("type");
        if (type == "results")
        {
          handle_results (obj, result_cb);
        }
        else if (type == "recommendations")
        {
          handle_recommendations (obj, recommend_cb);
        }
        else
        {
          warning ("Unknown type: %s", type);
          throw new ParseError.INVALID_JSON ("Unknown type: %s", type);
        }
      }
      else
      {
        warning ("Missing 'type' element");
        throw new ParseError.INVALID_JSON ("Missing 'type' element");
      }
    }

    private void handle_results (Json.Object obj, SmartScopeResult result_cb) throws Error
    {
      if (obj.has_member ("info"))
      {
        Json.Object info = obj.get_object_member ("info");
        // iterate over {"a_scope_id" : [{...},{...} ,...}]} results
        info.foreach_member ((_obj, _member, _node) =>
        {
          var handler = handlers.lookup (_member);
          if (handler == null)
            handler = handlers["default"];
          Json.Array rows = _node.get_array (); // array of result rows (each row is a dict)
          rows.foreach_element ((_array, _index, _arrnode) =>
          {
            Json.Object _res_dict = _arrnode.get_object ();
            try
            {
              var row = handler.parse (_member, _res_dict);
              if (row.length > 0)
                result_cb (_member, row);
            }
            catch (ParseError e) // error in single result shouldn't break processing of other results from that scope
            {
              warning ("Parsing error: %s", e.message);
            }
          });
        });
      }
      else
      {
        warning ("Missing 'info' element");
        throw new ParseError.INVALID_JSON ("Missing 'info' element");
      }
    }

    private void handle_recommendations (Json.Object obj, SmartScopeRecommendations recommend_cb) throws Error
    {
      if (obj.has_member ("scopes"))
      {
        if (!obj.has_member ("server_sid")) // server_sid is used by server to match feedback events with search queries; we send it back with feedback
        {
          warning ("Missing 'server_sid' element");
          throw new ParseError.INVALID_JSON ("Missing 'server_sid' element");
        }
        var sid = obj.get_string_member ("server_sid");
        var recommendations = new GLib.List<RecommendedScope?> ();
        Json.Array recommend = obj.get_array_member ("scopes");
        recommend.foreach_element ((_array, _index, _node) =>
        {
          Json.Array scope_elm_array = _node.get_array (); // each recommendation is an array: ["scope id", "type"], type is "server" or "client"
          if (scope_elm_array.get_length () == 2)
          {
            var id = scope_elm_array.get_element (0).get_string ();
            ScopeType tp = ScopeType.ClientScope;
            bool invalid_type = false;

            var type_str = scope_elm_array.get_element (1).get_string ();

            if (type_str == "client")
              tp = ScopeType.ClientScope;
            else if (type_str == "server")
              tp = ScopeType.ServerScope;
            else
              invalid_type = true;

            if (!invalid_type)
            {
              var scope_rec = RecommendedScope ()
              {
                scope_id = id,
                scope_type = tp
              };
              recommendations.append (scope_rec);
            }
            else
            {
              warning ("Invalid recommended scope type: %s", type_str);
            }
          }
          else
          {
            warning ("Invalid recommendations array");
          }
        });
        recommend_cb (sid, recommendations);
      }
      else
      {
        warning ("Missing 'scopes' element");
        throw new ParseError.INVALID_JSON ("Missing 'scopes' element");
      }
    }
  }

}
