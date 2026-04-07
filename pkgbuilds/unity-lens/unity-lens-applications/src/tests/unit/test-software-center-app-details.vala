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

    Test.add_data_func ("/Unit/AppDetailsCtor", test_app_details_ctor);
    Test.add_data_func ("/Unit/AppDetailsPkgStates", test_app_details_pkg_states);

    Test.run ();
    return 0;
  }

  internal HashTable<string, Variant> create_sc_app_details_data ()
  {
    var data = new HashTable<string, Variant> (str_hash, str_equal);
    data.insert ("name", "Foo");
    data.insert ("summary", "Foobar application");
    data.insert ("description", "Completly useless application");
    data.insert ("version", "1.0");

    var bld = new GLib.VariantBuilder (new VariantType ("aa{sv}"));
    bld.open (new VariantType ("a{sv}"));
    bld.add ("{sv}", "large_image_url", new Variant.string ("http://foo-bar.org/screenshot.png"));
    bld.add ("{sv}", "small_image_url", new Variant.string ("http://foo-bar.org/screenshot_small.png"));
    bld.close ();
    data.insert ("screenshots", bld.end ());

    data.insert ("signing_key_id", "");
    data.insert ("region_requirements_satisfied", true);
    data.insert ("desktop_file", "foo.desktop");
    data.insert ("license", "GPL");
    data.insert ("icon_file_name", "foo.png");
    data.insert ("icon_url", "http://foo-bar.org/icon.png");
    data.insert ("price", "1 EUR");
    data.insert ("raw_price", "1");
    data.insert ("installation_date", "01-01-2012");
    data.insert ("website", "http://foo-bar.org");
    data.insert ("hardware_requirements", "PC");
    data.insert ("size", "10000");
    data.insert ("is_desktop_dependency", true);
    data.insert ("pkg_state", "installed");

    return data;
  }

  internal static void test_app_details_ctor ()
  {
    var data = create_sc_app_details_data ();
    var app_details = new SoftwareCenterData.AppDetailsData (data);
    assert (app_details.name == "Foo");
    assert (app_details.summary == "Foobar application");
    assert (app_details.description == "Completly useless application");
    assert (app_details.version == "1.0");
    assert (app_details.screenshot == "http://foo-bar.org/screenshot.png");
    assert (app_details.desktop_file == "foo.desktop");
    assert (app_details.license == "GPL");
    assert (app_details.icon == "foo.png");
    assert (app_details.icon_url == "http://foo-bar.org/icon.png");
    assert (app_details.price == "1 EUR");
    assert (app_details.raw_price == "1");
    assert (app_details.installation_date == "01-01-2012");
    assert (app_details.website == "http://foo-bar.org");
    assert (app_details.size == 10000);
    assert (app_details.hardware_requirements == "PC");
    assert (app_details.is_desktop_dependency == true);
    assert (app_details.pkg_state == SoftwareCenterData.PackageState.INSTALLED);
  }

  internal static void test_app_details_pkg_states ()
  {
    var data = create_sc_app_details_data ();
    var app_details = new SoftwareCenterData.AppDetailsData (data);
    assert (app_details.pkg_state == SoftwareCenterData.PackageState.INSTALLED);

    data.replace ("pkg_state", "uninstalled");
    app_details = new SoftwareCenterData.AppDetailsData (data);
    assert (app_details.pkg_state == SoftwareCenterData.PackageState.UNINSTALLED);

    data.replace ("pkg_state", "needs_purchase");
    app_details = new SoftwareCenterData.AppDetailsData (data);
    assert (app_details.pkg_state == SoftwareCenterData.PackageState.NEEDS_PURCHASE);

    data.replace ("pkg_state", "");
    app_details = new SoftwareCenterData.AppDetailsData (data);
    assert (app_details.pkg_state == SoftwareCenterData.PackageState.UNKNOWN);
  }
}