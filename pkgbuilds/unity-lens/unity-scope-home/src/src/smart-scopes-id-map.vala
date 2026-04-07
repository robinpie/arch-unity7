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
   * Helper class that stores home_channel_id -> session_id and home_channel_id -> server_sid mappings.
   */
  public class ChannelIdMap
  {
    private struct ServerIds
    {
      string session_id;
      string? server_sid;
    }

    private HashTable<string, ServerIds?> ids = new HashTable<string, ServerIds?> (str_hash, str_equal);

    public void remove_channel (string channel_id)
    {
      ids.remove (channel_id);
    }

    public bool has_session_id_for_channel (string channel_id)
    {
      return ids.contains (channel_id);
    }

    public bool has_server_sid_for_channel (string channel_id)
    {
      var id = ids.lookup (channel_id);
      if (id != null)
        return id.server_sid != null;
      return false;
    }

    public string? session_id_for_channel (string channel_id)
    {
      var id = ids.lookup (channel_id);
      if (id != null)
        return id.session_id;
      return null;
    }

    public string? server_sid_for_channel (string channel_id)
    {
      var id = ids.lookup (channel_id);
      if (id != null)
        return id.server_sid;
      return null;
    }

    public void map_session_id (string channel_id, string session_id)
    {
      var id = ServerIds ()
      {
        session_id = session_id,
        server_sid = null
      };
      ids[channel_id] = id;
    }

    public bool map_server_sid (string channel_id, string server_sid)
    {
      var id = ids.lookup (channel_id);
      if (id != null)
      {
        id.server_sid = server_sid;
        ids[channel_id] = id;
      }
      else
      {
        warning ("Can't map server_sid without session_id mapping for channel %s", channel_id);
        return false;
      }
      return true;
    }
  }
}
