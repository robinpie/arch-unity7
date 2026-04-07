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
 * Authored by James Henstridge <james.henstridge@canonical.com>
 *
 */

namespace Unity.ApplicationsLens {
  const string GENERIC_SCOPE_ICON = ICON_PATH + "service-generic.svg";
  const string[] invisible_scope_ids =
  {
    "home.scope",
    "applications-scopes.scope",
  };

  private class ScopesScope : Unity.AbstractScope
  {
    public ApplicationsScope appscope;
    public Unity.Package.Searcher? searcher;
    public HashTable<unowned string, unowned string> disabled_scope_ids;
    public Dee.Model remote_scopes_model;
    public Dee.Index scopes_index;
    public Dee.Analyzer analyzer;

    public HashTable<string, bool> locked_scope_ids;

    public ScopesScope (ApplicationsScope appscope)
    {
      this.appscope = appscope;
      // Track the remote scopes
      var shared_model = new Dee.SharedModel ("com.canonical.Unity.SmartScopes.RemoteScopesModel");
      shared_model.set_schema ("s", "s", "s", "s", "s", "as");
      shared_model.end_transaction.connect (this.remote_scopes_changed);
      remote_scopes_model = shared_model;

      scopes_index = Utils.prepare_index (remote_scopes_model,
                                          RemoteScopesColumn.NAME,
                                          (model, iter) =>
      {
        unowned string name = model.get_string (iter, RemoteScopesColumn.NAME);
        return "%s\n%s".printf (_("scope"), name);
      }, out analyzer);

      disabled_scope_ids = new HashTable<unowned string, unowned string> (str_hash, str_equal);
      update_disabled_scopes_hash ();
      var pref_man = PreferencesManager.get_default ();
      pref_man.notify["disabled-scopes"].connect (update_disabled_scopes_hash);

      locked_scope_ids = new HashTable<string, bool> (str_hash, str_equal);
      var settings = new Settings (LIBUNITY_SCHEMA);
      foreach (unowned string scope_id in settings.get_strv ("locked-scopes"))
      {
        locked_scope_ids[scope_id] = true;
      }

      build_scope_index.begin ();
    }

    private void remote_scopes_changed (uint64 begin_seqnum, uint64 end_seqnum)
    {
      this.results_invalidated (SearchType.DEFAULT);
    }

    private void update_disabled_scopes_hash ()
    {
      disabled_scope_ids.remove_all ();
      var pref_man = PreferencesManager.get_default ();
      foreach (unowned string scope_id in pref_man.disabled_scopes)
      {
        // using HashTable as a set (optimized in glib when done like this)
        disabled_scope_ids[scope_id] = scope_id;
      }
    }

    private void disable_scope (string scope_id)
    {
      if (scope_id in disabled_scope_ids) return;

      var pref_man = PreferencesManager.get_default ();
      var disabled_scopes = pref_man.disabled_scopes;
      disabled_scopes += scope_id;

      var settings = new Settings (LIBUNITY_SCHEMA);
      settings.set_strv ("disabled-scopes", disabled_scopes);
    }

    private void enable_scope (string scope_id)
    {
      if (!(scope_id in disabled_scope_ids)) return;

      var pref_man = PreferencesManager.get_default ();
      string[] disabled_scopes = {};
      foreach (unowned string disabled_scope_id in pref_man.disabled_scopes)
      {
        if (disabled_scope_id != scope_id)
          disabled_scopes += disabled_scope_id;
      }

      var settings = new Settings (LIBUNITY_SCHEMA);
      settings.set_strv ("disabled-scopes", disabled_scopes);
    }

    private async void build_scope_index ()
    {
      try
      {
        var scope_registry = yield Unity.Protocol.ScopeRegistry.find_scopes (
          Config.PKGDATADIR + "/scopes");
        searcher = new Unity.Package.Searcher.for_scopes (scope_registry);
      }
      catch (Error err)
      {
        warning ("Unable to find scopes: %s", err.message);
      }
    }

    public override string get_group_name ()
    {
      return "com.canonical.Unity.Scope.Applications";
    }

    public override string get_unique_name ()
    {
      return "/com/canonical/unity/scope/scopes";
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
        filter.add_option ("scopes", _("Dash plugins"));
        filter.add_option ("system", _("System"));

        filters.add (filter);
      }
      return filters;
    }

    public override Unity.CategorySet get_categories ()
    {
      Unity.CategorySet categories = new Unity.CategorySet ();
      File icon_dir = File.new_for_path (ICON_PATH);

      var cat = new Unity.Category ("scopes", _("Dash plugins"),
                                    new FileIcon (icon_dir.get_child ("group-installed.svg")));
      categories.add (cat);

      return categories;
    }

    public override Unity.Schema get_schema ()
    {
      var schema = new Unity.Schema ();
      schema.add_field ("scope_disabled", "u", Schema.FieldType.OPTIONAL);
      return schema;
    }

    public override string normalize_search_query (string search_query)
    {
      return search_query.strip ();
    }

