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

const string ICON_PATH = Config.DATADIR + "/icons/unity-icon-theme/places/svg/";
const int ICON_COLUMN = 1;

// metrics will get flushed and sent to the server after METRICS_MIN_NUM_EVENTS, or every METRICS_SEND_INTERVAL (whichever comes first).
const int METRICS_SEND_INTERVAL_SECS = 600;
const int METRICS_MIN_NUM_EVENTS = 10;
const int REMOTE_SCOPES_INITIAL_RETRY_INTERVAL_SECS = 2; //initial one, it will get increased with each retry
const int REMOTE_SCOPES_RETRY_INTERVAL_MAX_SECS = 600; //will retry after that many seconds tops
const uint SMART_SCOPES_RECOMMENDATIONS_CUTOFF = 5; //how many of the recommended scopes should be taken into account
const int SMART_SCOPES_QUERY_MIN_DELAY_MS = 50;
const int SMART_SCOPES_QUERY_MAX_DELAY_MS = 100;
const int CATEGORY_REORDER_TIME_MS = 1500;
const uint FLUSH_DELAY_MS = 1500;
const string[] ALWAYS_REORDER_SCOPE_IDS = {"applications.scope", "files.scope"};
const string SCOPES_QUERY_SCHEMA_PREFIX = "x-unity-no-preview-scopes-query://";

public class HomeScope : Unity.AggregatorScope
{
  private ScopeManager scope_mgr = new ScopeManager ();
  private SearchQueryState query_state = new SearchQueryState ();
  private FilterState filter_state = new FilterState ();
  private KeywordSearch keywords_search = new KeywordSearch ();
  private bool smart_scopes_initialized = false;
  public bool smart_scopes_ready { get; internal set; default = false; }
  private SmartScopes.ChannelIdMap channel_id_map = new SmartScopes.ChannelIdMap ();
  private uint metrics_timer;
  private int remote_scopes_retry_count = 0;
  private RemoteScopeRegistry remote_scope_registry;
  private uint category_reorder_time_ms = 0; // minimum time after which category reordering can happen (in milliseconds)
  private uint flush_delay_ms = 0; //minimum time after which results will be shown in the dash (in milliseconds)
  private NetworkMonitor? netmon = null;
  private bool remote_scopes_request_running = false;
  private ulong netmon_sig_id = 0;

  public HomeScope ()
  {
    Object (dbus_path: "/com/canonical/unity/home",
            id:"home.scope",
            merge_mode: Unity.AggregatorScope.MergeMode.OWNER_SCOPE);
  }

  protected override void constructed ()
  {
    base.constructed ();

    var reorder_time = Environment.get_variable ("HOME_SCOPE_REORDER_TIME"); //in milliseconds
    var flush_delay_ms_time = Environment.get_variable ("HOME_SCOPE_FLUSH_DELAY"); //in milliseconds

    if (flush_delay_ms_time != null)
      flush_delay_ms = int.parse (flush_delay_ms_time);
    if (flush_delay_ms == 0)
      flush_delay_ms = FLUSH_DELAY_MS;

    if (reorder_time != null)
      category_reorder_time_ms = int.parse (reorder_time);
    if (category_reorder_time_ms == 0)
      category_reorder_time_ms = CATEGORY_REORDER_TIME_MS;

    if (flush_delay_ms > category_reorder_time_ms)
      category_reorder_time_ms = flush_delay_ms;

    automatic_flushing = false;
    discover_scopes_sync ();

    CategoryManager.instance ().set_default_sort_order_from_dconf ();

    var reg = ScopeRegistry.instance ();
    if (reg.scopes == null)
    {
      critical ("No scopes found. Please check your installation");
      return;
    }

    update_search_hint ();

    keywords_search.rebuild ();

    populate_filters ();
    populate_categories ();

    debug ("Starting master scopes");
    scope_mgr.start_master_scopes ();

    debug ("Exporting home scope");
    export ();

    scope_mgr.disabled_scopes_changed.connect (() =>
    {
      debug ("The list of disabled scopes has changed.");
      populate_filters ();
    });

  }

  private static bool discovery_started = false;

