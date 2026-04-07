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
 */

using Unity.HomeScope.SmartScopes;
using Unity.Protocol;

namespace Unity.HomeScope
{

  static const string HOME_SCOPE_DBUS_NAME = "com.canonical.Unity.Scope.HomeTest";
  static const string HOME_SCOPE_DBUS_PATH = "/com/canonical/unity/home";
  static const string FEEDBACK_FILE = Config.TESTRUNDATADIR + "/feedback.dump";

  bool verbose_debug = false;

  HomeScope? scope = null;
  Application? app = null;
  
  private KeywordSearch kw;
  const string[] RESULTS_SCHEMA = {"s", "s", "u", "u", "s", "s", "s", "s", "a{sv}"}; //TODO use schema def from libunity when it's public

  public class FakeSmartScopesServer
  {
    private Rand rand = new Rand ();
    private Pid server_pid;
    public int server_port { get; internal set; }

    public void start () throws SpawnError
    {
      server_port = rand.int_range (1024, 9000); 
    
      Process.spawn_async (null, {Config.TOPSRCDIR + "/tests/fake-server/fake-sss-server.py",
          "--scopes", Config.TOPSRCDIR + "/tests/fake-server/samples/remote-scopes-minimal.txt",
          "--search", Config.TESTRUNDATADIR + "/search.dump",
          "--feedback", FEEDBACK_FILE,
          "--requests", "9",
          "--timeout", "15",
          "--port", server_port.to_string (),
          Config.TOPSRCDIR + "/tests/unit/data/search_results1.txt"},
      null, 0, null, out server_pid);
    
      Socket socket = new Socket (SocketFamily.IPV4, SocketType.STREAM, SocketProtocol.TCP);
      assert (socket != null);
    
      InetAddress addr = new InetAddress.from_bytes ({127, 0, 0, 1}, SocketFamily.IPV4);
      InetSocketAddress server_addr = new InetSocketAddress (addr, (uint16)server_port);
      
      bool conn = false;
      int retry = 5;
      while (!conn && retry > 0)
      {
        try
        {
          conn = socket.connect (server_addr);
        }
        catch (Error e) {}
        if (!conn)
          Thread.usleep (1*1000000); // sleep for 1 second
        --retry;
      }
    }

    public void stop ()
    {
      Posix.kill (server_pid, Posix.SIGTERM);
      Process.close_pid (server_pid);
    }
  }

  FakeSmartScopesServer fake_server;

