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
public extern unowned Json.Builder json_builder_add_value (Json.Builder builder, owned Json.Node node);

namespace Unity.HomeScope.SmartScopes {

    public class SmartScopesClient : SmartScopeClientInterface, Object
    {
      public static const string SERVER = "https://productsearch.ubuntu.com";
      public static const string SEARCH_URI = "/smartscopes/v1/search";
      public static const string PREVIEW_URI = "/smartscopes/v1/preview";
      public static const string FEEDBACK_URI = "/smartscopes/v1/feedback";
      public static const string REMOTE_SCOPES_URI = "/smartscopes/v1/remote-scopes";
      
      private SmartScopesPreviewParser preview_parser = new SmartScopesPreviewParser ();

      internal class SearchMessage : Object
      {
        internal Soup.Message msg;
        SmartScopeResult result_cb;
        SmartScopeRecommendations recommend_cb;
        private StringBuilder current_line = new StringBuilder ();
        private SearchResponseHandler response_handler;

        construct
        {
          response_handler = new SearchResponseHandler ();
        }

        internal SearchMessage (owned SmartScopeResult result_cb, owned SmartScopeRecommendations recommend_cb)
        {
          this.result_cb = (owned) result_cb;
          this.recommend_cb = (owned) recommend_cb;
        }

        internal void on_data_chunk (Soup.Buffer buffer)
        {
          var bld = new StringBuilder.sized (buffer.length + 1);
          bld.append_len ((string)buffer.data, (ssize_t)buffer.length);
          string buffer_str = bld.str;

          int eol_idx = 0;
          int start = 0;

          while (eol_idx >= 0)
          {
            eol_idx = buffer_str.index_of ("\n", start);
            if (eol_idx < 0)
            {
              current_line.append (buffer_str.substring (start));
            }
            else
            {
              current_line.append (buffer_str.substring (start, eol_idx - start));
              if (current_line.str != "")
              {
                try
                {
                  response_handler.parse_results_line (current_line.str, result_cb, recommend_cb);
                }
                catch (Error e)
                {
                  warning ("Error parsing server data: %s", e.message);
                }
                current_line.truncate ();
              }
              start = eol_idx + 1;
            }
          };
        }

        /*
         Parse remaining bytes in the buffer; may happen if there was no eol in the last chunk
         */
        internal void finalize_parsing () throws Error
        {
          if (current_line.str != "")
          {
            response_handler.parse_results_line (current_line.str, result_cb, recommend_cb);
            current_line.truncate ();
          }
        }

        internal async void do_search (Soup.Session session, string uri, GLib.Cancellable? cancellable) throws Error
        {
          debug ("Sending request: %s", uri);

          current_line.truncate ();

          msg = new Soup.Message ("GET", uri);
          msg.response_body.set_accumulate (false); //incoming data handled by on_data_chunk, no need to get body
          var sig_id = msg.got_chunk.connect (on_data_chunk);

          try
          {
            msg = yield queue_soup_message (session, msg, cancellable);
            cancellable.set_error_if_cancelled ();
            if (msg.status_code == 200)
              finalize_parsing ();
          }
          finally
          {
            SignalHandler.disconnect (msg, sig_id);
          }
        }
      }

      internal class PreviewRequest: Object
      {
        internal Soup.Message msg;

        public PreviewRequest ()
        {
        }

        public static string scope_result_to_json (ScopeResult result)
        {
          Json.Generator json_generator = new Json.Generator ();
          var builder = new Json.Builder ();
          Json.Builder b = builder.begin_object ();
          b.set_member_name ("title");
          b.add_string_value (result.title);
          b.set_member_name ("uri");
          b.add_string_value (result.uri);
          b.set_member_name ("icon_hint");
          b.add_string_value (result.icon_hint);
          b.set_member_name ("comment");
          b.add_string_value (result.comment);

          b.set_member_name ("metadata");

          var content = result.metadata.lookup ("content"); //master scopes put 'metadata' in 'content'
          var metadata_var = content;

          Json.Node meta_node = Json.gvariant_serialize (metadata_var);
          json_builder_add_value (b, meta_node); //workaround for API bug
          b.end_object ();

          json_generator.set_root (builder.get_root ());
          size_t len;
          string res = json_generator.to_data (out len);      
          return res;
        }
        
        public async string? request_preview (Soup.Session session, string uri, string server_sid, ScopeResult result, GLib.Cancellable? cancellable) throws Error
        {
          debug ("Sending request: %s, server_sid=%s", uri, server_sid);

          var serialized_result = scope_result_to_json (result);

          msg = new Soup.Message ("GET", uri);
          msg.request_headers.append ("X-SERVER-SID", server_sid );
          msg.request_headers.append ("X-PREVIOUS-RESULT", serialized_result);

          msg = yield queue_soup_message (session, msg, cancellable);
          if (msg.status_code == 200)
            return (string)msg.response_body.data;

          return null;
        }
      }

