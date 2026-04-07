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

namespace Unity.HomeScope {

  public enum SearchQueryChange
  {
    NOT_CHANGED,
    NEW_QUERY,
    CANNED_QUERY,
    APPENDS_TO_PREVIOUS_QUERY,
    REMOVES_FROM_PREVIOUS_QUERY
  }
 
  public class SearchQueryState
  {
    private HashTable<string, string> previous_query = new HashTable<string, string> (str_hash, str_equal);
    private HashTable<string, string> canned_query = new HashTable<string, string> (str_hash, str_equal);
    
    public void remove (string home_channel_id)
    {
      previous_query.remove (home_channel_id);
      canned_query.remove (home_channel_id);
    }

    public bool has_channel (string home_channel_id)
    {
      return previous_query.contains (home_channel_id);
    }

    public void set_canned_query (string channel_id, string search_string)
    {
      canned_query[channel_id] = search_string;
    }

    /**
     * Compare search string with previous string for given home channel. Store new search string.
     */
    public SearchQueryChange search_query_changed (string channel_id, string search_string)
    {
      var query = search_string.strip ();
      var changed = SearchQueryChange.NEW_QUERY;

      if (canned_query.contains(channel_id) && canned_query[channel_id] == query)
      {
        changed = SearchQueryChange.CANNED_QUERY;
      }
      else
      {
        if (previous_query.contains (channel_id))
        {
          var prev = previous_query[channel_id];
          if (query == "" && prev != "") // there was a query previously, but user removed all characters in new one
          {
            changed = SearchQueryChange.NEW_QUERY;
          }
          else
          {
            if (prev == query)
              changed = SearchQueryChange.NOT_CHANGED;
            else if (prev == "")
              changed = SearchQueryChange.NEW_QUERY;
            else if (query.has_prefix (prev))
              changed = SearchQueryChange.APPENDS_TO_PREVIOUS_QUERY;
            else if (prev.has_prefix (query))
              changed = SearchQueryChange.REMOVES_FROM_PREVIOUS_QUERY;
          }
        }
      }
      
      canned_query.remove (channel_id);
      previous_query[channel_id] = query;

      debug ("search_query_changed for channel %s: %s", channel_id, changed.to_string ());
      return changed;
    }
  }
}

