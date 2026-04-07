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

// workaround a problem in json-glib bindings
public extern unowned Variant json_gvariant_deserialize (Json.Node node, string signature) throws Error;

namespace Unity.HomeScope.SmartScopes {

  public class DefaultParser: SingleResultParser, Object
  {
    public Variant[] parse (string scope_id, Json.Object result_dict) throws ParseError
    {
      if (!result_dict.has_member ("metadata"))
        throw new ParseError.INVALID_JSON ("metadata element is missing");

      var metadata_node = result_dict.get_member ("metadata");
      var metadata = metadata_node.get_object ();

      string? result_uri = result_dict.get_string_member ("uri");
      if (result_uri == null || result_uri == "")
        throw new ParseError.INVALID_JSON ("uri element is missing");

      if (result_uri.has_prefix("scopes-query"))
        result_uri = "x-unity-no-preview-" + result_uri;

      string? icon_hint = null;
      if (result_dict.has_member ("icon_hint"))
        icon_hint = result_dict.get_string_member ("icon_hint");

      Variant? metadata_var = null;
      try
      {
        if (metadata.has_member("images"))
        {
            var image_obj = metadata.get_object_member ("images");
            // protect against "images":null
            if (image_obj == null)
                metadata_node.get_object ().remove_member ("images");
        }
        // the binding is wrong, deserialize returns floating variant
        metadata_var = json_gvariant_deserialize (metadata_node, "a{sv}");
      }
      catch (Error e)
      {
        warning ("Error deserializing metadata: %s", e.message);
        metadata_var = new Variant.array (VariantType.VARDICT.element (), {} );
      }

      var uri_variant = new Variant.string (result_uri);

      Variant row[9] = {
        uri_variant,
        new Variant.string (icon_hint != null ? icon_hint : ""),
        new Variant.uint32 (0), //global category
        new Variant.uint32 (Unity.ResultType.DEFAULT),
        new Variant.string ("text/html"),
        new Variant.string (result_dict.get_string_member ("title")),
        new Variant.string (result_dict.has_member ("comment") ? result_dict.get_string_member ("comment") : ""),
        uri_variant,
        metadata_var
      };

      return row;
    }
  }
}

