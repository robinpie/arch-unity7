/* unity-package-search.vapi hand crafted by Mikkel Kamstrup Erlandsen */

[CCode (cprefix = "Unity", lower_case_cprefix = "unity_", cheader_filename = "unity-package-search.h")]
namespace Unity {

  [CCode (cprefix = "UnityPackage", lower_case_cprefix = "unity_package_", cheader_filename = "unity-package-search.h")]
  namespace Package {
    
    [CCode (cprefix = "UNITY_PACKAGE_SEARCHTYPE_", has_type_id = false, cheader_filename = "unity-package-search.h")]    
    public enum SearchType {
      PREFIX,
      EXACT
    }
    
    [CCode (cprefix = "UNITY_PACKAGE_SORT_", has_type_id = false, cheader_filename = "unity-package-search.h")]
    public enum Sort {
      BY_NAME,
      BY_RELEVANCY
    }
    
    public delegate bool AppFilterCallback (PackageInfo pkginfo);

    [Compact]
    [CCode (free_function = "unity_package_searcher_free", cheader_filename = "unity-package-search.h")]
    public class Searcher {
      [CCode (cname = "unity_package_searcher_new")]
      public Searcher ();
      [CCode (cname = "unity_package_searcher_new_for_menu")]
      public Searcher.for_menu(GMenu.Tree menu);
      [CCode (cname = "unity_package_searcher_new_for_scopes")]
      public Searcher.for_scopes(Unity.Protocol.ScopeRegistry scope_registry);
      public SearchResult search (string search_string, uint max_hits, Unity.Package.SearchType search_type, Unity.Package.Sort sort);
      public SearchResult get_random_apps (string? filter_query, uint n_apps);
      public SearchResult get_apps (string? filter_query, uint n_apps, AppFilterCallback cb);
      public SearchResult get_by_exact_names (GLib.SList<string> names);
      public PackageInfo? get_by_desktop_file (string desktop_file);
    }
    
    [Compact]
    [CCode (free_function = "unity_package_search_result_free", cheader_filename = "unity-package-search.h")]
    public class SearchResult {
      [CCode (cname="unity_package_search_result_new")]
      public SearchResult ();
      public GLib.SList<PackageInfo> results;
      public int num_hits;
      public bool fuzzy_search;
    }
    
    [Compact]
    [CCode (cname = "UnityPackageInfo", free_function = "unity_package_package_info_free", cheader_filename = "unity-package-search.h")]
    public class PackageInfo {
      public PackageInfo ();
      public string package_name;
      public string application_name;
      public string description;
      public string desktop_file;
      public string icon;
      public string price;
      public bool needs_purchase;
      public int relevancy;
      public bool is_master_scope;
    }
  }
}