  public static int main (string[] args)
  {
    Environment.set_variable ("GSETTINGS_BACKEND", "memory", true);
    Environment.set_variable ("HOME_SCOPE_IGNORE_OFONO", "1", true);

    var xdg_data_dirs = Environment.get_variable ("XDG_DATA_DIRS");
    if (xdg_data_dirs == null) xdg_data_dirs = "/usr/share";
    Environment.set_variable ("XDG_DATA_DIRS",
                              "%s:%s".printf (Config.TESTDATADIR, xdg_data_dirs),
                              true);
    Environment.set_variable ("LIBUNITY_SCOPE_DIRECTORIES",
                              "%s/unity/scopes".printf (Config.TESTDATADIR),
                              true);

    fake_server = new FakeSmartScopesServer ();
    fake_server.start ();

    kw = new KeywordSearch ();

    Test.init (ref args); 
    
    Test.add_data_func ("/Unit/Search/ScopeRegistry", Fixture.create<HomeScopeSearchTester> (HomeScopeSearchTester.test_registry));
    Test.add_data_func ("/Unit/Search/KeywordSearch", Fixture.create<HomeScopeSearchTester> (HomeScopeSearchTester.test_keyword_search));
    Test.add_data_func ("/Unit/Search/SearchQueryState", Fixture.create<HomeScopeSearchTester> (HomeScopeSearchTester.test_search_query_state));
    Test.add_data_func ("/Unit/CategoryManager/BinarySearch", Fixture.create<HomeScopeSearchTester> (HomeScopeSearchTester.test_category_manager_binary_search));
    Test.add_data_func ("/Unit/CategoryManager/CmpCategoryData", Fixture.create<HomeScopeSearchTester> (HomeScopeSearchTester.test_category_manager_cmp_category_data));
    Test.add_data_func ("/Unit/CategoryManager/SortingPersonalContentOnly", Fixture.create<HomeScopeSearchTester> (HomeScopeSearchTester.test_category_manager_sort_personal));
    Test.add_data_func ("/Unit/CategoryManager/SortingFull", Fixture.create<HomeScopeSearchTester> (HomeScopeSearchTester.test_category_manager_sort_full));
    Test.add_data_func ("/Unit/CategoryManager/ContainsVisibleMatch", Fixture.create<HomeScopeSearchTester> (HomeScopeSearchTester.test_category_manager_contains_visible_match));

    Test.add_data_func ("/Unit/SearchUtil/BuildSearchScopesList", Fixture.create<SearchUtilTester> (SearchUtilTester.test_build_search_scopes_list));
    Test.add_data_func ("/Unit/SearchUtil/SetSubscopesFilterHint", Fixture.create<SearchUtilTester> (SearchUtilTester.test_set_subscopes_filter_hint));
    Test.add_data_func ("/Unit/SearchUtil/ScopesToQueryFromRequestedIds", Fixture.create<SearchUtilTester> (SearchUtilTester.test_scopes_to_query_from_requested_ids));
    Test.add_data_func ("/Unit/SearchUtil/GetMasterScopeIdFromScopeId", Fixture.create<SearchUtilTester> (SearchUtilTester.test_get_master_id_from_scope_id));

    Test.add_data_func ("/Unit/FilterState/SetFilters", Fixture.create<FilterStateTester> (FilterStateTester.test_set_filters));

    Test.add_data_func ("/Unit/SmartScopes/Parse", Fixture.create<SmartScopesUtilTester> (SmartScopesUtilTester.test_smart_scopes_parse));
    Test.add_data_func ("/Unit/SmartScopes/ParseErrors", Fixture.create<SmartScopesUtilTester> (SmartScopesUtilTester.test_smart_scopes_parse_errors));
    Test.add_data_func ("/Unit/SmartScopes/ParseMissingOptionalFields", Fixture.create<SmartScopesUtilTester> (SmartScopesUtilTester.test_smart_scopes_parse_missing_optional_fields));

    Test.add_data_func ("/Unit/SmartScopes/ChannelIdMap", Fixture.create<SmartScopesUtilTester> (SmartScopesUtilTester.test_channel_id_map));
    Test.add_data_func ("/Unit/SmartScopes/ClientScopesInfoFromData", Fixture.create<SmartScopesUtilTester> (SmartScopesUtilTester.test_client_scopes_info_from_data));
    Test.add_data_func ("/Unit/SmartScopes/ClientScopesInfoFromFile", Fixture.create<SmartScopesUtilTester> (SmartScopesUtilTester.test_client_scopes_info_from_file));
    Test.add_data_func ("/Unit/SmartScopes/PlatformVersion", Fixture.create<SmartScopesUtilTester> (SmartScopesUtilTester.test_platform_version));

    Test.add_data_func ("/Unit/MarkupCleaner/NoMarkup", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_no_markup));
    Test.add_data_func ("/Unit/MarkupCleaner/BrTagSupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_br_tag_support));
    Test.add_data_func ("/Unit/MarkupCleaner/BTagSupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_b_tag_support));
    Test.add_data_func ("/Unit/MarkupCleaner/ITagSupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_i_tag_support));
    Test.add_data_func ("/Unit/MarkupCleaner/UTagSupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_u_tag_support));
    Test.add_data_func ("/Unit/MarkupCleaner/TtTagSupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_tt_tag_support));
    Test.add_data_func ("/Unit/MarkupCleaner/STagSupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_s_tag_support));
    Test.add_data_func ("/Unit/MarkupCleaner/StrikeTagSupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_strike_tag_support));
    Test.add_data_func ("/Unit/MarkupCleaner/BigTagSupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_big_tag_support));
    Test.add_data_func ("/Unit/MarkupCleaner/SmallTagSupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_small_tag_support));
    Test.add_data_func ("/Unit/MarkupCleaner/SubTagSupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_sub_tag_support));
    Test.add_data_func ("/Unit/MarkupCleaner/SupTagSupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_sup_tag_support));
    Test.add_data_func ("/Unit/MarkupCleaner/UnsupportedTags", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_unsupported_tags));
    Test.add_data_func ("/Unit/MarkupCleaner/NestedTags", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_nested_tags));
    Test.add_data_func ("/Unit/MarkupCleaner/AmpEntitySupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_amp_entity));
    Test.add_data_func ("/Unit/MarkupCleaner/NbspEntitySupport", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_nbsp_entity));
    Test.add_data_func ("/Unit/MarkupCleaner/EntitiesArePreserved", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_basic_entities_are_preserved));
    Test.add_data_func ("/Unit/MarkupCleaner/UnsupportedEntitiesAreRaw", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_unsupported_entities_are_raw));
    Test.add_data_func ("/Unit/MarkupCleaner/NumericEntitiesArePreserved", Fixture.create<MarkupCleanerTester> (MarkupCleanerTester.test_num_entities_are_preserved));

    Test.add_data_func ("/Unit/HomeScopeInstance/PhoneFilters", Fixture.create<HomeScopeInstanceTester> (HomeScopeInstanceTester.test_phone_filters));
    Test.add_data_func ("/Unit/HomeScopeInstance/DesktopFilters", Fixture.create<HomeScopeInstanceTester> (HomeScopeInstanceTester.test_desktop_filters));

    Environment.set_variable ("SMART_SCOPES_SERVER", "http://127.0.0.1:%d".printf (fake_server.server_port), true);

    HomeScope.discover_scopes_sync ();

    app = Extras.dbus_own_name (HOME_SCOPE_DBUS_NAME, () =>
    {
      scope = new HomeScope ();
    });
    kw.rebuild ();

    Test.run ();

    fake_server.stop ();

    return 0;
  }

  class HomeScopeInstanceTester: Object, Fixture
  {
    ScopeProxy? proxy = null;
    string channel_id;

    private void setup ()
    {
      if (!scope.smart_scopes_ready) {
        var ml = new MainLoop ();
        scope.notify["smart_scopes_ready"].connect(() => { ml.quit (); });
        run_with_timeout (ml, 8);
      }

      proxy = acquire_test_proxy (HOME_SCOPE_DBUS_NAME, HOME_SCOPE_DBUS_PATH);
      channel_id = open_channel (proxy, ChannelType.GLOBAL, null);
      assert (channel_id != null);
    }

    private void teardown ()
    {
      proxy.close_channel (channel_id, null);
      proxy = null;
    }

    internal void test_phone_filters ()
    {
      // ignore warnings
      Test.log_set_fatal_handler (() => { return false; });

      bool got_filters_update = false;

      proxy.filter_settings_changed.connect ((chid, filter_rows) => {
          got_filters_update = true;
      });      
      
      var hints = new HashTable<string, Variant> (str_hash, str_equal);
      hints["form-factor"] = "phone";
      perform_search (proxy, channel_id, "metallica", hints, null);

      assert (got_filters_update == false); // we expect no filter updates on the phone
    }

    internal void test_desktop_filters ()
    {
      // ignore warnings
      Test.log_set_fatal_handler (() => { return false; });

      int got_filters_update = 0;

      Variant? filters = null;
      proxy.filter_settings_changed.connect ((chid, filter_rows) => {
          got_filters_update += 1;
          // note: there are 2 filter updates during search, but we care about the final value of filters
          filters = filter_rows;
      });
    
      assert (channel_id != null);
      assert (got_filters_update == 0);

      wait_for_synchronization (proxy.filters_model);
      assert (proxy.filters_model.get_n_rows () == 2);
      
      // verify all filter options are initially inactive
      int option_count = 0;
      for (var iter = proxy.filters_model.get_first_iter (); iter != proxy.filters_model.get_last_iter (); iter = proxy.filters_model.next (iter))
      {
        var opts = proxy.filters_model.get_row (iter)[4].lookup_value ("options", null);
        for (int i = 0; i<opts.n_children (); i++)
        {
          option_count += 1;
          var opt = opts.get_child_value(i);
          assert (opt.get_child_value (3).get_boolean () == false);
        }
      }

      assert (option_count == 17);

      var hints = new HashTable<string, Variant> (str_hash, str_equal);
      hints["form-factor"] = "desktop";
      perform_search (proxy, channel_id, "iron maiden", hints, null);

      assert (got_filters_update == 2);
      assert (filters.n_children() == 2); // two filters ('sources' and 'categories')
      var src_filter = filters.get_child_value(0);
      var cat_filter = filters.get_child_value(1);
      
      // verify 'sources' filter
      var opts = src_filter.get_child_value (4).lookup_value ("options", null);
      var option_flags = new HashTable<string, bool>(str_hash, str_equal);

      for (int i = 0; i<opts.n_children (); i++) // create scopeid -> enabled flag lookup for sources filter
      {
          var opt = opts.get_child_value(i);
          option_flags[opt.get_child_value (0).get_string ()] = opt.get_child_value (3).get_boolean ();
      }

      assert (option_flags["reference-stackexchange.scope"] == false);
      assert (option_flags["reference-dictionary.scope"] == false);
      assert (option_flags["reference-themoviedb.scope"] == false);
      assert (option_flags["masterscope_b-subscope1.scope"] == false);
      assert (option_flags["masterscope_b-subscope2.scope"] == false);
      assert (option_flags["masterscope_a-subscope1.scope"] == false);
      assert (option_flags["masterscope_a-subscope2.scope"] == false);
      assert (option_flags["more_suggestions-amazon.scope"] == false);
      assert (option_flags["more_suggestions-etsy.scope"] == false);
      assert (option_flags["more_suggestions-ebay.scope"] == false);
      assert (option_flags["more_suggestions-skimlinks.scope"] == false);
      
      assert (option_flags["more_suggestions-u1ms.scope"] == true);
      assert (option_flags["reference-wikipedia.scope"] == true);

      // verify 'categories' filter
      opts = cat_filter.get_child_value (4).lookup_value ("options", null);
      option_flags = new HashTable<string, bool>(str_hash, str_equal);

      for (int i = 0; i<opts.n_children (); i++) // create scopeid -> enabled flag lookup for categories filter
      {
          var opt = opts.get_child_value(i);
          option_flags[opt.get_child_value (0).get_string ()] = opt.get_child_value (3).get_boolean ();
      }
      
      assert (option_flags["masterscope_a.scope"] == false);
      assert (option_flags["masterscope_b.scope"] == false);

      assert (option_flags["more_suggestions.scope"] == true);
      assert (option_flags["reference.scope"] == true);
    }
  }

  class HomeScopeSearchTester: Object, Fixture
  {
    private Variant empty_asv = new Variant.array (new VariantType ("{sv}"), {});

    private void teardown ()
    {
      CategoryManager.instance ().clear ();
    }

    internal void test_registry ()
    {
      // scope registry is a singleton and is initialized in main on start
      var registry = ScopeRegistry.instance ();
      var scopes = registry.flatten ();
      assert (scopes.size == 8);
      assert (scopes.contains ("masterscope_a.scope"));
      assert (scopes.contains ("masterscope_b.scope"));
      assert (scopes.contains ("masterscope_a-subscope1.scope"));
      assert (scopes.contains ("masterscope_a-subscope2.scope"));
      assert (scopes.contains ("masterscope_b-subscope1.scope"));
      assert (scopes.contains ("masterscope_b-subscope2.scope"));
    }

    internal void test_keyword_search ()
    {
      assert (kw.num_of_mappings == 9);
      string new_search_string;

      assert (kw.process_query ("abcd: foobar", out new_search_string) == null); //unknown keyword, leave query as is
      assert (new_search_string == null);

      unowned Gee.Set<string>? ids = kw.process_query ("subb1: foobar", out new_search_string);
      assert (ids != null);
      assert (ids.size == 1 && ids.contains ("masterscope_b-subscope1.scope"));
      assert (new_search_string == "foobar");

      ids = kw.process_query ("mastera:foobar:123", out new_search_string);
      assert (ids != null);
      assert (ids.size == 1 && ids.contains ("masterscope_a.scope"));

      ids = kw.process_query ("mastera:foobar:123", out new_search_string);
      assert (ids != null);
      assert (ids.size == 1 && ids.contains ("masterscope_a.scope"));
      assert (new_search_string == "foobar:123");

      ids = kw.process_query ("sub1a1b: foobar:123", out new_search_string);
      assert (ids != null);
      assert (ids.size == 2 && ids.contains ("masterscope_a-subscope1.scope") && ids.contains ("masterscope_b-subscope1.scope"));
      assert (new_search_string == "foobar:123");
    }

    internal void test_search_query_state ()
    {
      var search_state = new SearchQueryState ();
      assert (search_state.has_channel ("channel1") == false);
      assert (search_state.search_query_changed ("channel1", "") == SearchQueryChange.NEW_QUERY);
      assert (search_state.search_query_changed ("channel1", "") == SearchQueryChange.NOT_CHANGED); //two empty queries in a row, this is intended
      assert (search_state.has_channel ("channel1") == true);
      assert (search_state.search_query_changed ("channel1", "t") == SearchQueryChange.NEW_QUERY);
      assert (search_state.has_channel ("channel1") == true);
      assert (search_state.search_query_changed ("channel2", "t") == SearchQueryChange.NEW_QUERY);
      assert (search_state.search_query_changed ("channel1", "t") == SearchQueryChange.NOT_CHANGED);
      assert (search_state.search_query_changed ("channel1", "te") == SearchQueryChange.APPENDS_TO_PREVIOUS_QUERY);
      assert (search_state.search_query_changed ("channel1", "term") == SearchQueryChange.APPENDS_TO_PREVIOUS_QUERY);
      assert (search_state.search_query_changed ("channel1", "te") == SearchQueryChange.REMOVES_FROM_PREVIOUS_QUERY);
      assert (search_state.search_query_changed ("channel1", "") == SearchQueryChange.NEW_QUERY);
      assert (search_state.has_channel ("channel1") == true);

      // simulate canned queries
      assert (search_state.search_query_changed ("channel1", "ab") == SearchQueryChange.NEW_QUERY);
      search_state.set_canned_query("channel1", "foo:lo");
      assert (search_state.search_query_changed ("channel1", "foo:lo") == SearchQueryChange.CANNED_QUERY);
      assert (search_state.search_query_changed ("channel1", "foo:lo") == SearchQueryChange.NOT_CHANGED);

      // simulate canned queries, user types a query immediately after activation
      assert (search_state.search_query_changed ("channel1", "me") == SearchQueryChange.NEW_QUERY);
      search_state.set_canned_query("channel1", "foo:metallica");
      assert (search_state.search_query_changed ("channel1", "iron maiden") == SearchQueryChange.NEW_QUERY);
      assert (search_state.search_query_changed ("channel1", "foo:metallica") == SearchQueryChange.NEW_QUERY); //it's no longer considered canned query
    }
    
    internal void test_category_manager_binary_search ()
    {
      var model = new Dee.SharedModel ("com.canonical.unity.scopes.test.scope1");
      model.set_schema_full (RESULTS_SCHEMA);

      var iter = CategoryManager.binary_search (model, 0); // search on empty model
      assert (iter == null);

      model.append ("uri", "", 333, 0, "", "", "", "", empty_asv);
      iter = CategoryManager.binary_search (model, 333); // model with 1 row
      assert (iter != null);
      assert (model.get_string (iter, 0) == "uri");

      // insert 100 categories, 3 rows for each category
      for (int i = 0; i < 100; i++)
      {
        for (int j = 1; j <= 3; j++)
          model.append ("uri%d-%d".printf (i, j), "", i, 0, "", "", "", "", empty_asv);
      }

      iter = CategoryManager.binary_search (model, 999); // doesn't exist
      assert (iter == null);
 
      iter = CategoryManager.binary_search (model, 0);
      assert (iter != null);
      assert (model.get_string (iter, 0) == "uri0-1");
     
      iter = CategoryManager.binary_search (model, 1);
      assert (iter != null);
      assert (model.get_string (iter, 0) == "uri1-1");

      iter = CategoryManager.binary_search (model, 13);
      assert (iter != null);
      assert (model.get_string (iter, 0) == "uri13-1");
      
      iter = CategoryManager.binary_search (model, 14);
      assert (iter != null);
      assert (model.get_string (iter, 0) == "uri14-1");

      iter = CategoryManager.binary_search (model, 99);
      assert (iter != null);
      assert (model.get_string (iter, 0) == "uri99-1");
    }
      
    internal void test_category_manager_cmp_category_data ()
    {
      CategoryManager.CategoryData cat1 = new CategoryManager.CategoryData ("scope1.scope");
      cat1.recommended_order = -1;
      cat1.dconf_order = -1;
      cat1.results[Unity.ResultType.PERSONAL] = 1;
      cat1.results[Unity.ResultType.SEMI_PERSONAL] = 3;
      cat1.results[Unity.ResultType.DEFAULT] = 5;

      CategoryManager.CategoryData cat2 = new CategoryManager.CategoryData ("scope2.scope");
      cat2.recommended_order = -1;
      cat2.dconf_order = -1;
      cat2.results[Unity.ResultType.PERSONAL] = 2;
      cat2.results[Unity.ResultType.SEMI_PERSONAL] = 1;
      cat2.results[Unity.ResultType.DEFAULT] = 5;
      assert (CategoryManager.cmp_category_data (cat1, cat2) > 0); // cat2 first, based on personal content

      cat1.results[Unity.ResultType.PERSONAL] = 2; // both have now same number of personal content
      assert (CategoryManager.cmp_category_data (cat1, cat2) < 0); // cat1 first, based on semi-personal content
      
      cat1.results[Unity.ResultType.SEMI_PERSONAL] = 1; // both have now same number of semi-personal content
      assert (CategoryManager.cmp_category_data (cat1, cat2) == 0); // equal
      
      cat1.results[Unity.ResultType.DEFAULT] = 7; // cat1 has now more default content
      assert (CategoryManager.cmp_category_data (cat1, cat2) < 0); // cat1 first, based on default content

      cat2.recommended_order = 1; //recommended order for cat1 remains unset
      assert (CategoryManager.cmp_category_data (cat1, cat2) > 0); // cat2 first, based on recommended order
      
      cat1.recommended_order = 2;
      assert (CategoryManager.cmp_category_data (cat1, cat2) > 0); // cat2 still first, based on recommended order
      
      cat1.recommended_order = 0;
      assert (CategoryManager.cmp_category_data (cat1, cat2) < 0); // cat1 first, higher recommended order

      cat2.dconf_order = 0;
      assert (CategoryManager.cmp_category_data (cat1, cat2) > 0); // cat2 first, based on dconf order, overrides recommended order of cat1
      
      cat1.dconf_order = 0;
      cat2.dconf_order = 1;
      assert (CategoryManager.cmp_category_data (cat1, cat2) < 0); // cat1 first, based on dconf order
    }

    /*
     * Test sorting based on number of personal/semi-personal/public content *only* (no app scope visible match, no dconf order defined, no recommendations
     */
    internal void test_category_manager_sort_personal ()
    {
      var mgr = CategoryManager.instance ();
      mgr.register ("scope1.scope");
      mgr.register ("scope2.scope");
      mgr.register ("scope3.scope");
      mgr.register ("scope4.scope");
      mgr.register ("scope5.scope");
      mgr.register ("more_suggestions.scope");

      assert (mgr.get_category_index ("scope1.scope") == 0);
      assert (mgr.get_category_index ("scope2.scope") == 1);
      assert (mgr.get_category_index ("scope3.scope") == 2);
      assert (mgr.get_category_index ("scope4.scope") == 3);
      assert (mgr.get_category_index ("scope5.scope") == 4);
      assert (mgr.get_category_index ("more_suggestions.scope") == 5);
      assert (mgr.get_scope_id_by_category_index (0) == "scope1.scope");
      assert (mgr.get_scope_id_by_category_index (1) == "scope2.scope");
      assert (mgr.get_scope_id_by_category_index (2) == "scope3.scope");
      assert (mgr.get_scope_id_by_category_index (3) == "scope4.scope");
      assert (mgr.get_scope_id_by_category_index (4) == "scope5.scope");
      assert (mgr.get_scope_id_by_category_index (5) == "more_suggestions.scope");

      var scope_model1_ch1 = new Dee.SharedModel ("com.canonical.unity.scopes.test.scope1");
      scope_model1_ch1.set_schema_full (RESULTS_SCHEMA);
      var scope_model2_ch1 = new Dee.SharedModel ("com.canonical.unity.scopes.test.scope2");
      scope_model2_ch1.set_schema_full (RESULTS_SCHEMA);

      mgr.observe ("channel1", scope_model1_ch1);
      mgr.observe ("channel2", scope_model2_ch1);

      // populate home model with results from various scopes; normally this is done my synchronizer.
      // note that source scope is determined by category index value and matches get_scope_id_by_category_index
      // values above.

      // scope3 adds 3 public results
      scope_model1_ch1.append ("uri5", "icon", 2, ResultType.DEFAULT, "mimetype", "title3", "comment3", "dnd_uri3", empty_asv);
      scope_model1_ch1.append ("uri7", "icon", 2, ResultType.DEFAULT, "mimetype", "title1", "comment1", "dnd_uri1", empty_asv);
      scope_model1_ch1.append ("uri7", "icon", 2, ResultType.DEFAULT, "mimetype", "title4", "comment3", "dnd_uri4", empty_asv);

      // scope1 adds 2 personal results
      scope_model1_ch1.append ("uri2", "icon", 0, ResultType.PERSONAL, "mimetype", "title2", "comment2", "dnd_uri2", empty_asv);
      scope_model1_ch1.append ("uri1", "icon", 0, ResultType.PERSONAL, "mimetype", "title1", "comment1", "dnd_uri1", empty_asv);

      // scope2 adds 2 public results 
      scope_model1_ch1.append ("uri4", "icon", 1, ResultType.DEFAULT, "mimetype", "title4", "comment3", "dnd_uri4", empty_asv);
      scope_model1_ch1.append ("uri3", "icon", 1, ResultType.DEFAULT, "mimetype", "title3", "comment3", "dnd_uri3", empty_asv);

      // scope5 adds 2 personal and 1 semi-personal result
      scope_model1_ch1.append ("uri8", "icon", 4, ResultType.PERSONAL, "mimetype", "title8", "comment8", "dnd_uri8", empty_asv);
      scope_model1_ch1.append ("uri9", "icon", 4, ResultType.PERSONAL, "mimetype", "title9", "comment9", "dnd_uri9", empty_asv);
      scope_model1_ch1.append ("uri10", "icon", 4, ResultType.SEMI_PERSONAL, "mimetype", "title10", "comment10", "dnd_uri10", empty_asv);

      // results for channel2, not really used but they are here to verify they don't impact sorting of channel1
      scope_model2_ch1.append ("uri1", "icon", 2, ResultType.PERSONAL, "mimetype", "title1", "comment1", "dnd_uri1", empty_asv);
      scope_model2_ch1.append ("uri2", "icon", 2, ResultType.PERSONAL, "mimetype", "title2", "comment2", "dnd_uri2", empty_asv);
      scope_model2_ch1.append ("uri2", "icon", 2, ResultType.PERSONAL, "mimetype", "title2", "comment2", "dnd_uri2", empty_asv);
      
      var recommendations = new List<SmartScopes.RecommendedScope?> ();

      // verify order of scope ids
      var order_ids = mgr.sort_categories ("foo", "channel1", scope_model1_ch1, recommendations);

      assert (order_ids.size == 6);
      assert (order_ids[0] == "scope5.scope");
      assert (order_ids[1] == "scope1.scope");
      assert (order_ids[2] == "scope3.scope");
      assert (order_ids[3] == "scope2.scope");
      // no data for scope4 or more_suggestions.scope

      // verify order of scope indices (it matches order of ids)
      var order_idx = mgr.get_category_order ("foo", "channel1", scope_model1_ch1, recommendations);

      assert (order_idx.length == 6);
      assert (order_idx[0] == 4);
      assert (order_idx[1] == 0);
      assert (order_idx[2] == 2);
      assert (order_idx[3] == 1);
    }
    
    /*
     * Full category sorting scenario: personal/public content, visible match in app scope, defined dconf order & recommendations from Smart Scope Service,
     * shopping results.
     */
    internal void test_category_manager_sort_full ()
    {
      var mgr = CategoryManager.instance ();
      mgr.register ("scope1.scope");
      mgr.register ("scope2.scope");
      mgr.register ("scope3.scope");
      mgr.register ("scope4.scope");
      mgr.register ("scope5.scope");
      mgr.register ("applications.scope");
      mgr.register ("more_suggestions.scope");

      var scope_model1_ch1 = new Dee.SharedModel ("com.canonical.unity.scopes.test.scope1");
      scope_model1_ch1.set_schema_full (RESULTS_SCHEMA);

      mgr.observe ("channel1", scope_model1_ch1);

      // populate home model with results from various scopes; normally this is done my synchronizer.
      // note that source scope is determined by category index value and matches get_scope_id_by_category_index
      // values above.

      // scope3 adds 3 public results
      scope_model1_ch1.append ("uri5", "icon", 2, ResultType.DEFAULT, "mimetype", "title3", "comment3", "dnd_uri3", empty_asv);
      scope_model1_ch1.append ("uri7", "icon", 2, ResultType.DEFAULT, "mimetype", "title1", "comment1", "dnd_uri1", empty_asv);
      scope_model1_ch1.append ("uri7", "icon", 2, ResultType.DEFAULT, "mimetype", "title4", "comment3", "dnd_uri4", empty_asv);

      // scope1 adds 2 personal results
      scope_model1_ch1.append ("uri2", "icon", 0, ResultType.PERSONAL, "mimetype", "title2", "comment2", "dnd_uri2", empty_asv);
      scope_model1_ch1.append ("uri1", "icon", 0, ResultType.PERSONAL, "mimetype", "title1", "comment1", "dnd_uri1", empty_asv);

      // scope2 adds 2 personal results as well
      scope_model1_ch1.append ("uri4", "icon", 1, ResultType.PERSONAL, "mimetype", "title4", "comment3", "dnd_uri4", empty_asv);
      scope_model1_ch1.append ("uri3", "icon", 1, ResultType.PERSONAL, "mimetype", "title3", "comment3", "dnd_uri3", empty_asv);

      // scope5 adds 2 personal and 1 semi-personal result
      scope_model1_ch1.append ("uri8", "icon", 4, ResultType.PERSONAL, "mimetype", "title8", "comment8", "dnd_uri8", empty_asv);
      scope_model1_ch1.append ("uri9", "icon", 4, ResultType.PERSONAL, "mimetype", "title9", "comment9", "dnd_uri9", empty_asv);
      scope_model1_ch1.append ("uri10", "icon", 4, ResultType.SEMI_PERSONAL, "mimetype", "title10", "comment10", "dnd_uri10", empty_asv);

      // unity-scope-applications.scope results
      scope_model1_ch1.append ("uri11", "icon", 5, ResultType.PERSONAL, "mimetype", "firefox", "Firefox web browser", "dnd_uri8", empty_asv);
      
      // more_suggestions.scope results
      scope_model1_ch1.append ("uri11", "icon", 6, ResultType.DEFAULT, "mimetype", "Metallica", "Master of Puppets", "dnd_uri8", empty_asv);


      // recommendations from Smart Scopes Service.
      // - first recommended scope contains only public content
      // - second and third recommended scopes contain equal number of personal results
      // - therefore recommended order will affect scope2 & scope1 only, scope3 will appear after them.
      var recommendations = new List<SmartScopes.RecommendedScope?> ();
      recommendations.append (SmartScopes.RecommendedScope () { scope_id = "scope3.scope", scope_type = SmartScopes.ScopeType.ClientScope });
      recommendations.append (SmartScopes.RecommendedScope () { scope_id = "scope1.scope", scope_type = SmartScopes.ScopeType.ClientScope });
      recommendations.append (SmartScopes.RecommendedScope () { scope_id = "scope2.scope", scope_type = SmartScopes.ScopeType.ClientScope });

      var dconf_order = new Gee.ArrayList<string> ();

      // verify order of scope ids
      var order_ids = mgr.sort_categories ("firefox", "channel1", scope_model1_ch1, recommendations);

      assert (order_ids.size == 7);
      assert (order_ids[0] == "applications.scope");
      assert (order_ids[1] == "scope5.scope");
      assert (order_ids[2] == "more_suggestions.scope");
      assert (order_ids[3] == "scope1.scope");
      assert (order_ids[4] == "scope2.scope");
      assert (order_ids[5] == "scope3.scope");
      assert (order_ids[6] == "scope4.scope"); // no data for scope4
      
      // repeat with dconf order that takes precedence over recommendations
      dconf_order.add ("applications.scope");
      dconf_order.add ("scope2.scope");
      dconf_order.add ("scope1.scope");

      mgr.set_default_sort_order (dconf_order);
      
      order_ids = mgr.sort_categories ("firefox", "channel1", scope_model1_ch1, recommendations);
     
      assert (order_ids.size == 7);
      assert (order_ids[0] == "applications.scope");
      assert (order_ids[1] == "scope5.scope");
      assert (order_ids[2] == "more_suggestions.scope");
      assert (order_ids[3] == "scope2.scope");
      assert (order_ids[4] == "scope1.scope");
      assert (order_ids[5] == "scope3.scope");
      assert (order_ids[6] == "scope4.scope"); // no data for scope4
    }

    internal void test_category_manager_contains_visible_match ()
    {
      var mgr = CategoryManager.instance ();
      mgr.register ("scope1.scope");
      mgr.register ("applications.scope");
      assert (mgr.get_category_index ("applications.scope") == 1);

      var scope_model1_ch1 = new Dee.SharedModel ("com.canonical.unity.scopes.test.scope1");
      scope_model1_ch1.set_schema_full (RESULTS_SCHEMA);

      // scope1 adds 2 results
      scope_model1_ch1.append ("uri1", "icon", 0, ResultType.DEFAULT, "mimetype", "title1", "comment3", "dnd_uri3", empty_asv);
      scope_model1_ch1.append ("uri2", "icon", 0, ResultType.DEFAULT, "mimetype", "title2", "comment1", "dnd_uri1", empty_asv);

      // app scope adds 5
      scope_model1_ch1.append ("uri3", "icon", 1, ResultType.PERSONAL, "mimetype", "this is a title3", "comment one", "dnd_uri2", empty_asv);
      scope_model1_ch1.append ("uri4", "icon", 1, ResultType.PERSONAL, "mimetype", "another title4", "comment two", "dnd_uri1", empty_asv);
      scope_model1_ch1.append ("uri5", "icon", 1, ResultType.PERSONAL, "mimetype", "foo bar title5", "comment three", "dnd_uri1", empty_asv);
      scope_model1_ch1.append ("uri7", "icon", 1, ResultType.PERSONAL, "mimetype", "abcdef title7", "comment five", "dnd_uri1", empty_asv);
      scope_model1_ch1.append ("uri6", "icon", 1, ResultType.PERSONAL, "mimetype", "foo bar title6", "comment four", "dnd_uri1", empty_asv);
      scope_model1_ch1.append ("uri6", "icon", 1, ResultType.PERSONAL, "mimetype", "foo bar title7", "comment four", "dnd_uri1", empty_asv);

      assert (mgr.contains_visible_match (scope_model1_ch1, 1, "qwerty") == false);
      assert (mgr.contains_visible_match (scope_model1_ch1, 1, "foo") == true); //one word from a title
      assert (mgr.contains_visible_match (scope_model1_ch1, 1, "foo bar title6") == true); //all words from a title
      assert (mgr.contains_visible_match (scope_model1_ch1, 1, "foo bar title7") == false); //fails because we only take 5 results into account
      assert (mgr.contains_visible_match (scope_model1_ch1, 1, "def") == false); //no suffix match
      assert (mgr.contains_visible_match (scope_model1_ch1, 1, "abc") == true); //prefix match works
      assert (mgr.contains_visible_match (scope_model1_ch1, 1, "five") == true); //match on second word from comment column
      assert (mgr.contains_visible_match (scope_model1_ch1, 1, "comment") == true); //match on first word from comment column
      assert (mgr.contains_visible_match (scope_model1_ch1, 1, "this one") == true); //match on "this" in the title and "one" in comment column.
    }
  }

  class SearchUtilTester: Object, Fixture
  {
    internal void test_build_search_scopes_list ()
    {
      var search_scopes = new HashTable<string, Gee.Set<string>?> (str_hash, str_equal);
      SearchUtil.build_search_scopes_list ("masterscope_a.scope", search_scopes);
      SearchUtil.build_search_scopes_list ("masterscope_b-subscope1.scope", search_scopes);

      assert (search_scopes.size () == 2);
      assert (search_scopes.contains ("masterscope_a.scope"));
      assert (search_scopes["masterscope_a.scope"] == null);
      assert (search_scopes.contains ("masterscope_b.scope"));
      var subscopes = search_scopes["masterscope_b.scope"];
      assert (subscopes.size == 1);
      assert (subscopes.contains ("masterscope_b-subscope1.scope"));
    }

    internal void test_set_subscopes_filter_hint ()
    {
      Gee.Set<string> subscopes1 = new Gee.TreeSet<string> ();
      Gee.Set<string> subscopes2 = new Gee.TreeSet<string> ();
      var search_scopes = new HashTable<string, Gee.Set<string>?> (str_hash, str_equal);
      subscopes1.add ("scope1-subscope1.scope");
      subscopes2.add ("scope2-subscope1.scope");
      subscopes2.add ("scope2-subscope2.scope");
      search_scopes["scope1.scope"] = subscopes1;
      search_scopes["scope2.scope"] = subscopes2;

      var hints = new HashTable<string, Variant> (str_hash, str_equal);

      SearchUtil.set_subscopes_filter_hint (hints, search_scopes, "scope1.scope");
      assert (hints.size () == 1);
      assert (hints.contains ("subscopes-filter"));
      var subscopes_variant = hints["subscopes-filter"];
      assert (subscopes_variant.n_children () == 1);
      assert (subscopes_variant.get_child_value (0).get_string () == "scope1-subscope1.scope");
      
      SearchUtil.set_subscopes_filter_hint (hints, search_scopes, "scope2.scope");
      assert (hints.size () == 1);
      assert (hints.contains ("subscopes-filter"));
      subscopes_variant = hints["subscopes-filter"];
      assert (subscopes_variant.n_children () == 2);
      assert (subscopes_variant.get_child_value (0).get_string () == "scope2-subscope1.scope");
      assert (subscopes_variant.get_child_value (1).get_string () == "scope2-subscope2.scope");

      hints.remove_all ();
      SearchUtil.set_subscopes_filter_hint (hints, search_scopes, "foobar.scope");
      assert (hints.size () == 0); //no hints if unknown scope
    }

    internal void test_scopes_to_query_from_requested_ids ()
    {
      Gee.Set<string> requested_by_kw = new Gee.TreeSet<string> ();
      requested_by_kw.add ("masterscope_a.scope");
      requested_by_kw.add ("masterscope_b-subscope1.scope");
      requested_by_kw.add ("masterscope_b-subscope2.scope");

      var search_scopes = new HashTable<string, Gee.Set<string>?> (str_hash, str_equal);

      bool direct_search = SearchUtil.scopes_to_query_from_requested_ids (requested_by_kw, search_scopes);
      assert (direct_search == true);
      assert (search_scopes.size () == 2);
      assert (search_scopes.contains ("masterscope_a.scope"));
      assert (search_scopes["masterscope_a.scope"] == null);
      assert (search_scopes.contains ("masterscope_b.scope"));
      var subscopes = search_scopes["masterscope_b.scope"];
      assert (subscopes.size == 2);
      assert (subscopes.contains ("masterscope_b-subscope1.scope"));
      assert (subscopes.contains ("masterscope_b-subscope2.scope"));
    }

    internal void test_get_master_id_from_scope_id ()
    {
      assert (SearchUtil.get_master_id_from_scope_id ("foo-bar.scope") == "foo.scope");
      assert (SearchUtil.get_master_id_from_scope_id ("foo.scope") == null);
    }
  }

  class FilterStateTester: Object, Fixture
  {
    internal void test_set_filters ()
    {
      var categories = new Unity.OptionsFilter ();
      categories.add_option ("a.scope", "A");

      var sources = new Unity.OptionsFilter ();
      sources.add_option ("a-b.scope", "A");
      
      var enabled_scopes = new string [0];
      assert (FilterState.set_filters (categories, sources, enabled_scopes) == false);
      assert (categories.get_option ("a.scope").active == false);
      assert (sources.get_option ("a-b.scope").active == false);

      enabled_scopes += "a.scope";
      assert (FilterState.set_filters (categories, sources, enabled_scopes) == true);
      assert (FilterState.set_filters (categories, sources, enabled_scopes) == false);
      assert (categories.get_option ("a.scope").active == true);
      assert (sources.get_option ("a-b.scope").active == false);

      enabled_scopes += "a-b.scope";
      assert (FilterState.set_filters (categories, sources, enabled_scopes) == true);
      assert (FilterState.set_filters (categories, sources, enabled_scopes) == false);
      assert (categories.get_option ("a.scope").active == true);
      assert (sources.get_option ("a-b.scope").active == true);
    }
  }

  class SmartScopesUtilTester: Object, Fixture
  {
    internal void test_smart_scopes_parse ()
    {
      CategoryManager.instance ().register ("more_suggestions.scope");
      CategoryManager.instance ().register ("reference.scope");

      int row_count = 0;
      int recommend_count = 0;

      var search_handler = new SmartScopes.SearchResponseHandler ();

      search_handler.parse_results_line ("""{"scopes": [["more_suggestions-amazon","server"],["more_suggestions-u1ms","server"]],"server_sid": "abcd", "type": "recommendations"}""",
          (scope_id, row) => 
          {
            assert_not_reached ();
          },
          (server_sid, recommend) => 
          {
            recommend_count++;
            assert (recommend.length () == 2);
            assert (recommend.nth_data (0).scope_id == "more_suggestions-amazon");
            assert (recommend.nth_data (1).scope_id == "more_suggestions-u1ms");
          });

      assert (recommend_count == 1);

      search_handler.parse_results_line ("""{"info": {"reference-wikipedia": [{"metadata":{"id":"1", "images":{}},"uri":"foo","title":"a"}]}, "type": "results"}""",
          (scope_id, row) =>
          {
            row_count++;
            assert (scope_id == "reference-wikipedia");
            //TODO assert on row values
          },
          (server_sid, recommend) =>
          {
            assert_not_reached ();
          });

      assert (row_count == 1);
    }

    internal void test_smart_scopes_parse_errors ()
    {
      // ignore warnings
      Test.log_set_fatal_handler (() => { return false; });

      var search_handler = new SmartScopes.SearchResponseHandler ();
      bool got_excp = false;
      try
      {
        search_handler.parse_results_line ("""{"type": "recommendations"}""",
            (scope_id, row) => { assert_not_reached (); },
            (recommend) => { assert_not_reached (); });
      }
      catch (SmartScopes.ParseError e) { got_excp = true; }

      assert (got_excp == true);

      got_excp = false;
      try
      {
        search_handler.parse_results_line ("""{"type": "results"}""", 
            (scope_id, row) => { assert_not_reached (); },
            (recommend) => { assert_not_reached (); });
      }
      catch (SmartScopes.ParseError e) { got_excp = true; }
    }

    internal void test_smart_scopes_parse_missing_optional_fields ()
    {
      int row_count = 0;
      var search_handler = new SmartScopes.SearchResponseHandler ();

      // missing or null 'images' in the metadata
      search_handler.parse_results_line ("""{"info": {"searchin-scope.scope": [{"title": "search in foursquare...", "icon_hint": "file:///usr/share/icons/unity-icon-theme/places/svg/group-info.svg", "uri": "scopes-query://foursquare:drink", "metadata": {"images":null}}, {"title": "search in recipepuppy...", "icon_hint": "file:///usr/share/icons/unity-icon-theme/places/svg/group-recipes.svg", "uri": "scopes-query://recipepuppy:drink", "metadata": {}}, {"title": "search in grooveshark...", "icon_hint": "file:///usr/share/icons/unity-icon-theme/places/svg/service-grooveshark.svg", "uri": "scopes-query://grooveshark:drink", "metadata": {}}, {"title": "search in ebay...", "icon_hint": "file:///usr/share/icons/unity-icon-theme/places/svg/service-ebay.svg", "uri": "scopes-query://ebay:drink", "metadata": {}}, {"title": "search in songkick...", "icon_hint": "file:///usr/share/icons/unity-icon-theme/places/svg/group-music.svg", "uri": "scopes-query://songkick:drink", "metadata": {}}]}, "type": "results"}""",
            (scope_id, row) => {
              assert (scope_id == "searchin-scope.scope");
              if (row_count == 0) {
                  assert (row[0].get_string () == "x-unity-no-preview-scopes-query://foursquare:drink"); // uri
                  assert (row[1].get_string () == "file:///usr/share/icons/unity-icon-theme/places/svg/group-info.svg"); // icon hint
                  assert (row[2].get_uint32 () == 0); // category
                  assert (row[3].get_uint32 () == Unity.ResultType.DEFAULT); // result type
                  assert (row[4].get_string () == "text/html"); // mimetype
                  assert (row[5].get_string () == "search in foursquare..."); // title
                  assert (row[6].get_string () == ""); // comment
                  assert (row[7].get_string () == "x-unity-no-preview-scopes-query://foursquare:drink"); // dnd uri
                  assert (row[8].is_of_type (new VariantType ("a{sv}")) && row[8].n_children () == 0); // metadata
              }
              row_count++;
            },
            (recommend) => { assert_not_reached (); });

      assert (row_count == 5);
    }

    const string SERVER_URI = "https://foobar.ubuntu.com";
    const string SEARCH_URI_PREFIX = SERVER_URI + "/smartscopes/v1/search";

    internal void test_smart_scopes_metrics ()
    {
      var server_sid1 = "abcdef";
      var server_sid2 = "ghijkl";
      var now = new DateTime.now_utc ();
      var metrics = new SmartScopes.SmartScopesMetrics ();

      assert (metrics.num_events == 0);

      metrics.add_preview_event ("123", server_sid1, "foo1.scope", now);
      var time = now.add_minutes (1);
      metrics.add_click_event ("123", server_sid1, "foo2.scope", time);
      time = now.add_minutes (2);
      metrics.add_preview_event ("981", server_sid1, "foo4.scope", time);
      time = now.add_minutes (3);
      metrics.add_click_event ("888", server_sid2, "foo5.scope", time);

      var scope_res = new Gee.HashMap<string, int> ();
      scope_res["foo1.scope"] = 1;
      scope_res["foo2.scope"] = 2;
      time = now.add_minutes (4);
      metrics.add_found_event ("456", server_sid1, scope_res, time);
      time = now.add_minutes (5);
      metrics.add_found_event ("711", server_sid2, scope_res, time);

      // verify parsed metrics
      var json = metrics.get_json ();
      var parser = new Json.Parser ();
      bool status = false;
      try
      {
        status = parser.load_from_data (json);
      }
      catch (Error e)
      {
        assert_not_reached ();
      }
      assert (status == true);
      var node = parser.get_root ();
      var event_array = node.get_array ();
      int num_click_events = 0;
      int num_preview_events = 0;
      int num_found_events = 0;
      int num_found_events_results = 0; // number of [scope, resultcount] results from all 'found' events
      event_array.foreach_element ((_array, _index, _node) =>
      {
        var ev_element = _node.get_object ();
        var ev_type = ev_element.get_string_member ("type");
        var session_id = ev_element.get_string_member ("session_id");
        var timestamp = ev_element.get_int_member ("timestamp");
        if (ev_type == "previewed")
        {
          var scope_id = ev_element.get_string_member ("previewed_scope");
          var server_sid = ev_element.get_string_member ("server_sid");
          if (scope_id == "foo1.scope")
          {
            ++num_preview_events;
            assert (session_id == "123");
            assert (server_sid == "abcdef");
            assert (timestamp == now.to_unix ());
          }
          else if (scope_id == "foo4.scope")
          {
            ++num_preview_events;
            assert (session_id == "981");
            assert (server_sid == "abcdef");
            assert (timestamp == now.to_unix () + 2*60);
          }
        }
        else if (ev_type == "clicked")
        {
          var scope_id = ev_element.get_string_member ("clicked_scope");
          if (scope_id == "foo2.scope")
          {
            ++num_click_events;
            assert (session_id == "123");
            assert (timestamp == now.to_unix () + 60);
            assert (ev_element.has_member ("server_sid") == false); //server_sid same as for foo1.scope with session 123, so optimzed out
          }
          if (scope_id == "foo5.scope")
          {
            ++num_click_events;
            assert (session_id == "888");
            assert (timestamp == now.to_unix () + 3*60);
            var server_sid = ev_element.get_string_member ("server_sid");
            assert (server_sid == "ghijkl");
          }
        }
        else if (ev_type == "found")
        {
          var results = ev_element.get_array_member ("results");
          var server_sid = ev_element.get_string_member ("server_sid");
          if (session_id == "456")
          {
            ++num_found_events;
            assert (server_sid == "abcdef");
            assert (timestamp == now.to_unix () + 4*60);
          }
          else if (session_id == "711")
          {
            ++num_found_events;
            assert (server_sid == "ghijkl");
            assert (timestamp == now.to_unix () + 5*60);
          }
          else
          {
            assert_not_reached ();
          }

          assert (results.get_length () == 2); // in this test each 'found' event has results for 2 scopes
          results.foreach_element ((_resarray, _resindex, _resnode) =>
          {
            var scope_res_arr = _resnode.get_array ();
            if (scope_res_arr.get_string_element (0) == "foo1.scope")
            {
              ++num_found_events_results;
              assert (scope_res_arr.get_int_element (1) == 1);
            }
            else if (scope_res_arr.get_string_element (0) == "foo2.scope")
            {
              ++num_found_events_results;
              assert (scope_res_arr.get_int_element (1) == 2);
            }
            else
            {
              assert_not_reached ();
            }
            });
          }
          else
          {
            assert_not_reached ();
          }
      });

      assert (num_preview_events == 2);
      assert (num_click_events == 2);
      assert (num_found_events == 2);
      assert (num_found_events_results == 4);

      assert (metrics.num_events == 6);
      metrics.clear_events ();
      assert (metrics.num_events == 0);
    }


    internal void test_client_scopes_info_from_data ()
    {
      var installed = new Gee.TreeSet<string> ();
      installed.add ("scope2.scope");
      installed.add ("scope3.scope");
      installed.add ("scope9.scope");

      string client_scopes = """{"unity-scope-a": ["scope1.scope","scope2.scope"], "unity-scope-b": ["scope3.scope"], "unity-scope-c": ["scope4.scope"]}""";
      var clinfo = ClientScopesInfo.from_data (client_scopes, installed);
      var added = clinfo.get_added_scopes ();
      var removed = clinfo.get_removed_scopes ();
      assert (added.size == 1);
      assert (added.contains ("scope9.scope"));
      assert (removed.size == 2);
      assert (removed.contains ("scope1.scope"));
      assert (removed.contains ("scope4.scope"));

      // check error on invalid json
      try
      {
        var clinfo2 = ClientScopesInfo.from_data (";;", installed);
        assert_not_reached ();
      }
      catch (Error e) {}
    }
    
    internal void test_client_scopes_info_from_file ()
    {
      var installed = new Gee.TreeSet<string> ();
      installed.add ("scope2.scope");
      installed.add ("scope4.scope");

      var clinfo = ClientScopesInfo.from_file (Config.TOPSRCDIR + "/tests/unit/data/unity/client-scopes.json", installed);
      var added = clinfo.get_added_scopes ();
      var removed = clinfo.get_removed_scopes ();

      assert (added.size == 1);
      assert (added.contains ("scope4.scope"));
      assert (removed.size == 2);
      assert (removed.contains ("scope1.scope"));
      assert (removed.contains ("scope3.scope"));
 
      // check error on missing file
      try
      {
        var clinfo4 = ClientScopesInfo.from_file ("/non/existing/file", installed);
        assert_not_reached ();
      }
      catch (Error e) {}
    }

    internal void test_channel_id_map ()
    {
      // ignore warnings
      Test.log_set_fatal_handler (() => { return false; });

      var ids = new SmartScopes.ChannelIdMap ();

      assert (ids.has_session_id_for_channel ("foo") == false);
      assert (ids.has_server_sid_for_channel ("foo") == false);

      assert (ids.map_server_sid ("channel3", "ssid2") == false); // it's an error to map server_sid before mapping session first

      ids.map_session_id ("channel1", "session1");
      ids.map_session_id ("channel2", "session2");
      assert (ids.has_session_id_for_channel ("channel1"));
      assert (ids.session_id_for_channel ("channel1") == "session1");
      assert (ids.has_server_sid_for_channel ("channel1") == false);
      assert (ids.server_sid_for_channel ("channel1") == null);
      assert (ids.map_server_sid ("channel1", "ssid1"));
      assert (ids.has_session_id_for_channel ("channel1"));
      assert (ids.has_server_sid_for_channel ("channel1"));
      assert (ids.server_sid_for_channel ("channel1") == "ssid1");

      ids.remove_channel ("channel1");
      assert (ids.has_session_id_for_channel ("channel1") == false);
      assert (ids.has_server_sid_for_channel ("channel1") == false);
      assert (ids.has_session_id_for_channel ("channel2"));
    }

    internal void test_platform_version ()
    {
      var rel = PlatformInfo.get_release_string ();
      assert (rel != null);
      // version info is dynamic, so make it convinient and possible to run these tests on various ubuntu releases
      assert (GLib.Regex.match_simple ("^\\d\\d\\.\\d\\d$", rel));
    }
  }

  class SmartScopesInterfaceTester: Object, Fixture
  {
  }

  class MarkupCleanerTester: Object, Fixture
  {
    internal void test_no_markup ()
    {
      string input = "This is a\ntest";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "This is a\ntest");
    }

    internal void test_br_tag_support ()
    {
      string input = "This is<br/> a<br />test<br>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "This is\n a\ntest\n");
    }

    internal void test_b_tag_support ()
    {
      string input = "<B>T</b>his <b>is</b> a <B>test</B>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "<b>T</b>his <b>is</b> a <b>test</b>");
    }

    internal  void test_i_tag_support ()
    {
      string input = "<I>T</i>his <i>is</i> a <I>test</I>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "<i>T</i>his <i>is</i> a <i>test</i>");
    }

    internal  void test_u_tag_support ()
    {
      string input = "<U>T</u>his <u>is</u> a <U>test</U>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "<u>T</u>his <u>is</u> a <u>test</u>");
    }

    internal void test_tt_tag_support ()
    {
      string input = "<TT>T</TT>his <tt>is</tt> a <tT>test</Tt>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "<tt>T</tt>his <tt>is</tt> a <tt>test</tt>");
    }

    internal void test_s_tag_support ()
    {
      string input = "<S>T</s>his <s>is</s> a <S>test</S>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "<s>T</s>his <s>is</s> a <s>test</s>");
    }

    internal void test_strike_tag_support ()
    {
      string input = "<STRIKE>T</STRIKE>his <strike>is</strike> a <STRike>test</Strike>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "<s>T</s>his <s>is</s> a <s>test</s>");
    }

    internal void test_small_tag_support ()
    {
      string input = "<SMALL>T</SMALL>his <small>is</small> a <SMall>test</SmaLL>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "<small>T</small>his <small>is</small> a <small>test</small>");
    }

    internal void test_big_tag_support ()
    {
      string input = "<BIG>T></BIG>his <big>is</big> a <bIG>test</BiG>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "<big>T></big>his <big>is</big> a <big>test</big>");
    }

    internal void test_sub_tag_support ()
    {
      string input = "<SUB>T</SUB>his <sub>is</sub> a <suB>test</SuB>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "<sub>T</sub>his <sub>is</sub> a <sub>test</sub>");
    }

    internal void test_sup_tag_support ()
    {
      string input = "<SUP>T</SUP>his <sup>is</sup> a <suP>test</SuP>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "<sup>T</sup>his <sup>is</sup> a <sup>test</sup>");
    }

    internal void test_unsupported_tags ()
    {
      string input = "<foo>This</bar> is a <a href=\"http://foo.com\">test</a>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "This is a test");
    }

    internal void test_nested_tags ()
    {
      string input = "<a href=\"wooo\"><small>Click me!</small></a>";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "<small>Click me!</small>");
    }

    internal void test_amp_entity ()
    {
      string input = "Foo & Bar";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "Foo &amp; Bar");
    }

    internal void test_nbsp_entity ()
    {
      string input = "Foo&nbsp;Bar";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "Foo Bar");
    }

    internal void test_basic_entities_are_preserved ()
    {
      string input = "Foo &amp; bar &lt; &GT; &Quot; &apos;";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "Foo &amp; bar &lt; &gt; &quot; &apos;");
    }

    internal void test_unsupported_entities_are_raw ()
    {
      string input = "Foo &frac14; bar &not;";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "Foo &amp;frac14 bar &amp;not");
    }

    internal void test_num_entities_are_preserved ()
    {
      string input = "Foo&#160;bar &#8364;";
      string result = MarkupCleaner.html_to_pango_markup (input);
      assert (result == "Foo&#160;bar &#8364;");
    }
  }
}