  public static void discover_scopes_sync ()
  {
    if (discovery_started) return;
    discovery_started = true;

    debug ("Starting scope discovery");

    var ml = new MainLoop ();

    var reg = ScopeRegistry.instance ();
    reg.find_scopes.begin ((obj, res) =>
    {
      reg.find_scopes.end (res);
      ml.quit ();
    });
    ml.run ();
    MetaScopeRegistry.instance ().update (reg, null);
  }

  internal void update_search_hint ()
  {
    debug ("Updating search hint");

    if (!scope_mgr.remote_content_search)
      search_hint = _("Search your computer");
    else
      search_hint = _("Search your computer and online sources");
  }

  internal void populate_filters ()
  {
    var reg = ScopeRegistry.instance ();
    var scopes_lut = new Gee.TreeSet<string> (); // lookup for scope ids that got added to sources filter

    var filter_list = new FilterSet ();

    var cat_filter = new Unity.CheckOptionFilter ("categories", _("Categories"));
    cat_filter.show_all_button = false;
    cat_filter.sort_type = Unity.OptionsFilter.SortType.DISPLAY_NAME;

    var src_filter = new Unity.CheckOptionFilter ("sources", _("Sources"));
    src_filter.sort_type = Unity.OptionsFilter.SortType.DISPLAY_NAME;

    var meta_reg = MetaScopeRegistry.instance ();

    foreach (var node in reg.scopes)
    {
      // display master scope in Category filter only if it has subscopes
      if (node.scope_info.visible && (
            node.scope_info.is_master == false || meta_reg.has_subscopes (node.scope_info.id)))
      {
        debug ("Category filter add: %s", node.scope_info.id);
        cat_filter.add_option (node.scope_info.id, node.scope_info.name); //add master scope id to 'Categories'
      }
      foreach (var scope in node.sub_scopes)
      {
        scopes_lut.add (scope.id);
        if (scope.visible && !scope_mgr.is_disabled (scope.id)) // should we also hide subsope if master is not visible?
        {
          debug ("Sources filter add: %s", scope.id);
          src_filter.add_option (scope.id, scope.name);
        }
      }
    }

    if (smart_scopes_ready)
    {
      foreach (var scope in remote_scope_registry.get_scopes ())
      {
        if (!reg.is_master (scope.scope_id) && !scopes_lut.contains (scope.scope_id) && !scope_mgr.is_disabled (scope.scope_id))
        {
          debug ("Sources filter add: %s (remote scope)", scope.scope_id);
          src_filter.add_option (scope.scope_id, scope.name);
        }
      }
    }

    src_filter.show_all_button = false;
    filter_list.add (cat_filter);
    filter_list.add (src_filter);

    filters = filter_list;
  }

  /**
    * Populate home scope category list with master scopes.
    */
  internal void populate_categories ()
  {
    debug ("Setting home scope categorties");

    var cats = new CategorySet ();

    foreach (var node in ScopeRegistry.instance ().scopes)
    {
      var scope_info = node.scope_info;
      var scope_id = scope_info.id;

      debug ("Adding home scope category: %s", scope_id);
      CategoryManager.instance ().register (scope_id);
      var icon = new GLib.FileIcon (File.new_for_path (scope_info.category_icon ?? scope_info.icon));

      // if present, take the renderer from the .scope file's "global" category
      bool defines_global_category = false;
      foreach (var cat_def in scope_info.get_categories ())
      {
        if (cat_def.id == "global")
        {
          var cat = DefinitionsParser.parse_category_definition (cat_def, icon);
          if (cat == null) continue;
          defines_global_category = true;
          // override id, name etc.
          var real_cat = new Unity.Category (scope_id, scope_info.name,
                                             cat.icon_hint,
                                             cat.default_renderer);
          real_cat.content_type = cat.content_type;
          cats.add (real_cat);
          break;
        }
      }

      if (!defines_global_category)
      {
        var cat = new Unity.Category (scope_id, scope_info.name, icon);
        cats.add (cat);
      }
    }
    categories = cats; //set scope categories
  }

  // TODO: hook it up once API support it
  private void on_home_channel_closed (string channel_id)
  {
    query_state.remove (channel_id);
    channel_id_map.remove_channel (channel_id);
  }


