/*
 * Copyright (C) 2010-2012 Canonical Ltd
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
using Zeitgeist;
using Config;
using Gee;

namespace Unity.FilesLens.Locate
{
  const uint MAX_RESULTS = 128;

  /* Careful about usage of FileAttribute.STANDARD_ICON, this is the same as
   * requesting FileAttribute.STANDARD_CONTENT_TYPE and causes reading
   * of the target file, we just want to stat it */
  const string ATTRIBS = FileAttribute.STANDARD_IS_HIDDEN + "," +
      FileAttribute.STANDARD_IS_BACKUP + "," +
      FileAttribute.STANDARD_DISPLAY_NAME + "," +
      FileAttribute.STANDARD_SIZE + "," +
      FileAttribute.TIME_MODIFIED + "," +
      FileAttribute.THUMBNAIL_PATH + "," +
      FileAttribute.ACCESS_CAN_WRITE + "," +
      FileAttribute.STANDARD_FAST_CONTENT_TYPE;

  private BlacklistTracker blacklist_tracker;

  public async GLib.SList<FileInfo> locate (string input_query,
                                            GLib.Cancellable cancellable) throws Error
  {
    if (blacklist_tracker == null) blacklist_tracker = new BlacklistTracker ();
    var result = new GLib.SList<FileInfo> ();
    var query = Regex.escape_string (input_query.strip ());

    // FIXME: we could limit the search to specific directories, but using regex
    //  matching slows down the search considerably
    string[] argv = { "locate", "-i", "-e", "-l", "%u".printf (MAX_RESULTS),
                      "*%s*".printf (query.replace (" ", "*")) };

    Pid pid;
    int read_fd;
    string? line = null;

    Process.spawn_async_with_pipes (null, argv, null, SpawnFlags.SEARCH_PATH,
                                    null, out pid, null, out read_fd);

    UnixInputStream read_stream = new UnixInputStream (read_fd, true);
    DataInputStream locate_output = new DataInputStream (read_stream);

    do
    {
      line = yield locate_output.read_line_async (Priority.DEFAULT_IDLE,
                                                  cancellable);
      if (line == null) break;
      if ("/." in line) continue; // we don't want no hidden files

      var file = File.new_for_path (line);
      var uri = file.get_uri ();

      bool skip = false;
      foreach (var blacklisted_uri in blacklist_tracker.get_blacklisted_uris ())
      {
        if (uri.has_prefix (blacklisted_uri))
        {
          skip = true;
          break;
        }
      }
      if (skip) continue;

      var fi = yield file.query_info_async (ATTRIBS, 0, Priority.DEFAULT_IDLE,
                                            cancellable);
      if (fi.get_is_hidden () || fi.get_is_backup () ||
          !fi.get_attribute_boolean (FileAttribute.ACCESS_CAN_WRITE)) continue;
      fi.set_data ("associated-gfile", file);
      result.prepend (fi);

    } while (line != null);

    result.reverse ();
    return result;
  }

} /* namespace */

