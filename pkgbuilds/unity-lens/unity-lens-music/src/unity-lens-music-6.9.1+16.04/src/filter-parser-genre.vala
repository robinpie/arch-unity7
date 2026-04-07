/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Authored by Alex Launi <alex.launi@canonical.com>
 *
 */

using GLib;

namespace Unity.MusicLens {
  
  public class GenreFilterParser : FilterParser
  {
    public GenreFilterParser (CheckOptionFilterCompact filter)
    {
      base (filter);
      map = new Genre ();
    }
   
    public override string parse ()
    {
      return "";
    }

    protected override string id { get { return "genre"; } }
    protected Genre map { get; private set; }

    protected List<FilterOption> get_all_selected_genres ()
    {
      unowned List<FilterOption> options = (filter as CheckOptionFilterCompact).options;
      var active = new List<FilterOption> ();

      foreach (FilterOption option in options)
	{
        if (option.active)
          active.append (option);
      }
      
      return active;
    }
  }
}
