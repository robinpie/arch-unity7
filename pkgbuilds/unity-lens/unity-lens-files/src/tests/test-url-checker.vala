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
 * Authored by Michal Hruby <michal.hruby@canonical.com>
 *
 */

using Unity.FilesLens;
using Assertions;

public class Main
{
  public static int main (string[] args)
  {
    Test.init (ref args);

    Test.add_data_func ("/Unit/UrlChecker/WebUris", test_web_uris);
    Test.add_data_func ("/Unit/UrlChecker/MountableUris", test_mountable_uris);

    Test.run ();

    return 0;
  }

  private static void test_web_uris ()
  {
    string result;
    var checker = new UrlChecker ();
    UrlType url_type;

    result = checker.check_url ("facebook.com", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.WEB);
    assert_cmpstr (result, CompareOperator.EQ, "http://facebook.com");

    result = checker.check_url ("google.co.uk", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.WEB);
    assert_cmpstr (result, CompareOperator.EQ, "http://google.co.uk");

    result = checker.check_url ("http://www.bbc.co.uk", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.WEB);
    assert_cmpstr (result, CompareOperator.EQ, "http://www.bbc.co.uk");

    result = checker.check_url ("https://launchpad.net", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.WEB);
    assert_cmpstr (result, CompareOperator.EQ, "https://launchpad.net");

    result = checker.check_url ("https://code.launchpad.net/unity-lens-files", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.WEB);
    assert_cmpstr (result, CompareOperator.EQ, "https://code.launchpad.net/unity-lens-files");

    result = checker.check_url ("google.com/?q=unity-lens-files", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.WEB);
    assert_cmpstr (result, CompareOperator.EQ, "http://google.com/?q=unity-lens-files");

    result = checker.check_url ("192.168.0.1", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.WEB);
    assert_cmpstr (result, CompareOperator.EQ, "http://192.168.0.1");

    result = checker.check_url ("daemon.vala", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.UNKNOWN);
    assert_cmpstr (result, CompareOperator.EQ, null);

    result = checker.check_url ("non-existing-tld.qv", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.UNKNOWN);
    assert_cmpstr (result, CompareOperator.EQ, null);
  }

  private static void test_mountable_uris ()
  {
    string result;
    var checker = new UrlChecker ();
    UrlType url_type;

    result = checker.check_url ("\\\\smb_share", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.MOUNTABLE);
    assert_cmpstr (result, CompareOperator.EQ, "smb://smb_share");

    result = checker.check_url ("smb://another_share", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.MOUNTABLE);
    assert_cmpstr (result, CompareOperator.EQ, "smb://another_share");

    result = checker.check_url ("ftp://mozilla.org", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.MOUNTABLE);
    assert_cmpstr (result, CompareOperator.EQ, "ftp://mozilla.org");

    result = checker.check_url ("ssh://a.server.somewhere", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.MOUNTABLE);
    assert_cmpstr (result, CompareOperator.EQ, "ssh://a.server.somewhere");

    result = checker.check_url ("sftp://secure.mozilla.org", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.MOUNTABLE);
    assert_cmpstr (result, CompareOperator.EQ, "sftp://secure.mozilla.org");

    result = checker.check_url ("dav://dav.share.co.uk", out url_type);
    assert_cmpint (url_type, CompareOperator.EQ, UrlType.MOUNTABLE);
    assert_cmpstr (result, CompareOperator.EQ, "dav://dav.share.co.uk");
  }
}
