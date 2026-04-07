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

namespace Unity.HomeScope
{
  /**
    * Maintains master scopes instances.
    */
  public class MasterScopesManager
  {
    private List<Unity.DeprecatedScopeBase?> master_scopes = new List<Unity.DeprecatedScopeBase?> ();

    public void start ()
    {
      foreach (var node in ScopeRegistry.instance ().scopes)
      {
        var info = node.scope_info;
        // if NoExport is defined it's considered an external master scope
        if (info.is_master && !info.no_export)
        {
          Icon? main_icon = null;
          if (info.category_icon != null && info.category_icon.length > 0)
          {
            var icon_file = File.new_for_path (info.category_icon);
            main_icon = new GLib.FileIcon (icon_file);
          }
          else
          {
            main_icon = new GLib.ThemedIcon ("generic"); //fallback
          }

          var category_set = new Unity.CategorySet ();
          // all masters need to have 'global' category because of category merging in home scope
          bool defines_global_category = false;
          foreach (var cat_def in info.get_categories ())
          {
            if (cat_def.id == "global")
            {
              defines_global_category = true;
              break;
            }
          }

          if (!defines_global_category)
          {
            var global_cat = new Unity.Category ("global", info.name, main_icon);
            category_set.add (global_cat);
          }

          foreach (var cat_def in info.get_categories ())
          {
            var category = DefinitionsParser.parse_category_definition (cat_def, main_icon);
            if (category == null) continue;
            category_set.add (category);
          }

          var filters = new FilterSet ();
          foreach (var filter_def in info.get_filters ())
          {
            var filter = DefinitionsParser.parse_filter_definition (filter_def);
            if (filter == null) continue;
            filters.add (filter);
          }

          var scope = new Unity.MasterScope (info.dbus_path, info.id);
          scope.search_hint = info.search_hint;
          scope.categories = category_set;
          scope.filters = filters;

          var schema = new Unity.Schema ();

          if (info.required_metadata != null)
          {
            var metadata_arr = info.required_metadata.columns;
            for (int i = 0; i < metadata_arr.length; i++)
            {
              var column = metadata_arr[i];
              schema.add_field (column.name, column.type_id, Unity.Schema.FieldType.REQUIRED);
            }
          }
          if (info.optional_metadata != null)
          {
            var metadata_arr = info.optional_metadata.columns;
            for (int i = 0; i < metadata_arr.length; i++)
            {
              var column = metadata_arr[i];
              schema.add_field (column.name, column.type_id, Unity.Schema.FieldType.OPTIONAL);
            }
          }
          scope.schema = schema;

          DefinitionsParser.apply_category_constraints (scope, info.get_categories ());

          if (info.no_content_hint != null)
          {
            scope.no_content_hint = info.no_content_hint;
          }

          master_scopes.append (scope);

          debug ("Start master scope: %s", info.id);

          try
          {
            scope.export ();
          }
          catch (Error e)
          {
            warning ("Failed to export master scope %s: %s", node.scope_info.id, e.message);
          }
        }
      }
    }
  }
}
