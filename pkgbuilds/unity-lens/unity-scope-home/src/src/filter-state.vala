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

public class FilterState
{
  // list of scopes that were recently active in filters (sources+categories filter combined), per home channel
  private HashTable<string, Gee.Set<string>?> filter_state = new HashTable<string, Gee.Set<string>?> (str_hash, str_equal);

  public void set_state_from_filters (string channel_id, Unity.OptionsFilter categories, Unity.OptionsFilter sources)
  {
    if (categories == null || sources == null)
    {
      warning ("Categories or Sources filters missing");
      return;
    }

    // fill in current_state from categories and sources
    var new_state = new Gee.TreeSet<string> ();
    get_selected_scopes (categories, new_state);
    get_selected_scopes (sources, new_state);
    
    filter_state[channel_id] = new_state;
  }

  /**
   * Update filters of current scope_search so that selecting a master scope automatically
   * highlights its subscopes, or deselecting last subscope deselects its master.
   * This methods stores filter state in order to be able to detect what was selected/deselected.
   */
  public bool update_filter_relations (string channel_id, Unity.OptionsFilter categories, Unity.OptionsFilter sources)
  {
    bool changed = false;

    if (categories == null || sources == null)
    {
      warning ("Categories or Sources filters missing");
      return false;
    }
   
    // 1. determine current filter state from categories+sources.
    // 2. compare previous and current state to find out what was selected or unselected
    // 3. select/unselect respective masters and subscopesaq

    // fill in current_state from categories and sources
    var current_state = new Gee.TreeSet<string> ();
    get_selected_scopes (categories, current_state);
    get_selected_scopes (sources, current_state);

    debug ("Scopes in current_state: %d", current_state.size);
    if (verbose_debug)
    {
      foreach (var scope_id in current_state)
        debug ("Current active: %s", scope_id);
    }

    var previous_state = filter_state.lookup (channel_id);

    if (previous_state != null)
    {
      if (verbose_debug)
      {
        foreach (var scope_id in previous_state)
          debug ("Previous active: %s", scope_id);
      }

      var selected = new Gee.TreeSet<string> ();
      var unselected = new Gee.TreeSet<string> ();

      foreach (var scope_id in current_state)
      {
        if (!previous_state.contains (scope_id))
        {
          debug ("Current filter state - selected scope: %s", scope_id);
          selected.add (scope_id);
        }
      }

      if (selected.size == 0) // there is no need to check unselected if we found out a newly selected scope.
      {
        foreach (var scope_id in previous_state)
        {
          if (!current_state.contains (scope_id))
          {
            debug ("Current filter state - deselected scope: %s", scope_id);
            unselected.add (scope_id);
          }
        }
      }
      
      if (selected.size > 0)
        changed = update_filters_for_selected (categories, sources, selected);
      else if (unselected.size > 0)
        changed = update_filters_for_unselected (categories, sources, unselected);
      
    }
    else // first search - no previous state
    {
      debug ("No previous filter state");
      changed = update_filters_for_selected (categories, sources, current_state);
    }

    var new_state = new Gee.TreeSet<string> ();
    get_selected_scopes (categories, new_state);
    get_selected_scopes (sources, new_state);
    if (verbose_debug)
    {
      foreach (var scope_id in new_state)
      debug ("New active: %s", scope_id);
    }

    filter_state[channel_id] = new_state;

    if (changed)
    {
      sources.filtering = true;
      categories.filtering = true;
    }

    return changed;
  }

  internal void get_selected_scopes (Unity.OptionsFilter? filter, Gee.Set<string> scopes)
  {
    foreach (var opt in filter.options)
    {
      if (opt.active)
        scopes.add (opt.id);
    }
  }

  internal bool update_filters_for_selected (Unity.OptionsFilter? categories, Unity.OptionsFilter? sources, Gee.Set<string> selected_scopes)
  {
    bool changed = false;

    var reg = ScopeRegistry.instance ();
    var select_masters = new Gee.TreeSet<string> ();
    var select_subscopes_of = new Gee.TreeSet<string> ();

    // selected master - all subscopes need to be selected
    // selected subscope - master needs to be selected

    // find master scopes among selected scopes
    foreach (var scope_id in selected_scopes)
    {
      debug ("Scope %s in selected_scopes", scope_id);
      if (reg.is_master (scope_id))
      {
        select_subscopes_of.add (scope_id);
      }
      else
      {
        var master_id = SearchUtil.get_master_id_from_scope_id (scope_id);
        if (master_id != null)
        {
          select_masters.add (master_id);
        }
        else
        {
          warning ("No master scope info for subscope %s", scope_id); // this shouldn't really happen
        }
      }
    }
  
    // actual filter update
    foreach (unowned Unity.FilterOption opt in categories.options)
    {
      if (select_masters.contains (opt.id))
      {
        if (!opt.active)
        {
          opt.active = true;
          changed = true;
          debug ("Changing category filter %s: %s", opt.id, opt.active.to_string ());
        }
      }
    }

    foreach (unowned Unity.FilterOption opt in sources.options)
    {
      var master_id = SearchUtil.get_master_id_from_scope_id (opt.id);
      if (master_id != null && select_subscopes_of.contains (master_id))
      {
        if (!opt.active)
        {
          opt.active = true;
          changed = true;
          debug ("Changing source filter %s: %s", opt.id, opt.active.to_string ());
        }
      }
    }

    return changed;
  }

