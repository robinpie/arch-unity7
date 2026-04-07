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
 *
 */

namespace Unity.VideoLens
{
  public static int main (string[] args)
  {
    /* Sort up locale to get translations but also sorting and
     * punctuation right */
    GLib.Intl.textdomain (Config.PACKAGE);
    GLib.Intl.bindtextdomain (Config.PACKAGE, Config.LOCALEDIR);
    GLib.Intl.bind_textdomain_codeset (Config.PACKAGE, "UTF-8");
    GLib.Intl.setlocale(GLib.LocaleCategory.ALL, "");

    Test.init (ref args);
    Test.add_data_func ("/UbuntuVideoSearch/ProcessSourcesResults", test_process_sources_results);
    Test.add_data_func ("/UbuntuVideoSearch/ProcessInvalidSourcesResults", test_process_invalid_sources_results);
    Test.add_data_func ("/UbuntuVideoSearch/BuildSearchUri", test_build_search_uri);
    Test.add_data_func ("/UbuntuVideoSearch/ProcessInvalidSearchResultsJson", test_invalid_search_results_json);
    Test.add_data_func ("/UbuntuVideoSearch/ProcessSearchResults", test_process_search_results);
    Test.add_data_func ("/UbuntuVideoSearch/ProcessSearchResultsTreatYourself", test_process_search_no_split_ty_category);
    Test.add_data_func ("/UbuntuVideoSearch/ProcessSearchResultsOnline", test_process_search_no_split_online_category);
    Test.add_data_func ("/UbuntuVideoSearch/JoinArray", test_join_array);
    Test.add_data_func ("/UbuntuVideoSearch/ProcessInvalidDetailsResults", test_process_invalid_details_results);
    Test.add_data_func ("/UbuntuVideoSearch/ProcessDetailsResults", test_process_details_results);

    Test.run ();

    return 0;
  }

  internal static void test_process_sources_results ()
  {
    assert (UbuntuVideoSearch.process_sources_results ("[]").size == 0);

    var sources = UbuntuVideoSearch.process_sources_results ("[\"Foo\", \"Bar\"]");
    assert (sources.size == 2);
    assert (sources[0] == "Foo");
    assert (sources[1] == "Bar");
  }

  internal static void test_process_invalid_sources_results ()
  {
    bool got_excp = false;
    try
    {
      UbuntuVideoSearch.process_sources_results (";");
    }
    catch (Error e)
    {
      got_excp = true;
    }
    assert (got_excp == true);
  }

  internal static void test_build_search_uri ()
  {
    // test null sources
    assert (UbuntuVideoSearch.build_search_uri ("foo", null) == "http://videosearch.ubuntu.com/v0/search?q=foo&split=true");

    // test empty sources list
    var sources = new Gee.ArrayList<string> (null);
    assert (UbuntuVideoSearch.build_search_uri ("foo", sources) == "http://videosearch.ubuntu.com/v0/search?q=foo&split=true");

    // test non-empty sources and uri escaping
    sources.add ("Amazon");
    sources.add ("BBC");
    assert (UbuntuVideoSearch.build_search_uri ("foo!", sources) == "http://videosearch.ubuntu.com/v0/search?q=foo%21&split=true&sources=Amazon,BBC");
  }

  internal static void test_invalid_search_results_json ()
  {
    // ignore warnings
    Test.log_set_fatal_handler (() => { return false; });

    bool got_excp = false;
    try
    {
      UbuntuVideoSearch.process_search_results (",", false);
    }
    catch (Error e)
    {
      got_excp = true;
    }
    assert (got_excp == true);
  }

  /**
   * Test search results of search query with 'split=true' flag, resulting in search results
   * being placed in CAT_INDEX_MORE (if from 'treats' results group) and CAT_INDEX_ONLINE category (if from 'other' group).
   */
  internal static void test_process_search_results ()
  {
    string datadir = Config.TESTDATADIR;
    uint8[] contents;
    assert (File.new_for_path (@"$datadir/videosearch_input1.txt").load_contents (null, out contents, null) == true);

    var results = UbuntuVideoSearch.process_search_results ((string)contents, false);
    assert (results.size == 5);

    bool got_video[5] = {false, false, false, false, false};

    // verify that all expected records are there
    foreach (var res in results)
    {
      if (res.title == "title0")
      {
        got_video[0] = true;
        assert (res.comment == "source0");
        assert (res.uri == "http://url0");
        assert (res.icon == "http://image0");
        assert (res.details_uri == "");
        assert (res.price == null);
        assert (res.category == CAT_INDEX_ONLINE);
      }
      else if (res.title == "title1")
      {
        got_video[1] = true;
        assert (res.comment == "source1");
        assert (res.uri == "http://url1");
        assert (res.icon == "http://image1");
        assert (res.details_uri == "http://details1");
        assert (res.price == null);
        assert (res.category == CAT_INDEX_ONLINE);
      }
      else if (res.title == "title2")
      {
        got_video[2] = true;
        assert (res.comment == "source2");
        assert (res.uri == "http://url2");
        assert (res.icon == "http://image2");
        assert (res.details_uri == "http://details2");
        assert (res.price == "1 USD");
        assert (res.category == CAT_INDEX_MORE);
      }
      else if (res.title == "title3")
      {
        got_video[3] = true;
        assert (res.comment == "source3");
        assert (res.uri == "http://url3");
        assert (res.icon == "http://image3");
        assert (res.details_uri == "http://details3");
        assert (res.price == _("Free"));
        assert (res.category == CAT_INDEX_MORE);
      }
      else if (res.title == "title4")
      {
        got_video[4] = true;
        assert (res.comment == "source4");
        assert (res.uri == "http://url4");
        assert (res.icon == "http://image4");
        assert (res.details_uri == "http://details4");
        assert (res.price == _("Free")); // null price of video4 turned into 'free' for 'Treat yourself category'
        assert (res.category == CAT_INDEX_MORE);
      }
      else
      {
        assert (1 == 0); // this shouldn't happen
      }
    }

    assert (got_video[0] == true);
    assert (got_video[1] == true);
    assert (got_video[2] == true);
    assert (got_video[3] == true);
    assert (got_video[4] == true);
  }

