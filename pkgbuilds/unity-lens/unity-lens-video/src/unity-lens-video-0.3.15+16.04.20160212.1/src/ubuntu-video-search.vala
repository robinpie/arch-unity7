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

namespace Unity.VideoLens.UbuntuVideoSearch
{
  private static const string SERVER = "http://videosearch.ubuntu.com/v0";

  public static string sources_uri ()
  {
    return SERVER + "/sources";
  }

  public static string recommendations_uri ()
  {
    return SERVER + "/search?q=";
  }

  public static string build_search_uri (string query, Gee.ArrayList<string>? sources, string? form_factor = null)
  {
    var uri = new StringBuilder ();
    uri.append (SERVER);
    uri.append ("/search?q=");
    uri.append (Uri.escape_string (query, "", false));
    uri.append ("&split=true");
    if (sources != null && sources.size>0)
    {
      uri.append ("&sources=");
      uri.append (sources[0]);
      for (int i=1; i<sources.size; i++)
      {
        uri.append (",");
        uri.append (sources[i]);
      }
    }
    if (form_factor != null)
    {
      uri.append ("&form_factor=");
      uri.append (form_factor);
    }
    return uri.str;
  }

  /**
   * Parses JSON data returned by search requests, returns list of videos.
   */
  public Gee.ArrayList<RemoteVideoFile?>? process_search_results (string json_data, bool is_treat_yourself) throws Error
  {
    var parser = new Json.Parser ();

    parser.load_from_data (json_data);
    var root = parser.get_root ();

    if (root.get_node_type () == Json.NodeType.OBJECT)
    {
      var records = root.get_object ().get_array_member ("other");
      var results = process_results_array (records, CAT_INDEX_ONLINE);
      records = root.get_object ().get_array_member ("treats");
      results.add_all (process_results_array (records, CAT_INDEX_MORE));
      return results;
    }
    else
    {
      var records = root.get_array ();
      var results = process_results_array (records, is_treat_yourself ? CAT_INDEX_MORE : CAT_INDEX_ONLINE);
      return results;
    }
  }

  /**
   * Joins elements of JSON array of strings.
   */
  internal string join_array (Json.Array array, string separator)
  {
    var res = new StringBuilder ();
    var len = array.get_length ();
    array.foreach_element ((a, index, node) =>
    {
      res.append (node.get_string ());
      if (index < len - 1)
        res.append (separator);
    });
    return res.str;
  }

  public string[] json_array_to_str_array (Json.Array array)
  {
    string [] res = {};
    array.foreach_element ((a, index, node) =>
    {
      res += node.get_string ();
    });
    return res;
  }

  public RemoteVideoDetails process_details_results (string json_data) throws Error
  {
    var parser = new Json.Parser ();
    parser.load_from_data (json_data);
    var details = parser.get_root ().get_object ();

    var video = RemoteVideoDetails ();
    video.title = details.get_string_member ("title");
    video.image = details.get_string_member ("image");
    video.description = details.get_string_member ("description");
    video.source = details.get_string_member ("source");

    if (details.has_member ("release_date"))
    {
      // v1 spec states release_date will be YYYY-MM-DD
      var release_date = GLib.Time ();
      if (release_date.strptime (details.get_string_member ("release_date"), "%Y-%m-%d") != null)
      {
        video.release_date = release_date.format ("%Y");
      }
      else
      {
        warning ("Failed to parse release_date: '%s'", details.get_string_member ("release_date"));
      }
    }

    video.price = details.has_member ("formatted_price") ? details.get_string_member ("formatted_price") : "";

    video.duration = 0;
    if (details.has_member ("duration"))
      video.duration = int.parse (details.get_string_member ("duration")) / 60;

    video.directors = {};
    if (details.has_member ("directors"))
    {
      var directors = details.get_array_member ("directors");
      video.directors = json_array_to_str_array (directors);
    }

    if (details.has_member ("starring"))
    {
      var starring = details.get_array_member ("starring");
      video.starring = join_array (starring, ", ");
    }

    video.genres = {};
    if (details.has_member ("genres"))
    {
      var genres = details.get_array_member ("genres");
      video.genres = json_array_to_str_array (genres);
    }

    if (details.has_member ("uploaded_by"))
      video.uploaded_by = details.get_string_member ("uploaded_by");

    if (details.has_member ("date_uploaded"))
      video.date_uploaded = details.get_string_member ("date_uploaded");

    return video;
  }

  /**
   * Parses JSON data returned by 'sources' query, returns list of sources (e.g. "Amazon", "Youtube" etc.)
   */
  public Gee.ArrayList<string>? process_sources_results (string json_data) throws Error
  {
    var parser = new Json.Parser ();

    parser.load_from_data (json_data);
    var sources_array = parser.get_root ().get_array ();
    var results = new Gee.ArrayList<string> (null);

    sources_array.foreach_element ((array, index, node) =>
    {
      results.add (node.get_string ());
    });

    return results;
  }

  /**
   * Converts JSON results array to list of RemoteVideoFile records.
   */
  internal Gee.ArrayList<RemoteVideoFile?> process_results_array (Json.Array results, int category)
  {
    var videos = new Gee.ArrayList<RemoteVideoFile?> ();

    results.foreach_element ((array, index, node) =>
    {
      try
      {
        var video = RemoteVideoFile ();
        var rec = node.get_object ();
        video.uri = rec.get_string_member ("url");
        video.title = rec.get_string_member ("title");
        video.icon = rec.get_string_member ("img");
        video.comment = rec.get_string_member ("source");
        video.details_uri = (rec.has_member ("details") ? rec.get_string_member ("details") : "");
        video.category = category;
        video.price = null;

        if (category == CAT_INDEX_MORE)
        {
          string price = null;
          if (rec.has_member ("formatted_price"))
          {
            price = rec.get_string_member ("formatted_price");
          }
          if (price == null || price == "free")
          {
            price = _("Free");
          }
          video.price = price;
        }
        videos.add (video);
      }
      catch (Error e)
      {
        warning ("Malformed result, skipping: %s", e.message);
      }
    });
    return videos;
  }
}
