/* -*- mode: vala; c-basic-offset: 2; indent-tabs-mode: nil -*- */
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

namespace Unity.ApplicationsLens {

  private class RunningAppsSearch : Unity.ScopeSearchBase
  {
    private unowned WinStack windows;
    private unowned Dee.ICUTermFilter icu;
    private HashTable<string, CategoryList?> category_map;
    
    public RunningAppsSearch (WinStack windows, Dee.ICUTermFilter icu, HashTable<string, CategoryList?> category_map)
    {
      this.windows = windows;
      this.icu = icu;
      this.category_map = category_map;
    }

    /*
      Check if categories defined in a .desktop file match any of the active filter categories.
     */
    private bool category_matches (string[] desktop_file_categories, Gee.Set<string> selected_categories)
    {
      bool matches = false;

      foreach (var sel in selected_categories)
      {
        // selected categories are filter ids, remap them to real categories
        unowned CategoryList? cm = category_map.lookup (sel);
        if (cm == null)
          return false;

        foreach (var cat in cm.categories)
        {
          if (cat in desktop_file_categories)
          {
            matches = true;
            break;
          }
        }

        if (matches == false)
          return false;

        foreach (var cat in cm.exclude_categories)
        {
          if (cat in desktop_file_categories)
          {
            matches = false;
            break;
          }
        }
      }
      return matches;
    }

    /*
      Return ids of active filter options (categories corresponding to category_map keys)
    */
    private Gee.Set<string> get_selected_categories (Unity.OptionsFilter filter)
    {
      var cats = new Gee.HashSet<string> ();
      foreach (var opt in filter.options)
      {
        if (opt.active)
          cats.add (opt.id);
      }
      return cats;
    }

    public override void run_async (ScopeSearchBaseCallback async_callback)
    {
      run ();
      async_callback (this);
    }

    public override void run ()
    {
      // Don't add results to global searches
      if (search_context.search_type == SearchType.GLOBAL)
        return;

      var query = icu.apply (search_context.search_query.down ());
      var type_filter = search_context.filter_state.get_filter_by_id ("type") as OptionsFilter;
      bool has_filter = (type_filter != null && type_filter.filtering);
      var selected_categories = has_filter ? get_selected_categories (type_filter) : null;
      
      var appmanager = AppInfoManager.get_default();
      var it = windows.iterator ();

      while (it.next ())
      {
        var win = it.get ();
        debug ("Window: %s", win.app_id);
        try
        {
          bool matches = false;

          var app_id = win.app_id;
          if (!app_id.has_suffix (".desktop"))
            app_id += ".desktop";

          var app_info = appmanager.lookup (app_id);
          if (app_info == null || !(app_info is DesktopAppInfo))
          {
            warning ("No desktop file for %s", app_id);
            continue;
          }
          
          var desktop_file = app_info as DesktopAppInfo;
          
          if (selected_categories != null && !category_matches (desktop_file.get_categories ().split (";"), selected_categories))
            continue;

          if (search_context.search_query.strip () == "")
          {
            matches = true;
          }
          else
          {
            var display_name = icu.apply (desktop_file.get_display_name ().down ());
            if (display_name.contains (query))
            {
              matches = true;
            }
            else
            {
              var descr = icu.apply (desktop_file.get_description ().down ());
              if (descr.contains (query))
              {
                matches = true;
              }
              else
              {
                var exec_name = icu.apply (desktop_file.get_executable ().down ());
                if (exec_name.contains (query))
                  matches = true;
              }
            }
          }

          if (matches)
          {
            var result = Unity.ScopeResult ();
            result.uri = "application://" + win.app_id;
            result.icon_hint = "image://screenshot/" + win.app_id;
            result.category = 0;
            result.result_type = Unity.ResultType.PERSONAL;
            result.mimetype = "application/x-desktop";
            result.title = desktop_file.get_display_name ();
            result.comment = "";
            result.dnd_uri = result.uri;
            result.metadata = new HashTable<string, Variant> (str_hash, str_equal);
            search_context.result_set.add_result (result);
          }
        }
        catch (Error e)
        {
          warning ("Error loading desktop file: %s", win.app_id);
        }
      }
    }
  }
}
