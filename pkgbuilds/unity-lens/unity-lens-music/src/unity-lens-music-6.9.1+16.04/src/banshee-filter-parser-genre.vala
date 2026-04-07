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
  
  public class BansheeGenreFilterParser : BansheeFilterParser, GenreFilterParser
  {

    public BansheeGenreFilterParser (GenreFilterParser parser)
    {
      base (parser.filter as CheckOptionFilterCompact);
    }
    
    public override string parse ()
    { 
      StringBuilder builder = new StringBuilder ();
      List<FilterOption> genres = get_all_selected_genres ();

      /* CoreTracks.Genre IS ___ OR CoreTracks.Genre IS ____ ... */
      foreach (FilterOption genre in genres)
      {
        string id = genre.id;
        if (genre.id == null)
          continue;

        foreach (string alt in map.get_genre_synonyms (id))
        {
          builder.append ("(CoreTracks.Genre LIKE '%");
          builder.append (alt);
          builder.append ("""%' ESCAPE '\' AND CoreTracks.Genre IS NOT NULL) OR """);
        }
      }
      builder.truncate (builder.len - (" OR ".length));
      builder.prepend ("(");
      builder.append (")");
     
      return builder.str;
    }
  }
}
