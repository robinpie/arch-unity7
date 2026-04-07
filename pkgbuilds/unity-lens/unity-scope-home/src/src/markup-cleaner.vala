/*
 * Copyright (C) 2012-2013 Canonical Ltd
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
  namespace MarkupCleaner
  {
    /**
     * Regex for capturing HTML tags. It's closely related to the logic in replace_cb,
     * so be careful with any changes to it; don't forget to run unit tests!
     * First part of the regex captures <..> tags.
     * The last part captures '&' unless it's an entity.
     */

    internal static const string HTML_MARKUP_RE = "(?'OPENING_BRACKET'</?)\\s*(?'TAG_NAME'[^>]*?)\\s*(?'CLOSING_BRACKET'/?>)|(?'ENTITY'\\&(?'ENTITY_NAME'[a-zA-Z0-9]+);)|(?'AMPERSAND'\\&(?!(#\\d+)))";

    internal static bool replace_cb (MatchInfo match, StringBuilder result)
    {
      string? entire_substring = match.fetch (0).down ();

      /* html <..> tags */
      if (match.get_match_count () == 4 && entire_substring != null)
      {
        string? opening_bracket = match.fetch_named ("OPENING_BRACKET");
        string? html_tag_name = match.fetch_named ("TAG_NAME").down ();
        string? closing_bracket = match.fetch_named ("CLOSING_BRACKET");

        if (html_tag_name == "br")
        {
          result.append ("\n");
        }
        else if (html_tag_name == "b" ||
                 html_tag_name == "i" ||
                 html_tag_name == "u" ||
                 html_tag_name == "s" ||
                 html_tag_name == "tt" ||
                 html_tag_name == "big" ||
                 html_tag_name == "small" ||
                 html_tag_name == "sup" ||
                 html_tag_name == "sub")
        {
          result.append (entire_substring);
        }
        else if (html_tag_name == "strike")
        {
          result.append (opening_bracket + "s" + closing_bracket);
        }
      }
      else if (match.get_match_count () == 6)
      {
        string entity_name = match.fetch_named ("ENTITY_NAME").down ();
        if (entity_name == "amp" ||
            entity_name == "quot" ||
            entity_name == "apos" ||
            entity_name == "lt" ||
            entity_name == "gt")
        {
          result.append (match.fetch_named ("ENTITY").down ());
        }
        else if (entity_name == "nbsp")
        {
          result.append (" ");
        }
        else //append raw entity name with &amp; prefix
        {
          result.append ("&amp;");
          result.append (entity_name);
        }
      }
      else if (match.get_match_count () == 7 && match.fetch_named ("AMPERSAND") == "&")
      {
        result.append ("&amp;");
      }
      /* else - ignore all other html tags */
      return false;
    }
    
    public static string html_to_pango_markup (string html_text)
    {
      try
      {
        var html_markup_re = new GLib.Regex (HTML_MARKUP_RE, GLib.RegexCompileFlags.MULTILINE);
        return html_markup_re.replace_eval (html_text, html_text.length, 0, 0, replace_cb);
      }
      catch (GLib.RegexError e) // this shouldn't really happen.. if it happens, it's a programming error
      {
        warning ("Regex compilation failed: %s", e.message);
      }
      return html_text;
    }
  }
}