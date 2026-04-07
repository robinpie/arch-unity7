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

namespace Unity.HomeScope.SearchUtil
{
  /*
   * Simple wrapper for GLib's source id that calls Source.remove once
   * it gets out of scope.
   */
  [Compact]
  public class GLibSourceWrapper
  {
    public uint source_id;
    public SourceFunc src_cb;

    public GLibSourceWrapper (uint timeout, owned SourceFunc cb)
    {    
      src_cb = (owned)cb;
      if (timeout == 0)
      {
        source_id = 0;
        Idle.add (cb_wrapper);
      }
      else
      {
        source_id = Timeout.add (timeout, cb_wrapper);
      }
    }

    private bool cb_wrapper ()
    {
      bool status = src_cb ();
      if (!status)
        source_id = 0;
      return status; 
    }

    public void remove ()
    {
      if (source_id > 0)
      {
        GLib.Source.remove (source_id);
        source_id = 0;
      }
    }

    ~GLibSourceWrapper ()
    {
      remove ();
    }
  }

  /*
   * Simple wrapper for signal id that disconnects it
   * when it gets out of scope.
   */
  [Compact]
  public class SignalWrapper
  {
    public unowned Object obj;
    public ulong sig_id;

    public SignalWrapper (Object o, ulong signal_id)
    {
      obj = o;
      sig_id = signal_id;
    }

    ~SignalWrapper ()
    {
      SignalHandler.disconnect (obj, sig_id);
    }
  }

  public bool update_filters (HashTable<string, Gee.Set<string>?> search_scopes,
                                                               List<SmartScopes.RecommendedScope?>? recommendations,
                                                               Unity.AggregatedScopeSearch scope_search,
                                                               bool check_result_counts)
  {
    bool changed = false;

    // create a lookup for recommended scopes ids and search_scopes.
    // recommendations include only specific scopes, so for every recommended scope insert its master as well.
    var rec_lookup = new Gee.TreeSet<string> ();

    if (recommendations != null)
    {
      foreach (var scope in recommendations)
      {
        string master_scope_id = get_master_id_from_scope_id (scope.scope_id);
        
        rec_lookup.add (scope.scope_id);
        rec_lookup.add (master_scope_id);
      }
    }

    search_scopes.foreach ((master, subscopes) =>
    {
      rec_lookup.add (master);
      if (subscopes != null)
      {
        foreach (var scope in subscopes)
          rec_lookup.add (scope);
      }
      else //no subscopes specified - assume all subscopes FIXME: use meta registry
      {
        var registry = MetaScopeRegistry.instance ();
        var sub_scopes = registry.get_subscopes (master);
        if (sub_scopes != null)
        {
          foreach (var scope_id in sub_scopes)
          {
            rec_lookup.add (scope_id);
          }
        }
      }
    });

    var home_channel = scope_search.channel_id;
    unowned Unity.OptionsFilter categories = scope_search.get_filter ("categories") as Unity.OptionsFilter;
    unowned Unity.OptionsFilter sources = scope_search.get_filter ("sources") as Unity.OptionsFilter;
    var mgr = CategoryManager.instance ();

    if (categories != null)
    {
      foreach (var opt in categories.options) //master scopes
      {
        int result_count = mgr.get_result_count (home_channel, opt.id);
        debug ("Results for %s: %d", opt.id, result_count);
        bool value = false;
        if (rec_lookup.contains (opt.id))
          value = check_result_counts == false || result_count > 0;
        else
          value = false;
        if (opt.active != value)
        {
          opt.active = value;
          changed = true;
        }
        debug ("Setting category filter %s: %s", opt.id, opt.active.to_string ());
      }
    }
    else
    {
      warning ("Couldn't get categories filter");
    }

    if (sources != null)
    {
      foreach (var opt in sources.options)
      {
        // light up subscope in sources filter if it has results
        int result_count = mgr.get_result_count (home_channel, opt.id);
        bool value = false;
        if (rec_lookup.contains (opt.id))
          value = check_result_counts == false || result_count > 0;
        else
          value = false;
        if (opt.active != value)
        {
          opt.active = value;
          changed = true;
        }
        debug ("Setting sources filter %s: %s", opt.id, opt.active.to_string ());
      }
    }
    else
    {
      warning ("Couldn't get sources filter");

      if (categories == null)
        return false; // no filters
    }

    return changed;
  }

