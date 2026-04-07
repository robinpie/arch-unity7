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

  public class MoreSuggestionsParser: SingleResultParser, Object
  {
    public Variant[] parse (string scope_id, Json.Object result_dict) throws ParseError
    {
      var category_index = CategoryManager.instance ().get_category_index (CategoryManager.MORE_SUGGESTIONS_SCOPE_ID);
      if (category_index < 0)
      {
        warning ("No category index for More Suggestions");
        return new Variant[0];
      }

      if (!result_dict.has_member ("metadata"))
        throw new ParseError.INVALID_JSON ("metadata element is missing");

      var metadata_node = result_dict.get_member ("metadata");
      var metadata = metadata_node.get_object ();

      string? result_uri = result_dict.get_string_member ("uri");
      if (result_uri == null || result_uri == "")
        throw new ParseError.INVALID_JSON ("uri element is missing");

      if (result_uri.has_prefix("scopes-query"))
        result_uri = "x-unity-no-preview-" + result_uri;

      string? price = null;
      if (metadata.has_member ("formatted_price"))
        price = metadata.get_string_member ("formatted_price");
      else if (metadata.has_member ("price"))
        price = metadata.get_string_member ("price");
        
      var image_obj = metadata.get_object_member ("images");
      string? image_uri = null;

      if (image_obj != null)
        image_uri = extract_image_uri (image_obj, 128*128);

      if ((image_uri == null || image_uri == "") && result_dict.has_member ("icon_hint"))
      {
        image_uri = result_dict.get_string_member ("icon_hint");
      }

      if (image_uri != null && image_uri != "")
      {
        var file = File.new_for_uri (image_uri);
        var icon = new AnnotatedIcon (new FileIcon (file));
        // FIXME: dash doesn't like empty string as ribbon
        icon.ribbon = price == null || price == "" ? " " : price;

        unowned string category = "";
        if (metadata.has_member ("category"))
          category = metadata.get_string_member ("category");

        switch (category)
        {
          case "application":
            icon.category = CategoryType.APPLICATION;
            break;
          case "movie":
            icon.category = CategoryType.MOVIE;
            break;
          case "music":
            icon.category = CategoryType.MUSIC;
            break;
          case "book":
            icon.category = CategoryType.BOOK;
            break;
          default:
            icon.category = CategoryType.HOME; // will use generic icon
            break;
        }
        image_uri = icon.to_string ();
      }

      Variant? metadata_var = null;
      try
      {
        if (image_obj == null)
          metadata_node.get_object ().remove_member ("images");
        // the binding is wrong, deserialize returns floating variant
        metadata_var = json_gvariant_deserialize (metadata_node, "a{sv}");
      }
      catch (Error e)
      {
        warning ("Error deserializing metadata: %s", e.message);
        metadata_var = new Variant.array (VariantType.VARDICT.element (), {} );
      }

      Variant row[9] = {
        result_uri,
        image_uri != null ? image_uri : "",
        new Variant.uint32 (0),
        new Variant.uint32 (Unity.ResultType.DEFAULT),
        new Variant.string ("text/html"),
        new Variant.string (result_dict.get_string_member ("title")),
        new Variant.string (result_dict.has_member ("comment") ? result_dict.get_string_member ("comment") : ""),
        new Variant.string (result_uri),
        metadata_var
      };

      return row;
    }
    
    private HashTable<string, string> get_image_uri_dict (Json.Object image_obj)
    {
      var dict = new HashTable<string, string> (str_hash, str_equal);

      foreach (unowned string dimensions in image_obj.get_members ())
      {
        int width, height;
        int res = dimensions.scanf ("%dx%d", out width, out height);
        if (res != 2) continue;
        dict[dimensions] =
          image_obj.get_array_member (dimensions).get_string_element (0);
      }

      return dict;
    }

    /**
     * extract_image_uri:
     *
     * Returns image uri with pixel size (width * height) that is more than
     * or equal to the given pixel size.
     * In case only images with smaller pixel size are available, returns
     * the largest of those.
     */
    private string extract_image_uri (Json.Object? image_obj,
                                      int pixel_size)
    {
      if (image_obj == null) return "";
      var dict = get_image_uri_dict (image_obj);
      var keys_list = get_sorted_keys_for_dim_dict (dict);
      if (keys_list == null) return "";

      // short-circuit evaluation
      if (pixel_size == int.MAX) return dict[keys_list.last ().data];

      foreach (unowned string dim_string in keys_list)
      {
        int width, height;
        dim_string.scanf ("%dx%d", out width, out height);
        if (width * height >= pixel_size)
        {
          return dict[dim_string];
        }
      }

      return dict[keys_list.last ().data];
    }

    private List<unowned string> get_sorted_keys_for_dim_dict (HashTable<string, string> dict)
    {
      var list = dict.get_keys ();
      list.sort ((a_str, b_str) =>
      {
        int width1, height1, width2, height2;

        a_str.scanf ("%dx%d", out width1, out height1);
        b_str.scanf ("%dx%d", out width2, out height2);

        return width1 * height1 - width2 * height2;
      });

      return list;
    }
  }
}