      private Soup.Session session;
      private string base_uri;
      private string feedback_uri;
      private string geo_store;
      private PlatformInfo info;
      private HashTable<string, SearchMessage?> search_requests = new HashTable<string, SearchMessage?> (str_hash, str_equal);
      private HashTable<string, PreviewRequest?> preview_requests = new HashTable<string, PreviewRequest?> (str_hash, str_equal);
      private SmartScopesMetrics metrics;

      construct
      {
        metrics = new SmartScopesMetrics ();
        metrics.event_added.connect ((ev) => {
                metrics_event_added (ev);
        });
      }

      public SmartScopesClient (PlatformInfo platform)
      {
        this.info = platform;
        init_soap ();
      }

      private void init_soap ()
      {
        base_uri = Environment.get_variable ("SMART_SCOPES_SERVER");
        if (base_uri == null) base_uri = SERVER;
        feedback_uri = "%s%s".printf (base_uri, FEEDBACK_URI);
        geo_store = Environment.get_variable ("SMART_SCOPES_GEO_STORE");

        session = new Soup.SessionAsync ();
        session.ssl_use_system_ca_file = true;
        session.ssl_strict = true;
        session.user_agent = "Unity Home Scope v" + Config.VERSION;
        session.add_feature_by_type (typeof (SoupGNOME.ProxyResolverGNOME));
      }

      public string create_session_id ()
      {
        var sid = UUID.randomized_time_uuid ();
        return sid;
      }

      public async RemoteScopeInfo[]? remote_scopes (GLib.Cancellable? cancellable) throws Error
      {
        // wait for the platform info to have all properties ready
        if (!info.is_ready)
        {
          var sig_id = info.notify["is-ready"].connect (() =>
          {
            if (info.is_ready) remote_scopes.callback ();
          });
          yield;
          SignalHandler.disconnect (info, sig_id);
        }
        var uri = new Soup.URI (base_uri + REMOTE_SCOPES_URI);
        var query_dict = new HashTable<string, string> (str_hash, str_equal);
        if (info.locale != null)
        {
          query_dict["locale"] = info.locale;
        }
        if (info.build_id != null)
        {
          query_dict["build_id"] = info.build_id;
        }
        if (info.country_code != null)
        {
          query_dict["country_code"] = info.country_code;
        }
        if (info.network_code != null)
        {
          query_dict["network_code"] = info.network_code;
        }
        uri.set_query_from_form (query_dict);
        
        debug ("Sending request: %s", uri.to_string (false));

        var msg = new Soup.Message.from_uri ("GET", uri);
        msg = yield queue_soup_message (session, msg, cancellable);
        
        if (msg.status_code == 200)
        {
          var json_parser = new Json.Parser ();
          json_parser.load_from_data ((string)msg.response_body.data);
          var parser = new RemoteScopesParser ();
          return parser.parse (json_parser);
        }
        return null;
      }

      public async void search (string query, string form_factor,
                                string session_id,
                                string[] scopes, string[] disabled_scopes,
                                owned SmartScopeResult result_cb,
                                owned SmartScopeRecommendations recommend_cb,
                                GLib.Cancellable? cancellable) throws Error
      {
        var uri = build_search_uri (query, form_factor, session_id, scopes, disabled_scopes);
        debug ("Dispatching query '%s' to %s", query, uri);

        if (search_requests.contains (session_id))
          session.cancel_message (search_requests[session_id].msg, Soup.KnownStatusCode.CANCELLED);

        var searcher = new SearchMessage ((owned) result_cb, (owned) recommend_cb);
        search_requests[session_id] = searcher;

        try
        {
          yield searcher.do_search (session, uri, cancellable);
        }
        finally
        {
          search_requests.remove (session_id);
        }
      }

      public async Preview? preview (string server_sid, string session_id, string result_id, ScopeResult result, GLib.Cancellable? cancellable) throws Error
      {
        var uri = build_preview_uri (session_id, result_id);
        debug ("Dispatching preview request for session_id %s, server_sid %s, result_id %s to %s", session_id, server_sid, result_id, uri);

        if (search_requests.contains (session_id))
          session.cancel_message (search_requests[session_id].msg, Soup.KnownStatusCode.CANCELLED);

        var req = new PreviewRequest ();
        preview_requests[session_id] = req;

        string? data = null;
        try
        {
          data = yield req.request_preview (session, uri, server_sid, result, cancellable);
          if (data != null)
          {
            if (verbose_debug)
              debug ("Got preview: %s", data);

            var json_parser = new Json.Parser ();
            json_parser.load_from_data (data);
            return preview_parser.parse (json_parser.get_root ().get_object ());
          }
          else
          {
            warning ("No preview data");
          }
        }
        finally
        {
          preview_requests.remove (session_id);
        }
        
        return null;
      }

      public int num_feedback_events ()
      {
        return metrics.num_events;
      }

