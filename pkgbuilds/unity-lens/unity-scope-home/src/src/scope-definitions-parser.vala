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
 * Authored by Michal Hruby <michal.hruby@canonical.com>
 *
 */

namespace Unity.HomeScope {
  namespace DefinitionsParser
  {
    public static Filter? parse_filter_definition (Unity.Protocol.FilterDefinition definition)
    {
      Filter? filter = null;
      Icon? icon = null;

      FilterRenderer renderer = FilterRenderer.from_string (definition.filter_type);
      switch (renderer)
      {
        case FilterRenderer.CHECK_OPTIONS:
          filter = new CheckOptionFilter (definition.id, definition.name, icon);
          break;
        case FilterRenderer.CHECK_OPTIONS_COMPACT:
          filter = new CheckOptionFilterCompact (definition.id, definition.name, icon);
          break;
        case FilterRenderer.RADIO_OPTIONS:
          filter = new RadioOptionFilter (definition.id, definition.name, icon);
          break;
        case FilterRenderer.RATINGS:
          filter = new RatingsFilter (definition.id, definition.name, icon);
          break;
        case FilterRenderer.MULTIRANGE:
          filter = new MultiRangeFilter (definition.id, definition.name, icon);
          break;
        default:
          break;
      }

      var options_filter = filter as OptionsFilter;
      if (options_filter != null && definition.sort_type != null)
      {
        switch (definition.sort_type)
        {
          case "display-name":
            options_filter.sort_type = Unity.OptionsFilter.SortType.DISPLAY_NAME;
            break;
          default:
            break;
        }
      }
      unowned string[] option_ids = definition.get_option_ids ();
      unowned string[] option_names = definition.get_option_names ();
      for (int i = 0; i < option_ids.length; i++)
      {
        options_filter.add_option (option_ids[i], option_names[i]);
      }

      return filter;
    }

    public static Category? parse_category_definition (Unity.Protocol.CategoryDefinition definition, Icon? default_icon = null)
    {
      Icon? icon = null;
      if (definition.icon != null && definition.icon[0] != '\0')
      {
        try
        {
          icon = Icon.new_for_string (definition.icon);
        }
        catch (Error err)
        {
          icon = null;
        }
      }
      if (icon == null) icon = default_icon;

      Category? category = null;
      CategoryRenderer renderer = CategoryRenderer.DEFAULT;
      if (definition.renderer != null)
      {
        renderer = CategoryRenderer.from_string (definition.renderer);
      }
      category = new Category (definition.id, definition.name, icon, renderer);
      if (definition.content_type != null)
      {
        category.content_type = CategoryContentType.from_string (definition.content_type);
      }
      if (definition.renderer_hint != null)
      {
        category.renderer_hint = definition.renderer_hint;
      }
      return category;
    }

    public static void apply_category_constraints (Unity.AggregatorScope scope,
                                                   Unity.Protocol.CategoryDefinition[] definitions)
    {
      var cat_ids = new HashTable<string, int> (str_hash, str_equal);
      int i = 0;
      foreach (var category in scope.categories.get_categories ())
      {
        cat_ids[category.id] = i++;
      }

      foreach (var definition in definitions)
      {
        if (!cat_ids.contains (definition.id)) continue;
        int cat_idx = cat_ids[definition.id];
        if (definition.sort_field != null && definition.sort_field[0] != '\0')
        {
          // this supports SortField=sort_field1;sort_field2::asc;
          foreach (var component in definition.sort_field.split (";"))
          {
            if (component == "") continue;
            var order = Unity.AggregatorScope.SortFlags.ASCENDING;
            var field_name = component;
            if ("::" in component)
            {
              var sort_props = component.split ("::", 2);
              field_name = sort_props[0];
              if (sort_props[1].down ().has_prefix ("des"))
                order = Unity.AggregatorScope.SortFlags.DESCENDING;
            }

            scope.add_sorter (cat_idx, field_name, order);
          }
        }
        if (definition.dedup_field != null && definition.dedup_field[0] != '\0' && cat_ids.contains (definition.dedup_field))
        {
          scope.add_constraint (cat_idx, definition.dedup_field);
        }
      }
    }
  }
}