  internal void handle_found_metrics (string home_channel_id, string session_id, string server_sid, List<SmartScopes.RecommendedScope?> recommendations)
  {
    var cat_mgr = CategoryManager.instance ();

    var result_counts = new Gee.HashMap<string, int> ();
    foreach (var scope_rec in recommendations)
    {
      int count = cat_mgr.get_result_count (home_channel_id, scope_rec.scope_id);
      debug ("Result count for recommended scope %s, home channel %s: %d", scope_rec.scope_id, home_channel_id, count);
      
      // we may get count = -1 in two cases:
      // - server recommended a client scope which we don't have (shouldn't happen if server considers removed_scopes properly).
      // - recommended client scope didn't produce any results.
      if (count >= 0)
        result_counts[scope_rec.scope_id] = count;
    }

    var timestamp = new DateTime.now_utc ();
  }

  internal override async ActivationResponse? activate (Unity.AggregatorActivation activation)
  {
    debug ("Activation request for scope %s, action_type=%u", activation.scope_id, activation.action_type);


    return null; // activation will be handled by actual scope
  }

  public override async void search (Unity.AggregatedScopeSearch scope_search)
  {
    debug ("------------ Search query: %s, channel %s -----------", scope_search.search_string, scope_search.channel_id);

    GLib.Cancellable cancellable = scope_search.search_context.cancellable.get_gcancellable();

    CategoryManager.instance ().observe (scope_search.channel_id, scope_search.results_model);

    bool wait_for_sss_query = false;
    bool wait_for_push = false;
    bool wait_for_search = false;
    bool sss_query_done = false;
    bool sss_query_started = false;
    uint num_scopes = 0;

    bool flushing_enabled = false;
    unowned string? form_factor = scope_search.search_context.search_metadata.form_factor;
    if (form_factor == null)
        form_factor = "unknown"; // set to 'unknown' as it's sent with smart scopes request
    bool disable_filter_updates = (form_factor != "desktop");
    
    if (disable_filter_updates)
      debug ("Filter updates disabled, form factor is %s", form_factor);

    // ids of scopes recommended by Smart Scope Service
    var recommended_search_scopes = new List<SmartScopes.RecommendedScope?> ();

    AsyncReadyCallback search_cb = (obj, res) =>
    {
      var search_obj = obj as Unity.AggregatedScopeSearch;
      try
      {
        var reply_dict = search_obj.search_scope.end (res);
        var iter = HashTableIter<string, Variant> (reply_dict);
        unowned string key;
        unowned Variant variant;

        while (iter.next (out key, out variant))
          scope_search.set_reply_hint (key, variant);
      }
      catch (Error err)
      {
        if (!(err is IOError.CANCELLED))
          warning ("Unable to search scope: %s", err.message);
      }
      if (--num_scopes == 0 && wait_for_search)
        search.callback ();
    };

    // data models that are populated by Smart Scopes search and pushed to respective scopes
    var push_data = new HashTable<string, Dee.SerializableModel> (str_hash, str_equal); //scope_id -> model
    int pending_push_count = 0;

    AsyncReadyCallback push_results_cb = (obj, res) =>
    {
      try
      {
        scope_search.push_results.end (res);
      }
      catch (Error err)
      {
        warning ("push_results failed: %s", err.message);
      }
      if (--pending_push_count == 0 && wait_for_push)
        search.callback ();
    };

    SourceFunc push_data_idle_cb = ()=>
    {
      var iter = HashTableIter<string, Dee.SerializableModel> (push_data);
      unowned string scope_id;
      unowned Dee.SerializableModel model;
      while (iter.next (out scope_id, out model))
      {
        debug ("Pushing results (%u rows) for scope %s", model.get_n_rows (), scope_id);
        ++pending_push_count;
        scope_search.push_results.begin (scope_id, model, {"global"}, push_results_cb);
      }
      push_data.remove_all ();
      return false;
    };

    // compare current search string with previous one to see if it's filters change only;
    // this impacts how we treat online recommendations and filters.
    var search_query_changed = query_state.search_query_changed (scope_search.channel_id, scope_search.search_string);

    // maps ids of scopes to search (master scopes or top-level scopes) to sub-scopes (according to filter or direct keyword search);
    // may map to null, in that case there is no filtering based on sub-scopes (all subscopes of given master will be queried).
    var search_scopes = new HashTable<string, Gee.Set<string>?> (str_hash, str_equal);

    // filters are updated during the search, but may need to be updated again afterwards if we find out there wero no results for some categories
    bool needs_filter_update = false;

    // default filter view flag; default view is kind of a special case that kicks in when search query is empty:
    // user can modify filters at will, and they are stored in gsettings key.
    // this flag prevents filter updates based on what scopes were actually searched and if they had results, so that user can comfortably
    // edit the all the filters without loosing them every second.
    bool default_view = false;

    bool empty_query = (scope_search.search_string.strip ().length == 0);

    // set if this search is a filter update only (e.g. search string is the same)
    bool filter_change_only = false;
    if (search_query_changed == SearchQueryChange.NOT_CHANGED)
    {
      filter_change_only = true;
      debug ("Filter change only");
    }

    // apply user filters only if search string is unchanged, the query is empty or smart scopes are disabled completly.
    // this mean user set filters *after* entering a query and we apply them;
    // otherwise smart scopes recommendations will take precedence.
    // if search query is empty, then apply default user's filters.
    unowned Unity.OptionsFilter categories_filter = scope_search.get_filter ("categories") as Unity.OptionsFilter;
    unowned Unity.OptionsFilter sources_filter = scope_search.get_filter ("sources") as Unity.OptionsFilter;

    bool relations_changed = false;

    // if query is empty but it's not just a filter change (i.e. state is REMOVES_FROM_PREVIOUS_QUERY or NEW_QUERY),
    // then apply default user filters.
    if (empty_query && search_query_changed != SearchQueryChange.NOT_CHANGED)
    {
      var default_filters = scope_mgr.home_lens_default_view;
      if (default_filters.length > 0)
      {
        debug ("Empty query, applying default filter view");
        needs_filter_update |= FilterState.set_filters (categories_filter, sources_filter, default_filters);
      }
      default_view = true;
    }

    if (!disable_filter_updates) {
      debug ("Updating filter interrelationships");
      relations_changed = filter_state.update_filter_relations (scope_search.channel_id, categories_filter, sources_filter);
    } else {
      needs_filter_update = false; //reset filter update flag if on the phone
    }

    // caution: push_filter_settings may get cancelled if we ever yield before;
    // in such case internal filter state must be updated later (at the end of search).
    if (relations_changed)
      push_filters_update (scope_search);
    debug ("Filter interrelationships changed: %s", relations_changed.to_string ());

    SearchUtil.scopes_to_query_from_filters (scope_search.get_filter ("sources") as Unity.OptionsFilter,
                                              scope_search.get_filter ("categories") as Unity.OptionsFilter,
                                              search_scopes);

    // if this search request is a filter change only and query is empty,
    // update user's default view in gsettings
    if (filter_change_only && empty_query && relations_changed)
    {
      default_view = true;
      debug ("Updating user's default filter view");
      scope_mgr.home_lens_default_view = SearchUtil.scopes_to_query_to_array (search_scopes);
    }

    // handle keywords (direct search)
    string? search_string = null;
    bool direct_search = false;
    unowned Gee.Set<string>? requested_scope_ids = keywords_search.process_query (
      scope_search.search_string, out search_string);
    if (requested_scope_ids != null && search_string != null)
    {
      debug ("Direct search query, search_string = '%s'", search_string);
      direct_search = SearchUtil.scopes_to_query_from_requested_ids (requested_scope_ids, search_scopes);
    }

    if (search_string == null)
      search_string = scope_search.search_string;
    else
      empty_query = (search_string.strip ().length == 0);

    uint push_data_idle_src = 0;

    var reorder_timer = new Timer ();
    // flush results after flush_delay_ms elapses
    var forced_flush_timer = new SearchUtil.GLibSourceWrapper (flush_delay_ms, () =>
    {
      if (scope_search.search_context.cancellable.is_cancelled ()) return false;
      debug ("Flush time reached");
      flushing_enabled = true;
      signal_categories_order (scope_search, recommended_search_scopes);
      scope_search.search_context.result_set.flush ();
      return false;
    });

    // flush results on end of each transaction if flush_delay_ms elapsed.
    var transaction_sig = new SearchUtil.SignalWrapper (scope_search, 
                                                        scope_search.transaction_complete.connect ((scope_id) =>
    {
      if (scope_search.search_context.cancellable.is_cancelled ()) return;
      debug ("Transaction complete for scope %s, time = %f", scope_id, reorder_timer.elapsed ());
      // only start flushing after the timeout; flush immediately if applications search finished so that
      // apps are immediately available, except for case when only filters changed (reduces flickering).
      if (flushing_enabled || (scope_id == "applications.scope" && search_query_changed != SearchQueryChange.NOT_CHANGED))
      {
        debug ("Flushing");
        bool reorder_enabled = reorder_timer.elapsed () * 1000 < category_reorder_time_ms;
        if (reorder_enabled || scope_id in ALWAYS_REORDER_SCOPE_IDS)
          signal_categories_order (scope_search, recommended_search_scopes);
        scope_search.search_context.result_set.flush ();
      }
    }));

    // may happen if remote-scopes haven't finished or returned an error
    debug ("Smart scopes not ready or not enabled for this query");

    // no filters set / no scopes to search, use always-search scopes
    if (search_scopes.size () == 0)
    {
      debug ("No scopes to search based on filters. Defaulting to always-search-scopes");
      SearchUtil.scopes_to_query_from_requested_ids (scope_mgr.get_always_search_scopes (), search_scopes);
    }

    num_scopes = search_scopes.size ();
    debug ("Dispatching search to %u scopes, home_channel=%s", num_scopes, scope_search.channel_id);

    // iterate over master scopes, dispatch search query
    foreach (var scope_id in search_scopes.get_keys ())
    {
      debug ("Dispatching search to scope %s", scope_id);
      // apply Sources (subscopes) filtering; pass subscopes requested via filters to relevant masterscopes
      var hints = new HashTable<string, Variant> (str_hash, str_equal);
      SearchUtil.set_subscopes_filter_hint (hints, search_scopes, scope_id);
      scope_search.search_scope.begin (scope_id, search_string,
                                        SearchType.GLOBAL, hints, search_cb);
    }

    bool use_recommended_scopes = false;
    
    // dispatch search to scopes recommended by smart scopes service
    if (smart_scopes_ready && !empty_query)
    {
      // wait for smart scopes service query to finish
      if (sss_query_started && !sss_query_done)
      {
        debug ("Waiting for Smart Scopes query to finish");
        wait_for_sss_query = true;
        yield;
      }

      try
      {
        cancellable.set_error_if_cancelled ();

        // update category order for each scope search finish
        // note: recommended scopes list may be initially empty, as it arrives after we initiate first local searches.
        if (flushing_enabled && reorder_timer.elapsed () * 1000 < category_reorder_time_ms)
        {
          signal_categories_order (scope_search, recommended_search_scopes);
        }

        if (push_data_idle_src > 0)
        {
          debug ("Flushing push results");
          push_data_idle_cb (); //make sure we flush all push_results
          if (pending_push_count > 0)
          {
            debug ("Waiting for results pushing to finish");
            wait_for_push = true;
            yield;
            wait_for_push = false;
          }
        }
      }
      catch (Error e)
      {
        debug ("The search for '%s' on channel %s was cancelled", scope_search.search_string, scope_search.channel_id);
        return;
      }
      finally
      {
        if (push_data_idle_src > 0)
          Source.remove (push_data_idle_src);
        push_data_idle_src = 0;
      }

      debug ("Got %u recommended scopes from Smart Scope Service", recommended_search_scopes.length ());
    }

    // only use recommended scopes if search query was changed (thus user-selected filters are reset).
    if (search_query_changed != SearchQueryChange.NOT_CHANGED && !direct_search && recommended_search_scopes.length () > 0)
      use_recommended_scopes = true;

    // only update filters with searched scopes when not in the default view, otherwise default view filters may get de-selected
    if (!default_view && !disable_filter_updates)
    {
      debug ("Updating filter state");
      needs_filter_update |= SearchUtil.update_filters (search_scopes, use_recommended_scopes ? recommended_search_scopes : null,
                                                        scope_search, false);
    }

    if (needs_filter_update)
    {
      var filters = FilterState.create_filter_set (scope_search);
      debug ("Sending updated filters");
      scope_search.push_filter_settings (filters);
    }
    
    if (use_recommended_scopes)
    {
        debug ("Search query changed, querying recommended scopes");

        // ids of recommended scopes
        var extra_search_scopes = new HashTable<string, Gee.Set<string>?> (str_hash, str_equal);
        var all_search_scopes = new HashTable<string, Gee.Set<string>?> (str_hash, str_equal);

        // build the list of master-scopes to search and subscopes_filter to pass to them based on recommendations.
        // note that recommendations may contains subscopes, so build_search_scopes_list will take care of all the magic.
        // also note that, because of that, we can't filter out master scopes that were already searched before (as part
        // of always-search list) just now - this happens below in the search loop.
        foreach (var scope_rec in recommended_search_scopes)
        {
          if (scope_rec.scope_type == SmartScopes.ScopeType.ClientScope)
            SearchUtil.build_search_scopes_list (scope_rec.scope_id, extra_search_scopes);
          SearchUtil.build_search_scopes_list (scope_rec.scope_id, all_search_scopes);
        }

        foreach (var scope_id in extra_search_scopes.get_keys ())
        {
          // ignore this master if it was already searched before.
          if (!search_scopes.contains (scope_id))
          {
            debug ("Dispatching search to recommended scope %s", scope_id);
            ++num_scopes;
            var hints = new HashTable<string, Variant> (str_hash, str_equal);
            SearchUtil.set_subscopes_filter_hint (hints, all_search_scopes, scope_id);
            scope_search.search_scope.begin (scope_id, search_string, SearchType.GLOBAL, hints, search_cb);
          }
        }
    }
    else
    {
      debug ("Search query not changed or direct search active, ignoring recommended scopes");
    }

    // wait for the results from recommended scopes
    if (num_scopes > 0)
    {
      debug ("Waiting for search (recommended scopes) to finish");
      wait_for_search = true;
      yield;
    }
    
    if (cancellable.is_cancelled ())
    {
      debug ("The search for '%s' on channel %s was cancelled", scope_search.search_string, scope_search.channel_id);
      return;
    }

    debug ("search finished");

    if (scope_search.results_model.get_n_rows () == 0)
    {
      scope_search.set_reply_hint ("no-results-hint",
                                   _("Sorry, there is nothing that matches your search."));
    }

    // update the category order only if we're in the reordering time slot,
    // or if this search was fast and we didn't even flush yet
    if (!scope_search.search_context.cancellable.is_cancelled ())
    {
      if ((flushing_enabled && reorder_timer.elapsed () * 1000 < category_reorder_time_ms) || !flushing_enabled)
      {
        signal_categories_order (scope_search, recommended_search_scopes);
      }
    }

    // update filter state again, but this time check result counts and send the update only if any of the highlighted masters has no results
    if (!default_view && !disable_filter_updates)
      needs_filter_update |= SearchUtil.update_filters (search_scopes, use_recommended_scopes ? recommended_search_scopes : null, scope_search, filter_change_only == false);

    if (needs_filter_update)
    {
      filter_state.set_state_from_filters (scope_search.channel_id, categories_filter, sources_filter);
      push_filters_update (scope_search);
    }

    if (use_recommended_scopes)
    {
      debug ("Adding 'found' metrics");

      string? server_sid = channel_id_map.server_sid_for_channel (scope_search.channel_id);
      string? session_id = channel_id_map.session_id_for_channel (scope_search.channel_id);

      // server_sid may be null if we didn't query smart scopes service
      if (server_sid != null)
        handle_found_metrics (scope_search.channel_id, session_id, server_sid, recommended_search_scopes);
    }

    debug ("All search activities finished");
  }

  public override int category_index_for_scope_id (string scope_id)
  {
    return CategoryManager.instance ().get_category_index (scope_id);
  }

  private void push_filters_update (AggregatedScopeSearch scope_search)
  {
    var filters = FilterState.create_filter_set (scope_search);
    debug ("Sending updated filters");
    scope_search.push_filter_settings (filters);
  }

  private void signal_categories_order (AggregatedScopeSearch search, List<SmartScopes.RecommendedScope?> recommended_scopes)
  {
    var cats = CategoryManager.instance ().get_category_order (search.search_string, search.channel_id, search.results_model, recommended_scopes);
    debug ("Updating categories order");
    search.category_order_changed (cats); //TODO only signal if order really changed?
  }
}

}