  /**
   * Test search results of search query with no 'split' option and is_treat_yourself=true, resulting in all search results
   * being placed in CAT_INDEX_MORE category.
   */
  internal static void test_process_search_no_split_ty_category ()
  {
    string datadir = Config.TESTDATADIR;
    uint8[] contents;
    assert (File.new_for_path (@"$datadir/videosearch_input2.txt").load_contents (null, out contents, null) == true);

    var results = UbuntuVideoSearch.process_search_results ((string)contents,  /* is_treat_yourself */ true);
    assert (results.size == 2);

    bool got_video[2] = {false, false};

    // verify that all expected records are there
    foreach (var res in results)
    {
      if (res.title == "title0")
      {
        got_video[0] = true;
        assert (res.comment == "source0");
        assert (res.uri == "http://url0");
        assert (res.icon == "http://image0");
        assert (res.details_uri == "");
        assert (res.price == _("Free"));
        assert (res.category == CAT_INDEX_MORE);
      }
      else if (res.title == "title1")
      {
        got_video[1] = true;
        assert (res.comment == "source1");
        assert (res.uri == "http://url1");
        assert (res.icon == "http://image1");
        assert (res.details_uri == "http://details1");
        assert (res.price == "1 USD");
        assert (res.category == CAT_INDEX_MORE);
      }
      else
      {
        assert (1 == 0); // this shouldn't happen
      }
    }

    assert (got_video[0] == true);
    assert (got_video[1] == true);
  }

  /**
   * Test search results of search query with no 'split' option and is_treat_yourself=false, resulting in all search results
   * being placed in CAT_INDEX_ONLINE
   */
  internal static void test_process_search_no_split_online_category ()
  {
    string datadir = Config.TESTDATADIR;
    uint8[] contents;
    assert (File.new_for_path (@"$datadir/videosearch_input2.txt").load_contents (null, out contents, null) == true);

    var results = UbuntuVideoSearch.process_search_results ((string)contents, /* is_treat_yourself */ false);
    assert (results.size == 2);

    bool got_video[2] = {false, false};

    // verify that all expected records are there
    foreach (var res in results)
    {
      if (res.title == "title0")
      {
        got_video[0] = true;
        assert (res.comment == "source0");
        assert (res.uri == "http://url0");
        assert (res.icon == "http://image0");
        assert (res.details_uri == "");
        assert (res.price == null);
        assert (res.category == CAT_INDEX_ONLINE);
      }
      else if (res.title == "title1")
      {
        got_video[1] = true;
        assert (res.comment == "source1");
        assert (res.uri == "http://url1");
        assert (res.icon == "http://image1");
        assert (res.details_uri == "http://details1");
        assert (res.price == null);
        assert (res.category == CAT_INDEX_ONLINE);
      }
      else
      {
        assert (1 == 0); // this shouldn't happen
      }
    }

    assert (got_video[0] == true);
    assert (got_video[1] == true);
  }

  internal static void test_join_array ()
  {
    var array = new Json.Array ();
    array.add_string_element ("abc");
    array.add_string_element ("def");

    assert (UbuntuVideoSearch.join_array (array, ", ") == "abc, def");
  }

  internal static void test_process_invalid_details_results ()
  {
    bool got_excp = false;
    try
    {
      UbuntuVideoSearch.process_details_results (",");
    }
    catch (Error e)
    {
      got_excp = true;
    }
    assert (got_excp == true);
  }

  internal static void test_process_details_results ()
  {
    string datadir = Config.TESTDATADIR;
    uint8[] contents;
    assert (File.new_for_path (@"$datadir/videosearch_details1.txt").load_contents (null, out contents, null) == true);

    var video = UbuntuVideoSearch.process_details_results ((string)contents);

    assert (video.title == "title1");
    assert (video.description == "a movie");
    assert (video.image == "http://image1");
    assert (video.directors.length == 2);
    assert (video.directors[0] == "director1");
    assert (video.directors[1] == "director2");
    assert (video.duration == 91);
    assert (video.genres.length == 2);
    assert (video.genres[0] == "genre1");
    assert (video.genres[1] == "genre2");
    assert (video.starring == "star1, star2");
    assert (video.source == "source1");
    assert (video.release_date == "2010");
    assert (video.uploaded_by == "uploader1");
    assert (video.price == "$1");
    assert (video.date_uploaded == "2012-06-06T21:55:12.000Z");
  }
}
