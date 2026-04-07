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

  /**
   * Keeps the list of all categories displayed in Home Lens (i.e. list of master scopes ids).
   * Keeps results counts for all of them (personal / semi-personal / default results).
   */
  public class CategoryManager: GLib.Object
  {
    public static const string APP_SCOPE_ID = "applications.scope";
    public static const string MORE_SUGGESTIONS_SCOPE_ID = "more_suggestions.scope";

    public static const int RESULT_CATEGORY_COLUMN = 2;
    public static const int RESULT_TYPE_COLUMN = 3; //result-type column in the model
    public static const int RESULT_NAME_COLUMN = 5; //title column in the results model
    public static const int RESULT_COMMENT_COLUMN = 6; //comment column in the results model
    public static const int RESULT_METADATA_COLUMN = 8;
    public static const int RESULT_TYPE_MAX = 2;  //warning! keep in sync with ResultType enum

    private Dee.TextAnalyzer analyzer = new Dee.TextAnalyzer ();
    private Dee.ICUTermFilter icu_filter = new Dee.ICUTermFilter.ascii_folder ();

    private struct SessionKey
    {
      string home_channel_id;
      string scope_id;

      public static uint hash (SessionKey? k)
      {
        if (k != null)
          return str_hash ("%s:%s".printf (k.home_channel_id, k.scope_id));
        return str_hash ("");
      }

      public static bool equal (SessionKey? k1, SessionKey? k2)
      {
        if (k1 != null && k2 != null)
          return k1.home_channel_id == k2.home_channel_id && k1.scope_id == k2.scope_id;
        return k1 == k2;
      }
    }

    //map result counts for each category, per session established by (home_channel_id+scope_id)
    private HashTable<SessionKey?, CategoryData?> category_counts = new HashTable<SessionKey?, CategoryData?> (SessionKey.hash, SessionKey.equal);

    //map scope id to its default index; this is static data that doesn't depend on session context
    private HashTable<string, int> catidx = new HashTable<string, int> (str_hash, str_equal);

    //all scope ids in the order they were registered on startup, provides reverse mapping to catidx
    private Gee.ArrayList<string> categories = new Gee.ArrayList<string> ();
    private int cat_cnt = 0;

    // maps home results model to home channel id, needed by row_removed handler.
    private HashTable<unowned Object, string> home_models = new HashTable<unowned Object, string> (direct_hash, direct_equal);

    private HashTable<string, int> dconf_order = new HashTable<string, int> (str_hash, str_equal);

    internal class CategoryData
    {
      public string scope_id { get; set; }
      public int num_default_results
      {
        get
        {
          return results[Unity.ResultType.DEFAULT];
        }
      }

      public int num_semi_personal_results
      {
        get
        {
          return results[Unity.ResultType.SEMI_PERSONAL];
        }
      }

      public int num_personal_results
      {
        get
        {
          return results[Unity.ResultType.PERSONAL];
        }
      }

      public int num_total_results
      {
        get
        {
          return num_default_results + num_personal_results + num_semi_personal_results;
        }
      }

      public int recommended_order { get; set; default = -1; }
      public int dconf_order { get; set; default = -1; }

      internal int results[3]; //FIXME remove magic

      public CategoryData (string scope_id)
      {
        this.scope_id = scope_id;
        results = {0};
      }
    }

    private static CategoryManager mgr;

    private CategoryManager ()
    {
      analyzer.add_term_filter ((terms_in, terms_out) =>
      {
        for (uint i = 0; i < terms_in.num_terms (); i++)
          terms_out.add_term (icu_filter.apply (terms_in.get_term (i)));
      });
    }

    public static CategoryManager instance ()
    {
      if (mgr == null)
      {
        mgr = new CategoryManager ();
      }
      return mgr;
    }

    public void set_default_sort_order (Gee.ArrayList<string> scope_ids)
    {
      int cnt = 0;
      dconf_order.remove_all ();
      foreach (var id in scope_ids)
        dconf_order[id] = cnt++;
    }

    public void set_default_sort_order_from_dconf ()
    {
      PreferencesManager preferences = PreferencesManager.get_default ();

      var scopes = new Gee.ArrayList<string> ();
      foreach (var scope_id in preferences.home_lens_priority)
        scopes.add (scope_id);
      set_default_sort_order (scopes);
    }

    public void clear ()
    {
      category_counts.remove_all ();
      catidx.remove_all ();
      categories.clear ();
      cat_cnt = 0;
      home_models.remove_all ();
      dconf_order.remove_all ();
    }

    public void register (string scope_id)
    {
      debug ("Registering category for %s with index %u", scope_id, cat_cnt);
      categories.add (scope_id);
      catidx[scope_id] = cat_cnt++;
    }

    internal HashTable<string, int> create_recommended_order_map (List<SmartScopes.RecommendedScope?> recommendations)
    {
      int cnt = 0;
      var recommended_order = new HashTable<string, int> (str_hash, str_equal);
      foreach (var rec in recommendations)
      {
        var master_id = SearchUtil.get_master_id_from_scope_id (rec.scope_id);
        if (master_id != null && !recommended_order.contains (master_id))
          recommended_order[master_id] = cnt++;
      }
      return recommended_order;
    }

    /**
      * Checks if words from the search_string match title/column of first five rows in the home_model in given category.
      * Used for application scope results.
      */
    internal bool contains_visible_match (Dee.Model home_model, uint category, string search_string)
    {
      // based on Unity HomeLens implementation of the same function (almost 1:1 copy, modulo vala specific stuff)

      if (search_string.length == 0)
        return true;

      var model = new Dee.SequenceModel ();
      model.set_schema ("s", "s", null);

      // find first 5 rows of specific category of home model, copy title & comment into temporary model
      int checked_results = 5;
      var iter = binary_search (home_model, category);
      if (iter == null)
        return false;
      var end_iter = home_model.get_last_iter ();
      while (iter != end_iter && checked_results > 0)
      {
        var catidx = model.get_uint32 (iter, RESULT_CATEGORY_COLUMN);
        if (catidx != category) // results are sorted by categories, so break as soon as different category is has been reached
          break;

        var name = home_model.get_value (iter, RESULT_NAME_COLUMN);
        var comment = home_model.get_value (iter, RESULT_COMMENT_COLUMN);
        Variant data[2] = {name, comment};
        model.append_row (data);
        --checked_results;

        iter = home_model.next (iter);
      }

      if (model.get_n_rows () == 0)
        return false;

      // model reader that concatenates title and comment, separated by newline
      var reader = Dee.ModelReader.new ((m, iter) =>
      {
        return "%s\n%s".printf (m.get_string (iter, 0), m.get_string (iter, 1));
      });

      var index = new Dee.TreeIndex (model, analyzer, reader);

      // tokenize the search string, so this will work with multiple words
      Dee.TermList search_terms = Object.new (typeof (Dee.TermList)) as Dee.TermList;
      analyzer.tokenize (search_string, search_terms);

      var iters = new Gee.TreeSet<Dee.ModelIter> ();

      // iterate over all terms from search query
      for (uint i = 0; i<search_terms.num_terms (); i++)
      {
        // get rows matching search term
        var results = index.lookup (search_terms.get_term (i), i == search_terms.num_terms ()-1 ? Dee.TermMatchFlag.PREFIX : Dee.TermMatchFlag.EXACT);
        if (i == 0) // first search term
        {
          while (results.has_next ())
            iters.add (results.next ());
        }
        else // remaining search terms
        {
          var iters2 = new Gee.TreeSet<Dee.ModelIter> ();
          while (results.has_next ())
            iters2.add (results.next ());

          // intersect the sets of rows for 1st term and other terms, e.g. if query has three words: "foo bar baz"
          // then we get three sets of row iterators (for all rows that contain any of these words) that we intersect
          // to find out if all words are in same row.
          var it = iters.iterator ();
          while (it.has_next ())
          {
            it.next ();
            if (iters2.contains (it.get ()) == false)
              it.remove ();
          }

          // no need to check more terms if the base set is already empty
          if (iters.is_empty)
            break;
        }
      }

      // there is a match if the iterator is isn't empty
      return !iters.is_empty;
    }

    internal static Dee.ModelIter? binary_search (Dee.Model model, uint category)
    {
      if (model.get_n_rows () == 0)
        return null;

      uint l = 0;
      uint r = model.get_n_rows () - 1;

      while (l <= r)
      {
        uint i = (l + r) / 2;
        var iter = model.get_iter_at_row (i);
        var cat = model.get_uint32 (iter, RESULT_CATEGORY_COLUMN);
        if (category < cat)
        {
          r = i - 1;
        }
        else if (category > cat)
        {
          l = i + 1;
        }
        else
        {
          // iterate back to find first row with that category
          while (i > 0)
          {
            var prev = model.get_iter_at_row (--i);
            cat = model.get_uint32 (prev, RESULT_CATEGORY_COLUMN);
            if (cat != category)
              break;
            iter = prev;
          }
          return iter;
        }
      }
      return null;
    }

    internal static int cmp_category_data (CategoryData a, CategoryData b)
    {
      // Sort categories by number of personal / semi-personal / default results and recommendations from Smart Scope Service.
      // Personal results have highest priority.
      if (a.num_personal_results == b.num_personal_results)
      {
        if (a.num_semi_personal_results == b.num_semi_personal_results)
        {
          // apply sorting defined by dconf unity-homelens-priority key
          // - if one of the two scopes has dconf order defined (and the other doesn't), it comes first
          // - if both scopes have dconf order defined, then just sort by it (compute the difference).
          // - if none of them have dconf order defined, proceed with sorting by recommendations
          if (a.dconf_order >= 0 && a.num_total_results > 0)
          {
            if (b.dconf_order >= 0 && b.num_total_results > 0)
              return a.dconf_order - b.dconf_order;
            return -1; // A comes first
          }

          // no dconf order for A
          if (b.dconf_order >= 0 && b.num_total_results > 0)
            return 1; // B comes first

          // apply sorting suggested by recommendations from the server:
          // - if scope A and B both have defined recommended order, just sort by it (return the difference)
          // - if only one of them has a defined recommended order, prefer it by returning -1 or 1 respectively
          // - if neiter A nor B has a defined recommended order, sort by number of default results.
          if (a.recommended_order >=0 && a.num_total_results > 0)
          {
            if (b.recommended_order >= 0 && b.num_total_results > 0)
              return a.recommended_order - b.recommended_order;
            return -1; // A comes first
          }

          if (b.recommended_order >= 0 && b.num_total_results > 0)
            return 1; // B comes first

          if (a.num_default_results == b.num_default_results)
          {
            if (a.recommended_order >= 0 || b.recommended_order >= 0)
            {
              int delta_rec = a.recommended_order - b.recommended_order;
              if (a.recommended_order < 0) return 1; // B comes first
              if (b.recommended_order < 0) return -1; // A comes first
              return delta_rec;
            }
          }
          // just sort by number of default results
          return b.num_default_results - a.num_default_results;
        }
        else
        {
          return b.num_semi_personal_results - a.num_semi_personal_results;
        }
      }
      return b.num_personal_results - a.num_personal_results;
    }

    public Gee.ArrayList<string> sort_categories (string search_string, string home_channel, Dee.Model home_model, List<SmartScopes.RecommendedScope?> recommendations)
    {
      var recommended_order = create_recommended_order_map (recommendations);
      var result = new Gee.ArrayList<string> ();
      var cats = new List<CategoryData?> ();

      // iterate over all registered categories (scope ids), insert+sort them into cats list.
      foreach (unowned string id in catidx.get_keys ())
      {
        // sort categories by number of personal / semi-personal / default results
        var key = SessionKey ()
        {
          home_channel_id = home_channel,
          scope_id = id
        };

        //
        // support the case when given subscope returned 0 results
        if (!category_counts.contains (key))
        {
          var counts = new CategoryData (id);
          category_counts[key] = counts;
        }

        category_counts[key].recommended_order = (recommended_order.contains (id) ? recommended_order[id]: -1);
        category_counts[key].dconf_order = (dconf_order.contains (id) ? dconf_order[id]: -1);

        cats.insert_sorted (category_counts[key], (CompareFunc)cmp_category_data);
      };

      foreach (var cat in cats)
      {
        result.add (cat.scope_id);
      }

      if (result.size > 0)
      {
        var app_key = SessionKey ()
        {
          home_channel_id = home_channel,
          scope_id = APP_SCOPE_ID
        };

        // apply special ordering of app scope if it contains visual match
        if (category_counts.contains (app_key) && category_counts[app_key].num_total_results > 0)
        {
          if (result[0] != APP_SCOPE_ID) // nothing to do if apps category is already 1st
          {
            var app_scope_cat = get_category_index (APP_SCOPE_ID);
            if (contains_visible_match (home_model, app_scope_cat, search_string))
            {
              result.remove (APP_SCOPE_ID);
              result.insert (0, APP_SCOPE_ID);
            }
          }
        }

        // apply special ordering for 'more suggestions' (needs to be 3rd if not empty)
        if (result.size > 1)
        {
          var shopping_key = SessionKey ()
          {
            home_channel_id = home_channel,
            scope_id = MORE_SUGGESTIONS_SCOPE_ID
          };

          if (category_counts.contains (shopping_key) && category_counts[shopping_key].num_total_results > 0)
          {
            int idx = result.index_of (MORE_SUGGESTIONS_SCOPE_ID);
            if (idx >= 0 && idx != 2)
            {
              result.remove_at (idx);
              // sorting guarantees, that categories with 0 results are last, no matter if they are in default dconf order or recommendations
              if (result.size >= 3)
                result.insert (2, MORE_SUGGESTIONS_SCOPE_ID);
              else
                result.add (MORE_SUGGESTIONS_SCOPE_ID);
            }
          }
        }
      }

      return result;
    }

    public uint32[] get_category_order (string search_string, string home_channel, Dee.Model home_model, List<SmartScopes.RecommendedScope?> recommendations)
    {
      var res = new Gee.LinkedList<uint32> ();
      var cats = CategoryManager.instance ().sort_categories (search_string, home_channel, home_model, recommendations);
      foreach (var scope_id in cats)
      {
        var idx = CategoryManager.instance ().get_category_index (scope_id);
        res.add (idx);
        debug ("Category order: %s (index %d)", scope_id, idx);
      }
      return res.to_array ();
    }

    /**
    * Return static index of category associated with given scope in home scope, or -1 for uknown scope_id.
    */
    public int get_category_index (string scope_id)
    {
      if (catidx.contains (scope_id))
        return catidx[scope_id];
      return -1;
    }

    public int get_result_count (string home_channel, string scope_id)
    {
      var key = SessionKey ()
      {
        home_channel_id = home_channel,
        scope_id = scope_id
      };
      
      var count = category_counts.lookup (key);
      if (count != null)
        return count.num_total_results;
      return -1;
    }

    public string? get_scope_id_by_category_index (uint32 idx)
    {
      if (idx < categories.size)
        return categories[(int)idx];
      return null;
    }

    private void home_model_removed (Object obj)
    {
      debug ("Home model removed");
      if (home_models.contains (obj))
      {
        var channel = home_models[obj];
        home_models.remove (obj);

        category_counts.foreach_remove ((k, v) =>
        {
          return k.home_channel_id == channel;
        });
      }
    }

    public void observe (string home_channel_id, Dee.Model home_model)
    {
      if (!home_models.contains (home_model))
      {
        debug ("Observing home model for channel %s", home_channel_id);
        home_model.row_added.connect (on_row_added);
        home_model.row_removed.connect (on_row_removed);
        home_model.weak_ref (home_model_removed);
        home_models[home_model] = home_channel_id;
      }
    }

    /**
     * Increase personal/semi-personal/public results counters when row is added to home scope model.
     */
    private void on_row_added (Dee.Model model, Dee.ModelIter iter)
    {
      update_result_counts (model, iter, 1);
    }

    /**
     * Decrease personal/semi-personal/public results counters when row is removed from home scope model.
     */
    private void on_row_removed (Dee.Model model, Dee.ModelIter iter)
    {
      update_result_counts (model, iter, -1);
    }

    private void update_result_counts (Dee.Model model, Dee.ModelIter iter, int value)
    {
      var home_channel_id = home_models.lookup (model);
      if (home_channel_id == null)
      {
        warning ("No home channel mapping found for model");
        return;
      }

      var result_type = model.get_uint32 (iter, RESULT_TYPE_COLUMN);
      var category_idx = model.get_uint32 (iter, RESULT_CATEGORY_COLUMN);

      var id = get_scope_id_by_category_index (category_idx);
      if (id != null)
      {
        // update result count for master scope category
        update_result_counts_for_scope_id (home_channel_id, id, result_type, value);
      }
      else
      {
        warning ("No scope_id mapping for category index %u", category_idx);
      }

      // update result count for subscope id stored in metadata column (needed by metrics)
      var metadata = model.get_value (iter, RESULT_METADATA_COLUMN);
      var content = metadata.lookup_value ("content", VariantType.VARDICT);
      if (content != null)
      {
        var scope_id_var = content.lookup_value ("scope-id", VariantType.STRING);
        if (scope_id_var != null)
        {
          update_result_counts_for_scope_id (home_channel_id, scope_id_var.get_string (), result_type, value);
        }
      }
    }

    private void update_result_counts_for_scope_id (string home_channel_id, string scope_id, uint result_type, int value)
    {
      var key = SessionKey ()
      {
        home_channel_id = home_channel_id,
        scope_id = scope_id
      };
        
      if (result_type <= RESULT_TYPE_MAX)
      {
        var counts = category_counts.lookup (key);
        if (counts == null)
        {
          counts = new CategoryData (scope_id);
          category_counts[key] = counts;
        }
        counts.results[result_type] += value;
      }
      else
      {
        warning ("Result type out of range for %s", scope_id);
      }
    }
  }
}
