/*
 * Copyright (C) 2012 Canonical Ltd
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
 */

namespace Unity.ApplicationsLens
{
  public static int main (string[] args)
  {
    Test.init (ref args);

    Test.add_data_func ("/Unit/EmptySearchQuery", test_empty_search_query);
    Test.add_data_func ("/Unit/EmptySearchWithSingleCategoryFilter", test_empty_search_with_single_cat_filter);
    Test.add_data_func ("/Unit/EmptySearchWithMultipleCategoryFilter", test_empty_search_with_multi_cat_filter);
    Test.add_data_func ("/Unit/SearchQueryWithNoFilters", test_search_query_no_filters);
    Test.add_data_func ("/Unit/SearchQueryWithSingleCategoryFilter", test_search_query_with_single_cat_filter);
    Test.add_data_func ("/Unit/SearchQueryWithMultipleCategoryFilter", test_search_query_with_multi_cat_filter);

    Test.add_data_func ("/Unit/ZgEmptySearchQuery", test_zg_empty_search_query);
    Test.add_data_func ("/Unit/ZgEmptySearchWithSingleCategoryFilter", test_zg_empty_search_with_single_cat_filter);
    Test.add_data_func ("/Unit/ZgEmptySearchWithMultipleCategoryFilter", test_zg_empty_search_with_multi_cat_filter);
    Test.add_data_func ("/Unit/ZgSearchQuery", test_zg_search_query_no_filters);
    Test.add_data_func ("/Unit/ZgSearchQueryWithSingleCategoryFilter", test_zg_search_query_with_single_cat_filter);
    Test.add_data_func ("/Unit/ZgSearchQueryWithMultipleCategoryFilter", test_zg_search_query_with_multi_cat_filter);

    Test.run ();
    return 0;
  }

  internal static void test_empty_search_query ()
  {
    var query = XapianUtils.prepare_pkg_search_string ("", null);
    assert (query == "(type:Application OR type:Scope)");
  }

  internal static void test_empty_search_with_single_cat_filter ()
  {
    var filter = new Unity.CheckOptionFilter ("filter", "Filter");
    filter.filtering = true;
    filter.add_option ("media", "Media", null).active = true;
    filter.add_option ("internet", "Internet", null);

    var query = XapianUtils.prepare_pkg_search_string ("", filter);
    assert (query == "(type:Application OR type:Scope) AND (category:AudioVideo)");
  }

  internal static void test_empty_search_with_multi_cat_filter ()
  {
    var filter = new Unity.CheckOptionFilter ("filter", "Filter");
    filter.filtering = true;
    filter.add_option ("media", "Media", null).active = true;
    filter.add_option ("internet", "Internet", null);
    filter.add_option ("game", "Game", null).active = true;
    var query = XapianUtils.prepare_pkg_search_string ("", filter);
    assert (query == "(type:Application OR type:Scope) AND (category:Game OR category:AudioVideo)");
  }

  internal static void test_search_query_no_filters ()
  {
    var query = XapianUtils.prepare_pkg_search_string (" foo ", null);
    assert (query == "(type:Application OR type:Scope) AND foo");

    query = XapianUtils.prepare_pkg_search_string ("foo", null);
    assert (query == "(type:Application OR type:Scope) AND foo");
  }

  internal static void test_search_query_with_single_cat_filter ()
  {
    var filter = new Unity.CheckOptionFilter ("filter", "Filter");
    filter.filtering = true;
    filter.add_option ("media", "Media", null);
    filter.add_option ("internet", "Internet", null).active = true;
    var query = XapianUtils.prepare_pkg_search_string ("foo", filter);
    assert (query == "(type:Application OR type:Scope) AND (category:Network) AND foo");
  }

  internal static void test_search_query_with_multi_cat_filter ()
  {
    var filter = new Unity.CheckOptionFilter ("filter", "Filter");
    filter.filtering = true;
    filter.add_option ("media", "Media", null);
    filter.add_option ("internet", "Internet", null).active = true;
    filter.add_option ("game", "Game", null).active = true;
    var query = XapianUtils.prepare_pkg_search_string ("foo", filter);
    assert (query == "(type:Application OR type:Scope) AND (category:Game OR category:Network) AND foo");
  }

  internal static void test_zg_empty_search_query ()
  {
    var query = XapianUtils.prepare_zg_search_string ("", null);
    assert (query == "NOT category:XYZ");
  }

  internal static void test_zg_empty_search_with_single_cat_filter ()
  {
    var filter = new Unity.CheckOptionFilter ("filter", "Filter");
    filter.filtering = true;
    filter.add_option ("media", "Media", null);
    filter.add_option ("office", "Office", null).active = true;
    var query = XapianUtils.prepare_zg_search_string ("", filter);
    assert (query == "(category:Office)");
  }
  internal static void test_zg_empty_search_with_multi_cat_filter ()
  {
    var filter = new Unity.CheckOptionFilter ("filter", "Filter");
    filter.filtering = true;
    filter.add_option ("media", "Media", null);
    filter.add_option ("office", "Office", null).active = true;
    filter.add_option ("customization", "Customization", null).active = true;
    var query = XapianUtils.prepare_zg_search_string ("", filter);
    assert (query == "(category:Office OR category:Settings)");
  }

  internal static void test_zg_search_query_no_filters ()
  {
    var query = XapianUtils.prepare_zg_search_string (" foo ", null);
    assert (query == "app:(foo*)");

    query = XapianUtils.prepare_zg_search_string (" foo*", null);
    assert (query == "app:(foo*)");
  }

  internal static void test_zg_search_query_with_single_cat_filter ()
  {
    var filter = new Unity.CheckOptionFilter ("filter", "Filter");
    filter.filtering = true;
    filter.add_option ("media", "Media", null);
    filter.add_option ("internet", "Internet", null).active = true;
    var query = XapianUtils.prepare_zg_search_string (" foo* ", filter);
    assert (query == "app:(foo*) AND (category:Network)");
  }

  internal static void test_zg_search_query_with_multi_cat_filter ()
  {
    var filter = new Unity.CheckOptionFilter ("filter", "Filter");
    filter.filtering = true;
    filter.add_option ("media", "Media", null);
    filter.add_option ("internet", "Internet", null).active = true;
    filter.add_option ("game", "Game", null).active = true;
    var query = XapianUtils.prepare_zg_search_string (" foo* ", filter);
    assert (query == "app:(foo*) AND (category:Game OR category:Network)");
  }
}