  public void build_search_scopes_list (string scope_id, HashTable<string, Gee.Set<string>?> search_scopes)
  {
    var registry = MetaScopeRegistry.instance ();
    if (!registry.is_master (scope_id))
    {
      var master_id = get_master_id_from_scope_id (scope_id);
      if (master_id != null) // if null, then treat it as a non-master scope that connects directly to home
      {
        var subscopes = search_scopes.lookup (master_id);
        if (subscopes == null)
        {
          subscopes = new Gee.TreeSet<string?> ();
          search_scopes[master_id] = subscopes;
        }
        subscopes.add (scope_id);
        return;
      }
    }
    search_scopes[scope_id] = null;
  }

  /**
   * Populate search hints with "subscopes-filter" value, suitable for passing to search_scope.
   */
  public void set_subscopes_filter_hint (HashTable<string, Variant> hints, HashTable<string, Gee.Set<string>?> search_scopes, string scope_id)
  {
    var subscopes = search_scopes.lookup (scope_id);
    if (subscopes != null)
    {
      var src_filter_var = new GLib.Variant.strv (subscopes.to_array ());
      hints ["subscopes-filter"] = src_filter_var;
    }
  }

  /**
    Populate list of master scopes and their subscopes to query based on sources and categories filters.
    search_scopes maps master scope_id to its subscopes.
   */
  public void scopes_to_query_from_filters (OptionsFilter sources_filter, OptionsFilter cat_filter, HashTable<string, Gee.Set<string>?> search_scopes)
  {
    if (sources_filter == null || cat_filter == null)
      return;

    Gee.Set<string> requested_ids = new Gee.TreeSet<string> ();

    foreach (var opt in sources_filter.options) //each option is a subscope id
    {
      if (opt.active)
          requested_ids.add (opt.id);
    }

    foreach (var cat in cat_filter.options)
    {
      if (cat.active)
        requested_ids.add (cat.id);
    }

    scopes_to_query_from_requested_ids (requested_ids, search_scopes);
  }

  public string[] scopes_to_query_to_array (HashTable<string, Gee.Set<string>?> search_scopes)
  {
    string[] scopes = new string [0];
    search_scopes.foreach ((master_id, subscopes) =>
    {
      scopes += master_id;
      if (subscopes != null)
      {
        foreach (var id in subscopes)
        {
          scopes += id;
        }
      }
    });
    return scopes;
  }

  public string? get_master_id_from_scope_id (string scope_id)
  {
    var parts = scope_id.split ("-", 2);
    if (parts.length == 2)
      return "%s.scope".printf (parts[0]);
    return null;
  }

  public bool scopes_to_query_from_requested_ids (Gee.Set<string> requested_scope_ids, HashTable<string, Gee.Set<string>?> search_scopes)
  {
    bool found = false;
    var registry = MetaScopeRegistry.instance ();

    foreach (var req_scope_id in requested_scope_ids)
    {
      if (registry.is_master (req_scope_id))
      {
        if (!found)
          search_scopes.remove_all ();

        if (!search_scopes.contains (req_scope_id))
          search_scopes[req_scope_id] = null;
        found = true;
      }
      else // user may have requested a subscope (not a master scope), we need to find a master that needs to receive the query
      {
        var master_id = get_master_id_from_scope_id (req_scope_id);
        if (master_id != null)
        {
          if (!found)
            search_scopes.remove_all ();

          Gee.Set<string>? subscopes = search_scopes.lookup (master_id);
          if (subscopes == null)
          {
            subscopes = new Gee.TreeSet<string> ();
          }
          subscopes.add (req_scope_id);
          search_scopes[master_id] = subscopes;
          found = true;
        }
        else
        {
          warning ("Requested scope '%s' doesn't match any known scope id", req_scope_id);
        }
      }
    }
    return found;
  }
}
