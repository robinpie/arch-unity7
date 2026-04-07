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

namespace Unity.HomeScope.SmartScopes
{
  public class SmartScopesMetrics: Object
  {
    public abstract class AbstractMetricEvent : Object
    {
      public int64 timestamp { get; set; }
      public string session_id { get ; set; }
      public string server_sid { get; set; }
      public abstract void json_for_event (Json.Builder builder);
      public abstract string get_event_type ();

      public void get_json (Json.Builder builder, string prev_session_id, string prev_server_sid)
      {
        var dict = builder.begin_object ();
        dict.set_member_name ("type");
        dict.add_string_value (get_event_type ());
        dict.set_member_name ("timestamp");
        dict.add_int_value (timestamp);
        dict.set_member_name ("session_id");
        dict.add_string_value (session_id);
        if (prev_session_id.length == 0 || session_id != prev_session_id || server_sid != prev_server_sid)
        {
          dict.set_member_name ("server_sid");
          dict.add_string_value (server_sid);
        }
        json_for_event (dict);
        dict.end_object ();
      }
    }

    public signal void event_added (AbstractMetricEvent event);

    public class ClickMetricEvent : AbstractMetricEvent
    {
      public string scope_id { get; set; }

      public ClickMetricEvent (string session_id, string server_sid, int64 timestamp, string scope_id)
      {
        Object (session_id: session_id, server_sid: server_sid, timestamp: timestamp, scope_id: scope_id);
      }

      public override string get_event_type ()
      {
        return "clicked";
      }

      public override void json_for_event (Json.Builder builder)
      {
        builder.set_member_name ("clicked_scope");
        builder.add_string_value (scope_id);
      }
    }

    public class PreviewMetricEvent : AbstractMetricEvent
    {
      public string scope_id { get; set; }

      public PreviewMetricEvent (string session_id, string server_sid, int64 timestamp, string scope_id)
      {
        Object (session_id: session_id, server_sid: server_sid, timestamp: timestamp, scope_id: scope_id);
      }

      public override string get_event_type ()
      {
        return "previewed";
      }

      public override void json_for_event (Json.Builder builder)
      {
        builder.set_member_name ("previewed_scope");
        builder.add_string_value (scope_id);
      }
    }

    public class FoundMetricEvent : AbstractMetricEvent
    {
      public Gee.Map<string, int> scope_results { get; set; }

      public FoundMetricEvent (string session_id, string server_sid, int64 timestamp, Gee.Map<string, int> scope_results)
      {
        Object (session_id: session_id, server_sid: server_sid, timestamp: timestamp, scope_results: scope_results);
      }

      public override string get_event_type ()
      {
        return "found";
      }

      public override void json_for_event (Json.Builder builder)
      {
        builder.set_member_name ("results");
        var resarray = builder.begin_array ();
        var iter = scope_results.map_iterator ();
        while (iter.next ())
        {
          var scoperes = resarray.begin_array ();
          scoperes.add_string_value (iter.get_key ());
          scoperes.add_int_value (iter.get_value ());
          scoperes.end_array ();
        }
        resarray.end_array ();
      }
    }

    private Gee.ArrayList<AbstractMetricEvent?> events = new Gee.ArrayList<AbstractMetricEvent?> ();
    private Json.Generator json_generator = new Json.Generator ();

    private void add_event (AbstractMetricEvent ev)
    {
      events.add (ev);
      event_added (ev);
    }

    public void add_click_event (string session_id, string server_sid, string scope_id, DateTime timestamp)
    {
      var ev = new ClickMetricEvent (session_id, server_sid, timestamp.to_unix (), scope_id);
      add_event (ev);
    }

    public void add_preview_event (string session_id, string server_sid, string scope_id, DateTime timestamp)
    {
      var ev = new PreviewMetricEvent (session_id, server_sid, timestamp.to_unix (), scope_id);
      add_event (ev);
    }

    public void add_found_event (string session_id, string server_sid, Gee.Map<string, int> scope_results, DateTime timestamp)
    {
      var ev = new FoundMetricEvent (session_id, server_sid, timestamp.to_unix (), scope_results);
      add_event (ev);
    }

    public int num_events
    {
     get
     {
      return events.size;
     }
    }

    public string get_json ()
    {
      var builder = new Json.Builder ();
      var ev_array = builder.begin_array ();
      var iter = events.iterator ();

      string prev_session_id = "";
      string prev_server_sid = "";
      while (iter.next ())
      {
        var ev = iter.get ();
        ev.get_json (ev_array, prev_session_id, prev_server_sid);
        prev_session_id = ev.session_id;
        prev_server_sid = ev.server_sid;
      }
      builder.end_array ();

      size_t len;
      json_generator.set_root (builder.get_root ());
      return json_generator.to_data (out len);
    }

    public void clear_events ()
    {
      events.clear ();
    }
  }
}
 