  internal bool update_filters_for_unselected (Unity.OptionsFilter? categories, Unity.OptionsFilter? sources, Gee.Set<string> unselected_scopes)
  {
    bool changed = false;

    var reg = MetaScopeRegistry.instance ();
    var unselect_masters = new HashTable<string, int> (str_hash, str_equal); // master scope id -> number of unselected scopes
    var unselect_subscopes_of = new Gee.TreeSet<string> (); // set of master scopes ids
   
    // find master scopes among unselected scopes
    foreach (var scope_id in unselected_scopes)
    {
      if (reg.is_master (scope_id))
        unselect_subscopes_of.add (scope_id);
    }
   
    // unselected master makes - all subscopes need to be unselected
    // unselected subscope: unselect master if it was the last subscope
 
    foreach (unowned Unity.FilterOption opt in sources.options)
    {
      var master_id = SearchUtil.get_master_id_from_scope_id (opt.id);
      if (master_id != null)
      {
        if (unselect_subscopes_of.contains (master_id))
        {
          if (opt.active)
          {
            opt.active = false;
            changed = true;
            debug ("Changing source filter %s: %s", opt.id, opt.active.to_string ());
          }
        }
        if (unselected_scopes.contains (opt.id) || opt.active == false)
        {
          if (unselect_masters.contains (master_id))
            unselect_masters[master_id] = unselect_masters[master_id] + 1;
          else
            unselect_masters[master_id] = 1;
        }
      }
      else
      {
        warning ("No master scope info for subscope %s", opt.id); // this shouldn't really happen
      }
    }

    // iterate over categories (masters), unselect masters if all its subscopes got unselected
    foreach (unowned Unity.FilterOption opt in categories.options)
    {
      if (unselect_masters.contains (opt.id))
      {
        var sub_scopes = reg.get_subscopes (opt.id);
        if (sub_scopes != null)
        {
          debug ("unselect_masters: id=%s, num=%d, subs=%u", opt.id, unselect_masters[opt.id], sub_scopes.size);
          if (unselect_masters[opt.id] == sub_scopes.size)
          {
            if (opt.active)
            {
              opt.active = false;
              changed = true;
              debug ("Changing category filter %s: %s", opt.id, opt.active.to_string ());
            }
          }
        }
        else
        {
          warning ("No subscopes for master scope %s", opt.id); // this shouldn't really happen
        }
      }
    }

    return changed;
  }

  public static bool set_filters (Unity.OptionsFilter categories, Unity.OptionsFilter sources, string[] enabled_scopes)
  {
    if (categories == null || sources == null)
    {
      warning ("Can't set filters, Categories or Sources filters missing");
      return false;
    }

    // convert enabled_scopes to set for faster lookups
    var scopes = new Gee.TreeSet<string> ();
    foreach (var scope in enabled_scopes)
    {
      scopes.add (scope);
    }

    bool changed = false;

    foreach (unowned Unity.FilterOption opt in categories.options)
    {
      bool value = scopes.contains (opt.id);
      if (opt.active != value)
      {
        opt.active = value;
        changed = true;
        debug ("Changing category filter %s: %s", opt.id, opt.active.to_string ());
      } 
    }

    foreach (unowned Unity.FilterOption opt in sources.options)
    {
      bool value = scopes.contains (opt.id);
      if (opt.active != value)
      {
        opt.active = value;
        changed = true;
        debug ("Changing sources filter %s: %s", opt.id, opt.active.to_string ());
      } 
    }
    
    return changed;
  }

  public static FilterSet create_filter_set (Unity.AggregatedScopeSearch scope_search)
  {
    var filters = new FilterSet ();

    var categories = scope_search.get_filter ("categories") as Unity.OptionsFilter;
    var sources = scope_search.get_filter ("sources") as Unity.OptionsFilter;

    filters.add (sources);
    filters.add (categories); 

    return filters;
  }
}

}