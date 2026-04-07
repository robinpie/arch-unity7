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

    Test.add_data_func ("/Unit/Find", test_find);
    Test.add_data_func ("/Unit/CreatePkgSearchQuery", test_pkgsearch_query);

    Test.run ();
    return 0;
  }

  public Unity.Package.SearchResult create_search_result ()
  {
    Unity.Package.SearchResult search_results = new Unity.Package.SearchResult ();
    var pi = new Unity.Package.PackageInfo ();
    pi.application_name = "Evince";
    pi.package_name = "evince";
    pi.needs_purchase = false;
    search_results.results.append ((owned) pi);

    pi = new Unity.Package.PackageInfo ();
    pi.application_name = "Commercial App1";
    pi.package_name = "commercial_app1";
    pi.price = "10 EUR";
    pi.needs_purchase = true;
    search_results.results.append ((owned) pi);

    pi = new Unity.Package.PackageInfo ();
    pi.application_name = "Commercial App2";
    pi.package_name = "commercial_app2";
    pi.price = "20 EUR";
    pi.needs_purchase = false;
    search_results.results.append ((owned) pi);

    search_results.num_hits = 3;
    
    return search_results;
  }

  internal static void test_find ()
  {
    // populate search results
    var search_results = create_search_result ();

    var phelper = new PurchaseInfoHelper ();
    phelper.from_pkgresults (search_results);

    assert (phelper.find ("FooApp", "foopkg") == null);
    var appinfo = phelper.find ("Evince", "evince");
    assert (appinfo != null);
    assert (appinfo.formatted_price == "");
    assert (appinfo.paid == false);

    appinfo = phelper.find ("Commercial App1", "commercial_app1");
    assert (appinfo != null);
    assert (appinfo.formatted_price == "10 EUR");
    assert (appinfo.paid == false);

    appinfo = phelper.find ("Commercial App2", "commercial_app2");
    assert (appinfo != null);
    assert (appinfo.formatted_price == "20 EUR");
    assert (appinfo.paid == true);
  }

  internal static void test_pkgsearch_query ()
  {
    SoftwareCenterData.AppInfo?[] data = new SoftwareCenterData.AppInfo?[2] {
      SoftwareCenterData.AppInfo () { package_name = "evince", application_name = "Evince" },
      SoftwareCenterData.AppInfo () { package_name = "skype", application_name = "Skype" }
    };
    var phelper = new PurchaseInfoHelper ();
    var query = phelper.create_pkgsearch_query (data);
    
    assert (query.length () == 4);
    assert (query.nth_data (0) == "Evince");
    assert (query.nth_data (1) == "evince");
    assert (query.nth_data (2) == "Skype");
    assert (query.nth_data (3) == "skype");
  }
}