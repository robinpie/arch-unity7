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
  
  public class BansheeDecadeFilterParser : BansheeFilterParser, DecadeFilterParser
  {

    public BansheeDecadeFilterParser (DecadeFilterParser parser)
    {
      base (parser.filter as MultiRangeFilter);
    }
    
    public override string parse ()
    {   
      int start_year, end_year;

      MultiRangeFilter range_filter = filter as MultiRangeFilter;
      FilterOption start = range_filter.get_first_active ();
      FilterOption end = range_filter.get_last_active ();

      start_year = int.parse (start.id);
      end_year = int.parse (end.id);
      
      // bump the end_year up to the beginning of the next range
      int end_index = range_filter.options.index (end);
      // if we have already selected the last option
      if (end_index == range_filter.options.length () - 1)
        end_year += 10;
      else
        end_year = int.parse (range_filter.options.nth_data (end_index + 1).id);

      return "(CoreTracks.Year >= %d AND CoreTracks.Year < %d) ".printf (start_year, end_year);
    }
  }
}