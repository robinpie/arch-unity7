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
 * based on python code by David Calle <davidc@framli.eu>
 */

namespace Unity.Utils
{
  public bool is_regular_file (string path)
  {
    var file = GLib.File.new_for_path (path);
    if (file.query_exists (null))
      return file.query_file_type (GLib.FileQueryInfoFlags.NONE, null) == GLib.FileType.REGULAR;
    return false;
  }

  public bool is_video (string path) throws Error
  {
    var file = GLib.File.new_for_path (path);
    if (file.query_exists (null))
    {
      if (file.query_file_type (GLib.FileQueryInfoFlags.NONE, null) == GLib.FileType.REGULAR)
      {
        var content_type = file.query_info ("standard::content-type", GLib.FileQueryInfoFlags.NONE, null).get_content_type ();
        return content_type.contains ("video");
      }
    }
    return false;
  }

  private bool is_hidden (string path) throws Error
  {
    var file = GLib.File.new_for_path (path);
    return file.query_info (GLib.FileAttribute.STANDARD_IS_HIDDEN, GLib.FileQueryInfoFlags.NONE, null).get_is_hidden ();
  }

  public string get_name (string path) throws Error
  {
    var file = GLib.File.new_for_path (path);
    var finfo = file.query_info (GLib.FileAttribute.STANDARD_DISPLAY_NAME, GLib.FileQueryInfoFlags.NONE, null);
    return finfo.get_attribute_as_string (GLib.FileAttribute.STANDARD_DISPLAY_NAME);
  }

  public uint gcd (uint a, uint b)
    requires (a > 0 && b > 0)
    ensures (result > 0)
  {
    for (;;)
    {
      if (a > b)
      {
        a = a % b;
        if (a == 0)
          return b;
      }
      else
      {
        b = b % a;
        if (b == 0)
          return a;
      }
    }
  }
}