      public async void send_feedback (GLib.Cancellable? cancellable) throws Error
      {
        if (metrics.num_events > 0)
        {
          debug ("Sending metrics to %s, event count = %u", feedback_uri, metrics.num_events);
          
          var data = metrics.get_json ();
          if (Unity.HomeScope.verbose_debug)
            debug ("Metrics data: %s", data);

          var msg = new Soup.Message ("POST", feedback_uri);
          msg.set_request ("application/json", Soup.MemoryUse.COPY, data.data);

          try
          {
            msg = yield queue_soup_message (session, msg, cancellable);
            debug ("Metrics sending completed");
          }
          finally
          {
            metrics.clear_events ();
          }
        }
      }

      public void add_click_event (string session_id, string server_sid, string scope_id, DateTime timestamp)
      {
        metrics.add_click_event (session_id, server_sid, scope_id, timestamp);
      }

      public void add_preview_event (string session_id, string server_sid, string scope_id, DateTime timestamp)
      {
        metrics.add_preview_event (session_id, server_sid, scope_id, timestamp);
      }

      public void add_found_event (string session_id, string server_sid, Gee.Map<string, int> scope_results, DateTime timestamp)
      {
        metrics.add_found_event (session_id, server_sid, scope_results, timestamp);
      }

      internal string build_search_uri (string query, string form_factor, string session_id, string[] scopes = {}, string[] disabled_scopes = {})
      {
        var sb = new StringBuilder (base_uri);
        sb.append (SEARCH_URI);
        sb.append ("?q=");
        sb.append (Uri.escape_string (query, "", false));
        sb.append ("&platform=");
        sb.append (form_factor);
        sb.append_c ('-');
        sb.append (info.platform);
        sb.append ("&session_id=");
        sb.append (session_id);

        if (info.locale != null)
        {
          sb.append ("&locale=");
          sb.append (info.locale);
        }
        if (info.build_id != null)
        {
          sb.append ("&build_id=");
          sb.append (info.build_id);
        }
        if (info.country_code != null)
        {
          sb.append ("&country_code=");
          sb.append (info.country_code);
        }
        if (info.network_code != null)
        {
          sb.append ("&network_code=");
          sb.append (info.network_code);
        }
        if (info.added_scopes.length > 0)
        {
          var added_scopes = string.joinv (",", info.added_scopes);
          sb.append ("&added_scopes=");
          sb.append (added_scopes);
        }
        if (info.removed_scopes.length > 0 || disabled_scopes.length > 0)
        {
          var rscopes = info.removed_scopes;
          foreach (var id in disabled_scopes)
            rscopes += id;
          var removed_scopes = string.joinv (",", rscopes);
          sb.append ("&removed_scopes=");
          sb.append (removed_scopes);
        }
        if (scopes.length > 0)
        {
          var scopes_str = string.joinv (",", scopes);
          sb.append ("&scopes=");
          sb.append (scopes_str);
        }

        if (geo_store != null && geo_store.length > 0)
        {
          sb.append ("&geo_store=");
          sb.append (geo_store);
        }

        return sb.str;
      }

      internal string build_preview_uri (string session_id, string result_id)
      {
        var sb = new StringBuilder (base_uri);
        sb.append (PREVIEW_URI);
        sb.append ("?session_id=");
        sb.append (Uri.escape_string (session_id, "", false));
        sb.append ("&result_id=");
        sb.append (Uri.escape_string (result_id, "", false));

        if (info.locale != null)
        {
          sb.append ("&locale=");
          sb.append (info.locale);
        }

        return sb.str;
      }
    }

    public async Soup.Message? queue_soup_message (Soup.Session session, Soup.Message msg, GLib.Cancellable? cancellable) throws Error
    {
      session.queue_message (msg, (session_, msg_) =>
      {
        msg = msg_;
        queue_soup_message.callback ();
      });
      
      var cancelled = false;
      ulong cancel_id = 0;
      if (cancellable != null)
      {
        cancel_id = cancellable.cancelled.connect (() =>
        {
          cancelled = true;
          session.cancel_message (msg, Soup.KnownStatusCode.CANCELLED);
        });
      }
      
      yield;
      
      if (cancelled)
      {
        // we can't disconnect right away, as that would deadlock (cause
        // cancel_message doesn't return before invoking the callback)
        Idle.add (queue_soup_message.callback);
        yield;
        cancellable.disconnect (cancel_id);
        throw new IOError.CANCELLED ("Cancelled");
      }
      
      if (cancellable != null)
        cancellable.disconnect (cancel_id);
      
      if (msg.status_code < 100)
        throw new IOError.FAILED ("Request failed with error %u", msg.status_code);
      
      if (msg.status_code != 200)
      {
        warning ("Request returned status code %u", msg.status_code);
        if (msg.response_body.data != null)
          debug ("Response body: %s", (string)msg.response_body.data);
      }
      
      return msg;
    }
}
