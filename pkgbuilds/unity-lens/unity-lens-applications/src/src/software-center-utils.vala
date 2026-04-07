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
 */

namespace Unity.ApplicationsLens.SoftwareCenterUtils
{
  public class MangledDesktopFileLookup
  {
    /* Maps demangled names to mangled names expected by S-C xapian DB (e.g. kde4-KCharSelect.desktop -> kde4__KCharSelect.desktop).
       There are very few apps that need this and we only store mappings when needed, so it takes very little memory.
    */
    private HashTable<string, string> mangled_desktop_ids;

    public MangledDesktopFileLookup ()
    {
      mangled_desktop_ids = new HashTable<string, string>(str_hash, str_equal);
    }

    public bool contains (string desktop_file)
    {
      return mangled_desktop_ids.contains (desktop_file);
    }

    public string get (string desktop_file)
    {
      return mangled_desktop_ids[desktop_file];
    }

    public string extract_desktop_id (string? desktop_file,
                                       bool unmangle = false)
    {
      if (desktop_file == null) return "";

      string desktop_id = Path.get_basename (desktop_file);
      /* S-C uses "app_name:desktop_id.desktop", get rid of the prefix */
      int colon_pos = desktop_id.index_of (":");
      if (unmangle && colon_pos > 0)
      {
        desktop_id = desktop_id[colon_pos+1:desktop_id.length];
        /* well it's still not real desktop_id, S-C converts slashes to "__"
         * if the desktop file is in /usr/share/applications */
        string demangled_desktop_id = desktop_id.replace ("__", "-");

        // store demangled name -> mangled name mapping
        if (desktop_id != demangled_desktop_id)
        {
          mangled_desktop_ids.replace (demangled_desktop_id, desktop_id);
        }
        desktop_id = demangled_desktop_id;
      }

      return desktop_id;
    }

  }
}