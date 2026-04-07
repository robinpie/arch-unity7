/*
 * Copyright (C) 2010 Canonical Ltd
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
 * Authored by Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 *
 */
using Posix;
using Unity;
using Gee;
 
namespace Unity.ApplicationsLens.Utils
{ 
  
  public AppInfo? get_app_info_for_actor (string actor)
  {
    string desktop_id = get_desktop_id_for_actor (actor);
    return AppInfoManager.get_default ().lookup (desktop_id);
  }
  
  public bool check_actor_has_category (string actor, Set<string>? categories)
  {
    if (categories == null)
      return false;
  
    AppInfoManager appman = AppInfoManager.get_default ();
    string desktop_id = get_desktop_id_for_actor (actor);
    AppInfo? appinfo =  appman.lookup (desktop_id);
    
    if (appinfo == null)
      return false;
    
    unowned string[]? actor_cats = appman.get_categories (desktop_id);
    if (actor_cats == null)
      return false;
    
    foreach (var cat in actor_cats)
    {
      if (cat in categories)
        return true;
    }
    
    return false;
  }
  
  public string get_desktop_id_for_actor (string actor)
  {
    if (actor.has_prefix("application://"))
      return actor.substring(14);
    else if (actor.has_prefix("app://"))
      return actor.substring(6);
    else if (actor.has_prefix("/"))
      {
        return Path.get_basename (actor);
      }
    
    /* At this point we assume that @actor is already the basename of the
     * .desktop file */
    return actor;
  }

  public bool is_search_empty (string? search_string)
  {
    if (search_string == null) return true;
    return search_string == "";
  }

  private static Dee.ICUTermFilter icu_filter;

  public string preprocess_string (string text)
  {
    if (icu_filter == null)
    {
      icu_filter = new Dee.ICUTermFilter.ascii_folder ();
    }

    var uncamelcased = uncamelcase (text);
    if (text != uncamelcased)
    {
      // make sure we don't discard the original completely
      uncamelcased = "%s\n%s".printf (text, uncamelcased);
    }
    var folded = icu_filter.apply (uncamelcased);

    if (uncamelcased == folded) return uncamelcased;

    return "%s\n%s".printf (uncamelcased, folded);
  }

  private static GLib.Regex camelcase_matcher;

  public string uncamelcase (string text)
  {
    if (camelcase_matcher == null)
    {
      try
      {
        // 1+ upper characters followed by 1+ non-upper 
        // (but preceded by non-upper or start of the string)
        camelcase_matcher = new GLib.Regex ("(?<=^|[^[:upper:]\\s])[[:upper:]]+[^[:upper:]]+",
                                       GLib.RegexCompileFlags.OPTIMIZE);
      }
      catch (Error e) { warning ("%s", e.message); }
    }

    /* we want to do "pyKaraoke" -> "py Karaoke",
       "CompizConfig" -> "Compiz Config", but leave "System Monitor" alone
     */
    try
    {
      string result;
      result = camelcase_matcher.replace_eval (text, -1, 0, 0,(mi, builder) =>
      {
        int start_pos;
        mi.fetch_pos (0, out start_pos, null);
        if (start_pos != 0) builder.append_c (' ');
        builder.append (mi.fetch (0));

        return false;
      });
      return result;
    }
    catch (Error e)
    {
      warning ("%s", e.message);
      return text;
    }
  }

  public Dee.Index prepare_index (Dee.Model model,
                                  uint sort_column,
                                  owned Dee.ModelReaderFunc reader_func,
                                  out Dee.Analyzer out_analyzer)
  {
    // reuse the icu_filter
    if (icu_filter == null)
    {
      icu_filter = new Dee.ICUTermFilter.ascii_folder ();
    }

    var sort_filter = Dee.Filter.new_collator (sort_column);
    var filter_model = new Dee.FilterModel (model, sort_filter);

    var analyzer = new Dee.TextAnalyzer ();
    analyzer.add_term_filter ((terms_in, terms_out) =>
    {
      for (uint i = 0; i < terms_in.num_terms (); i++)
      {
        unowned string term = terms_in.get_term (i);
        var folded = icu_filter.apply (term);
        terms_out.add_term (term);
        if (folded != term) terms_out.add_term (folded);
      }
    });
    out_analyzer = analyzer;

    var reader = Dee.ModelReader.new ((owned) reader_func);
    return new Dee.TreeIndex (filter_model, analyzer, reader);
  }

  public SList<Dee.ModelIter> search_index (Dee.Index index,
                                            Dee.Analyzer analyzer,
                                            string query)
  {
    if (is_search_empty (query))
    {
      var model = index.get_model ();
      var iter = model.get_first_iter ();
      var end_iter = model.get_last_iter ();

      var result = new SList<Dee.ModelIter> ();
      while (iter != end_iter)
      {
        result.prepend (iter);
        iter = model.next (iter);
      }
      result.reverse ();
      return result;
    }

    var term_list = Object.new (typeof (Dee.TermList)) as Dee.TermList;
    analyzer.tokenize (query, term_list);
    var matches = new Sequence<Dee.ModelIter> ();

    uint num_terms = term_list.num_terms ();
    for (uint i = 0; i < num_terms; i++)
    {
      var rs = index.lookup (term_list.get_term (i),
                             i < num_terms - 1 ? Dee.TermMatchFlag.EXACT : Dee.TermMatchFlag.PREFIX);

      bool first_pass = i == 0;
      CompareDataFunc<Dee.ModelIter> cmp_func = (a, b) =>
      {
        return a == b ? 0 : ((void*) a > (void*) b ? 1 : -1);
      };
      // intersect the results (cause we want to AND the terms)
      var remaining = new Sequence<Dee.ModelIter> ();
      foreach (var item in rs)
      {
        if (first_pass)
          matches.insert_sorted (item, cmp_func);
        else if (matches.lookup (item, cmp_func) != null)
          remaining.insert_sorted (item, cmp_func);
      }
      if (!first_pass) matches = (owned) remaining;
      // final result set empty already?
      if (matches.get_begin_iter () == matches.get_end_iter ()) break;
    }

    var result = new SList<Dee.ModelIter> ();
    var iter = matches.get_begin_iter ();
    var end_iter = matches.get_end_iter ();
    while (iter != end_iter)
    {
      result.prepend (iter.get ());
      iter = iter.next ();
    }

    result.reverse ();
    return result;
  }

  /* Substitute tilde character in @s by the home directory.
   * Expansion of ~username also works if 'username' is found. */
  public string subst_tilde (string s)
  {
    int k;
    string name;
    unowned Passwd? pw;

    if (s[0] != '~')
      return s;

    if (s.length == 1 || s[1] == '/')
      return Environment.get_home_dir () + s.substring (1, -1);

    k = s.index_of ("/");
    if (k == -1)
      name = s.substring (1, -1);
    else
      name = s.substring (1, k-1);

    pw = Posix.getpwnam (name);
    if (pw == null)
      return s;

    if (k == -1)
      return pw.pw_dir;
    else
      return pw.pw_dir + s.substring (k, -1);
  }

}
