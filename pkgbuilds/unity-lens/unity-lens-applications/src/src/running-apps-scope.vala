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

public int unity_scope_module_get_version ()
{
  return Unity.SCOPE_API_VERSION;
}

public List<Unity.AbstractScope> unity_scope_module_load_scopes () throws Error
{
  /* Sort up locale to get translations but also sorting and
   * punctuation right */
  GLib.Intl.bindtextdomain (Config.PACKAGE, Config.LOCALEDIR);
  GLib.Intl.bind_textdomain_codeset (Config.PACKAGE, "UTF-8");
  GLib.Intl.setlocale(GLib.LocaleCategory.ALL, "");

  var scopes = new List<Unity.AbstractScope> ();
  scopes.append(new Unity.ApplicationsLens.RunningAppsScope ());
  return scopes;
}

namespace Unity.ApplicationsLens {
  
  const string ICON_PATH = Config.DATADIR + "/icons/unity-icon-theme/places/svg/";

  internal struct CategoryList
  {
    Gee.Set<string> categories;
    Gee.Set<string> exclude_categories;
    
    public CategoryList (string[] cats, string[] exclude)
    {
      categories = new Gee.HashSet<string> ();
      exclude_categories = new Gee.HashSet<string> ();

      foreach (var cat in cats)
      {
        categories.add (cat);
      }
      
      foreach (var cat in exclude)
      {
        exclude_categories.add (cat);
      }
    }
  }

  public class RunningAppsScope : Unity.AbstractScope
  {
    internal HashTable<string, CategoryList?> category_map = new HashTable<string, CategoryList?> (str_hash, str_equal);
    private WindowStackProxy? winproxy = null;
    internal WinStack windows = new WinStack ();
    internal Dee.ICUTermFilter icu = new Dee.ICUTermFilter.ascii_folder ();

    public RunningAppsScope ()
    {
      init_proxy ();
      windows.updated.connect (on_winstack_changed);
      populate_category_map ();
    }

    public override string get_group_name ()
    {
      return "com.canonical.Unity.Scope.RunningApps";
    }

    public override string get_unique_name ()
    {
      return "/com/canonical/unity/scope/runningapps";
    }

    public override Unity.FilterSet get_filters ()
    {
      var filters = new Unity.FilterSet ();

      /* Type filter */
      {
        var filter = new CheckOptionFilter ("type", _("Type"));
        filter.sort_type = Unity.OptionsFilter.SortType.DISPLAY_NAME;

        filter.add_option ("accessories", _("Accessories"));
        filter.add_option ("education", _("Education"));
        filter.add_option ("game", _("Games"));
        filter.add_option ("graphics", _("Graphics"));
        filter.add_option ("internet", _("Internet"));
        filter.add_option ("fonts", _("Fonts"));
        filter.add_option ("office", _("Office"));
        filter.add_option ("media", _("Media"));
        filter.add_option ("customization", _("Customization"));
        filter.add_option ("accessibility", _("Accessibility"));
        filter.add_option ("developer", _("Developer"));
        filter.add_option ("science-and-engineering", _("Science & engineering"));
        filter.add_option ("scopes", _("Search plugins"));
        filter.add_option ("system", _("System"));

        filters.add (filter);
      }

      return filters;
    }

    public override Unity.CategorySet get_categories ()
    {
      File icon_dir = File.new_for_path (ICON_PATH);
      Unity.CategorySet categories = new Unity.CategorySet ();
      var cat = new Unity.Category ("recent", _("Recent apps"),
                                new FileIcon (icon_dir.get_child ("group-apps.svg")));
      categories.add (cat);
      return categories;
    }

    public override Unity.Schema get_schema ()
    {
      var schema = new Unity.Schema ();
      return schema;
    }

    public override string normalize_search_query (string search_query)
    {
      return search_query.strip ();
    }

    public override Unity.ScopeSearchBase create_search_for_query (Unity.SearchContext search_context)
    {
      var search = new RunningAppsSearch (windows, icu, category_map);
      search.set_search_context (search_context);
      return search;
    }

    public override Unity.ResultPreviewer create_previewer (Unity.ScopeResult result, Unity.SearchMetadata metadata)
    {
      return null;
    }

    public override Unity.ActivationResponse? activate (Unity.ScopeResult result, Unity.SearchMetadata metadata, string? action_id)
    {
      return new Unity.ActivationResponse (Unity.HandledType.NOT_HANDLED);
    }

    internal void init_proxy ()
    {
      debug ("Initializing winstack proxy");
      try
      {
        winproxy = WindowStackProxy.get_proxy ();
      
        // request current windows stack
        var winstack = winproxy.get_window_stack_sync ();
        winproxy.window_created.connect (windows.on_window_created);
        winproxy.window_destroyed.connect (windows.on_window_destroyed);
        winproxy.focused_window_changed.connect (windows.on_focused_window_changed);
        windows.from_win_stack (winstack);
      }
      catch (Error e)
      {
        warning ("Failed to create WindowStackProxy: %s", e.message);
      }
    }

    private void populate_category_map ()
    {
      category_map["accessories"] = CategoryList ({"Utility"}, {"Accessibility"});
      category_map["education"] = CategoryList ({"Education"}, {"Science"});
      category_map["game"] = CategoryList ({"Game"}, {});
      category_map["graphics"] = CategoryList ({"Graphics"}, {});
      category_map["internet"] = CategoryList ({"Network"}, {});
      category_map["fonts"] = CategoryList ({"Fonts"}, {});
      category_map["office"] = CategoryList ({"Office"}, {});
      category_map["media"] = CategoryList ({"AudioVideo"}, {});
      category_map["customization"] = CategoryList ({"Settings"}, {});
      category_map["accessibility"] = CategoryList ({"Accessibility"}, {"Settings"});
      category_map["developer"] = CategoryList ({"Development"}, {});
      category_map["science-and-engineering"] =  CategoryList ({"Science", "Engineering"}, {});
      category_map["system"] = CategoryList ({"System", "Security"}, {});
    }

    private void on_winstack_changed ()
    {
      results_invalidated (Unity.SearchType.DEFAULT);
    }
  }
}