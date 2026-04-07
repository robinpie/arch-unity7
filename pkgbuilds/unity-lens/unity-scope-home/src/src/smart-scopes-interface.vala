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
  
  public enum ScopeType
  {
    ClientScope,
    ServerScope
  }

  public struct RecommendedScope
  {
    string scope_id;
    ScopeType scope_type;
  }

  public struct RemoteScopeInfo
  {
    string scope_id;
    string name;
    string description;
    string icon_hint;
    string screenshot_url;
    string[] keywords;
  }

  public delegate void SmartScopeResult (string scope_id, Variant[] row);
  public delegate void SmartScopeRecommendations (string server_sid, List<RecommendedScope?> scopes);

  public interface SmartScopeClientInterface : Object
  {
    public signal void metrics_event_added (SmartScopesMetrics.AbstractMetricEvent event);

    public abstract string create_session_id ();
    public abstract async void search (string query, string form_factor, 
                                       string session_id, string[] scopes,
                                       string[] disabled_scopes,
                                       owned SmartScopeResult result_cb,
                                       owned SmartScopeRecommendations recommend_cb, 
                                       GLib.Cancellable? cancellable) throws Error;
    public abstract async Preview? preview (string server_sid,
                                            string session_id, string result_id,
                                            ScopeResult result,
                                            GLib.Cancellable? cancellable) throws Error;
    public abstract async RemoteScopeInfo[]? remote_scopes (GLib.Cancellable? cancellable) throws Error;
    public abstract int num_feedback_events ();
    public abstract async void send_feedback (GLib.Cancellable? cancellable) throws Error;
    public abstract void add_click_event (string session_id, string server_sid,
                                          string scope_id, DateTime timestamp);
    public abstract void add_preview_event (string session_id, string server_sid,
                                            string scope_id, DateTime timestamp);
    public abstract void add_found_event (string session_id, string server_sid,
                                          Gee.Map<string, int> scope_results,
                                          DateTime timestamp);
  }
}