    public override Unity.ScopeSearchBase create_search_for_query (Unity.SearchContext search_context)
    {
      return new ScopesSearch (this, search_context);
    }

    public override Unity.ResultPreviewer create_previewer (Unity.ScopeResult result, Unity.SearchMetadata metadata)
    {
      return new ScopesResultPreviewer (this, result, metadata);
    }

    public override Unity.ActivationResponse? activate (Unity.ScopeResult result, Unity.SearchMetadata metadata, string? action_id)
    {
      // uris are "scope://scope_id.scope"
      var scope_id = result.uri.substring (8);
      if (action_id == "enable-scope")
      {
        enable_scope (scope_id);
      }
      else if (action_id == "disable-scope")
      {
        disable_scope (scope_id);
      }
      /* Make a preview */
      var preview = create_previewer (result, metadata).run ();
      if (preview is Unity.Preview)
      {
        return new Unity.ActivationResponse.with_preview (preview as Unity.Preview);
      }
      else
      {
        warning ("Failed to generate preview for %s", result.uri);
        return new Unity.ActivationResponse (Unity.HandledType.NOT_HANDLED);
      }
    }

    public static Icon? get_default_icon ()
    {
      try
      {
        return Icon.new_for_string (GENERIC_SCOPE_ICON);
      }
      catch (Error err)
      {
        warning ("%s", err.message);
      }
      return null;
    }
  }

  private class ScopesSearch : Unity.ScopeSearchBase
  {
    private ScopesScope scope;

    public ScopesSearch (ScopesScope scope, Unity.SearchContext search_context)
    {
      this.scope = scope;
      this.search_context = search_context;
    }

    public override void run ()
    {
      if (this.scope.searcher == null)
        return;

      var context = this.search_context;
      // Don't add results to global searches
      if (context.search_type == SearchType.GLOBAL)
        return;

      var filter = context.filter_state.get_filter_by_id ("type") as OptionsFilter;
      if (filter != null && filter.filtering &&
          !filter.get_option ("scopes").active)
        return;

      add_local_results (context.search_query, context.result_set);
      add_remote_results (context.search_query, context.result_set);
    }

    public override void run_async (ScopeSearchBaseCallback callback)
    {
      // just run the matching in the origin thread, it's cheap
      run ();
      callback (this);
    }

    private string get_scope_icon (string icon_hint, bool is_disabled)
    {
      try
      {
        Icon base_icon = icon_hint == null || icon_hint == "" ?
          ScopesScope.get_default_icon () : Icon.new_for_string (icon_hint);
        var anno_icon = new AnnotatedIcon (base_icon);
        anno_icon.size_hint = IconSizeHint.SMALL;
        // dim disabled icons by decreasing their alpha
        if (is_disabled)
          anno_icon.set_colorize_rgba (1.0, 1.0, 1.0, 0.5);
        return anno_icon.to_string ();
      }
      catch (Error err)
      {
        return "";
      }
    }

    private void add_local_results (string search_query, Unity.ResultSet result_set)
    {
      bool has_search = !Utils.is_search_empty (search_query);
      var pkg_search_string = XapianUtils.prepare_pkg_search_string (
        search_query, null);

      var search_result = this.scope.searcher.search (
        pkg_search_string, 0, Unity.Package.SearchType.PREFIX,
        has_search ?
          Unity.Package.Sort.BY_RELEVANCY : Unity.Package.Sort.BY_NAME);

      foreach (unowned Unity.Package.PackageInfo info in search_result.results)
      {
        /* Don't include master scopes in the search results.  This is
         * performed after deduping so the master scopes don't just
         * move to the "available" category. */
        if (info.is_master_scope)
          continue;

        if (info.desktop_file in invisible_scope_ids)
          continue;

        bool is_disabled = info.desktop_file in scope.disabled_scope_ids;
        var result = Unity.ScopeResult ();
        result.uri = @"scope://$(info.desktop_file)";
        result.category = 0;
        result.icon_hint = get_scope_icon (info.icon, is_disabled);
        result.result_type = Unity.ResultType.DEFAULT;
        result.mimetype = "application/x-unity-scope";
        result.title = info.application_name;
        if (result.title == null)
          result.title = "";
        result.comment = "";
        result.dnd_uri = "";
        result.metadata = new HashTable<string, Variant> (str_hash, str_equal);
        result.metadata["scope_disabled"] = new Variant.uint32 (is_disabled ? 1 : 0);

        result_set.add_result (result);
      }
    }

    private void add_remote_results (string search_query, Unity.ResultSet result_set)
    {
      var results = Utils.search_index (
        scope.scopes_index, scope.analyzer, search_query);
      foreach (var iter in results)
      {
        unowned string scope_id =
          scope.remote_scopes_model.get_string (iter, RemoteScopesColumn.SCOPE_ID);
        unowned string icon_hint =
          scope.remote_scopes_model.get_string (iter, RemoteScopesColumn.ICON_HINT);
        bool is_disabled = scope_id in scope.disabled_scope_ids;

        var result = Unity.ScopeResult ();
        result.uri = @"scope://$(scope_id)";
        result.category = 0;
        result.icon_hint = get_scope_icon (icon_hint, is_disabled);
        result.result_type = Unity.ResultType.DEFAULT;
        result.mimetype = "application/x-unity-scope";
        result.title =
          scope.remote_scopes_model.get_string (iter, RemoteScopesColumn.NAME);
        result.comment = "";
        result.dnd_uri = "";
        result.metadata = new HashTable<string, Variant> (str_hash, str_equal);
        result.metadata["scope_disabled"] = new Variant.uint32 (is_disabled ? 1 : 0);

        result_set.add_result (result);
      }
    }
  }

  private class ScopesResultPreviewer : Unity.ResultPreviewer
  {
    private ScopesScope scope;

    public ScopesResultPreviewer (ScopesScope scope, Unity.ScopeResult result, Unity.SearchMetadata metadata)
    {
      this.scope = scope;
      set_scope_result (result);
      set_search_metadata (metadata);
    }

    public override Unity.AbstractPreview? run ()
    {
      var ml = new MainLoop ();
      Unity.AbstractPreview? preview = null;
      run_async ((obj, p) =>
        {
          preview = p;
          ml.quit ();
        });
      ml.run ();
      return preview;
    }

    public override void run_async (Unity.AbstractPreviewCallback callback)
    {
      make_preview.begin ((obj, res) =>
      {
        var preview = make_preview.end (res);
        callback (this, preview);
      });
    }

    private async Unity.AbstractPreview? make_preview ()
    {
      Unity.ApplicationPreview? preview = null;
      // uris are "scope://scope_id.scope"
      var scope_id = result.uri.substring (8);
      bool scope_disabled = scope_id in scope.disabled_scope_ids;

      // figure out if the scope is remote
      bool is_remote_scope = false;
      var info = scope.searcher.get_by_desktop_file (scope_id);

      Dee.ModelIter found_iter = null;
      if (info == null)
      {
        var iter = scope.remote_scopes_model.get_first_iter ();
        var end_iter = scope.remote_scopes_model.get_last_iter ();
        while (iter != end_iter)
        {
          if (scope.remote_scopes_model.get_string (iter, 0) == scope_id)
          {
            is_remote_scope = true;
            found_iter = iter;
            break;
          }
          iter = scope.remote_scopes_model.next (iter);
        }
      }

      if (is_remote_scope)
      {
        var name = scope.remote_scopes_model.get_string (found_iter, 1);
        if (name == null || name == "")
          name = scope.remote_scopes_model.get_string (found_iter, 0);
        var description = scope.remote_scopes_model.get_string (found_iter, 2);
        if (description != null)
          description = Markup.escape_text(description);
        Icon? icon = null;
        Icon? screenshot = null;
        var icon_hint = scope.remote_scopes_model.get_string (found_iter, 3);
        var screenshot_url = scope.remote_scopes_model.get_string (found_iter, 4);
        try
        {
          if (icon_hint != "") icon = Icon.new_for_string (icon_hint);
          if (screenshot_url != "") screenshot = Icon.new_for_string (screenshot_url);
        }
        catch (Error err)
        {
          warning ("%s", err.message);
        }

        if (icon == null) icon = ScopesScope.get_default_icon ();

        preview = new Unity.ApplicationPreview (name,
                                                "",
                                                description,
                                                icon,
                                                screenshot);
      }
      else if (info != null)
      {
        var name = info.application_name;
        if (name == null || name == "")
          name = info.desktop_file;
        var subtitle = "";
        var description = info.description;
        if (description != null)
          description = Markup.escape_text(description);
        Icon? icon = null;
        Icon? screenshot = null;
        try
        {
          if (info.icon != null && info.icon != "")
            icon = Icon.new_for_string (info.icon);
        }
        catch (Error err)
        {
          warning ("%s", err.message);
        }
        if (icon == null)
          icon = ScopesScope.get_default_icon ();

        string version, screenshot_uri;
        if (yield scope.appscope.get_version_and_screenshot (
              scope_id, out version, out screenshot_uri))
        {
          if (version != "")
            subtitle = _("Version %s").printf (version);
          if (screenshot_uri != null)
          {
            File scr_file = File.new_for_uri (screenshot_uri);
            screenshot = new FileIcon (scr_file);
          }
        }

        preview = new Unity.ApplicationPreview (name,
                                                subtitle,
                                                description,
                                                icon,
                                                screenshot);
      }
      if (preview != null && !(scope_id in invisible_scope_ids)
        && !(scope_id in scope.locked_scope_ids))
      {
        PreviewAction action;
        preview.set_rating(-1.0f, 0);
        if (scope_disabled)
        {
          action = new Unity.PreviewAction ("enable-scope", _("Enable"), null);
        }
        else
        {
          action = new Unity.PreviewAction ("disable-scope", _("Disable"), null);
        }
        preview.add_action (action);
      }
      return preview;
    }
  }
}
