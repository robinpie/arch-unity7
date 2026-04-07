/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Authored by Alex Launi <alex.launi@canonical.com>
 *
 */

using GLib;
using Config;

namespace Unity.MusicLens
{
  public abstract class SimpleScope : Object
  {
    public Unity.DeprecatedScope scope { get; protected set; }

    protected abstract int num_results_without_search { get; }
    protected abstract int num_results_global_search { get; }
    protected abstract int num_results_lens_search { get; }

    protected abstract async void perform_search (DeprecatedScopeSearch search,
                                                  SearchType search_type,
                                                  owned List<FilterParser> filters,
                                                  int max_results = -1,
                                                  GLib.Cancellable? cancellable = null);

    protected SimpleScope ()
    {
    }

    protected void initialize ()
    {
      populate_filters ();
      populate_categories ();

      scope.active_sources_changed.connect (() => {
        scope.queue_search_changed (SearchType.DEFAULT);
      });

      /* No need to search if only the whitespace changes */
      scope.generate_search_key.connect ((search) => {
        return search.search_string.strip ();
      });

      /* Listen for changes to the lens search entry */
      scope.search_changed.connect ((search, search_type, cancellable) => {
        update_search_async.begin (search, search_type, cancellable);
      });
    }


    private void populate_filters ()
    {
      var filters = new Unity.FilterSet ();

      /* Decade filter */
      {
        var filter = new MultiRangeFilter ("decade", _("Decade"));

        filter.add_option ("0", _("Old"));
        filter.add_option ("1960", _("60s"));
        filter.add_option ("1970", _("70s"));
        filter.add_option ("1980", _("80s"));
        filter.add_option ("1990", _("90s"));
        filter.add_option ("2000", _("00s"));
        filter.add_option ("2010", _("10s"));

        filters.add (filter);
      }

      /* Genre filter */
      {
        var filter = new CheckOptionFilterCompact ("genre", _("Genre"));
        filter.sort_type = OptionsFilter.SortType.DISPLAY_NAME;

        filter.add_option (Genre.BLUES_ID, _("Blues"));
        filter.add_option (Genre.CLASSICAL_ID, _("Classical"));
        filter.add_option (Genre.COUNTRY_ID, _("Country"));
        filter.add_option (Genre.DISCO_ID, _("Disco"));
        filter.add_option (Genre.FUNK_ID, _("Funk"));
        filter.add_option (Genre.ROCK_ID, _("Rock"));
        filter.add_option (Genre.METAL_ID, _("Metal"));
        filter.add_option (Genre.HIPHOP_ID, _("Hip-hop"));
        filter.add_option (Genre.HOUSE_ID, _("House"));
        filter.add_option (Genre.NEWWAVE_ID, _("New-wave"));
        filter.add_option (Genre.RANDB_ID, _("R&B"));
        filter.add_option (Genre.PUNK_ID, _("Punk"));
        filter.add_option (Genre.JAZZ_ID, _("Jazz"));
        filter.add_option (Genre.POP_ID, _("Pop"));
        filter.add_option (Genre.REGGAE_ID, _("Reggae"));
        filter.add_option (Genre.SOUL_ID, _("Soul"));
        filter.add_option (Genre.TECHNO_ID, _("Techno"));
        filter.add_option (Genre.OTHER_ID, _("Other"));

        filters.add (filter);
      }

      scope.filters = filters;
    }

    // FIXME icons
    private void populate_categories ()
    {
      /* Offsets of categories must match up with the Category enum */
      var categories = new Unity.CategorySet ();
      var icon_dir = File.new_for_path (Config.ICON_PATH);

      var cat = new Unity.Category ("global", _("Music"),
                                    new FileIcon (icon_dir.get_child ("group-songs.svg")));
      categories.add (cat);

      cat = new Unity.Category ("songs", _("Songs"),
                                new FileIcon (icon_dir.get_child ("group-songs.svg")));
      categories.add (cat);

      cat =  new Unity.Category ("albums", _("Albums"),
                                 new FileIcon (icon_dir.get_child ("group-albums.svg")));
      categories.add (cat);

      cat = new Unity.Category ("more", _("More suggestions"),
                                new FileIcon (icon_dir.get_child ("group-treat-yourself.svg")),
                                Unity.CategoryRenderer.CAROUSEL);
      categories.add (cat);

      cat = new Unity.Category ("radio", _("Radio"),
                                new FileIcon (icon_dir.get_child ("group-songs.svg")));
      categories.add (cat);

      scope.categories = categories;
    }

    private async void update_search_async (DeprecatedScopeSearch search,
                                            SearchType search_type,
                                            GLib.Cancellable cancellable)
    {
      int max_results;
      /*
       * results for a global search from just hitting Super.
       * Here we want to return a smaller number of results with 0 filters.
       */
      if (search_type == SearchType.GLOBAL)
        max_results = num_results_global_search;
      else if (is_search_empty (search))
        max_results = num_results_without_search;
      else
        max_results = num_results_lens_search;

      yield prepare_search (search, search_type, max_results, cancellable);
      search.finished ();
    }

    private async void prepare_search (DeprecatedScopeSearch search,
                                       SearchType search_type,
                                       int max_results,
                                       GLib.Cancellable cancellable)
    {
      var results_model = search.results_model;

      List<FilterParser> filters = new List<FilterParser> ();
      // don't apply filters to global searches
      if (search_type != SearchType.GLOBAL)
      {
        Filter filter = search.get_filter ("genre");
        if (filter.filtering)
          filters.append (new GenreFilterParser (filter as CheckOptionFilterCompact));

        filter = search.get_filter ("decade");
        if (filter.filtering)
          filters.append (new DecadeFilterParser (filter as MultiRangeFilter));
      }

      results_model.clear ();

      // don't perform search is all sources are inactive
      if (scope.sources.options.length () > 0 && scope.sources.filtering)
      {
        bool any_active = false;
        foreach (var source in scope.sources.options)
        {
          if (source.active) any_active = true;
        }
        if (!any_active)
        {
          return;
        }
      }

      yield perform_search (search, search_type, (owned) filters, max_results, cancellable);

      if (results_model.get_n_rows () == 0)
      {
        search.set_reply_hint ("no-results-hint",
          _("Sorry, there is no music that matches your search."));
      }
    }

    protected bool is_search_empty (DeprecatedScopeSearch search)
      requires (search.search_string != null)
    {
      return search.search_string.strip () == "";
    }
  }
}
