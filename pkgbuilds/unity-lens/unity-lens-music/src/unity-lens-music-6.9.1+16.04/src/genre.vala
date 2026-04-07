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

using Gee;

namespace Unity.MusicLens {
	
  public class Genre : GLib.Object
  {
    public static const string BLUES_ID = "blues";
    public static const string CLASSICAL_ID = "classical";
    public static const string COUNTRY_ID = "country";
    public static const string DISCO_ID = "disco";
    public static const string FUNK_ID = "funk";
    public static const string ROCK_ID = "rock";
    public static const string METAL_ID = "metal";
    public static const string HIPHOP_ID = "hip-hop";
    public static const string HOUSE_ID = "house";
    public static const string NEWWAVE_ID = "new-wave";
    public static const string RANDB_ID = "r-and-b";
    public static const string PUNK_ID = "punk";
    public static const string JAZZ_ID = "jazz";
    public static const string POP_ID = "pop";
    public static const string REGGAE_ID = "reggae";
    public static const string SOUL_ID = "soul";
    public static const string TECHNO_ID = "techno";
    public static const string OTHER_ID = "other";

    private static TreeMultiMap<string, string> map;
    private static Map<string, string> inverted_map;

    static construct
    {
      map = new TreeMultiMap<string, string> ();

      /* blues */
      map.set (BLUES_ID, "blues");

      /* classical */
      map.set (CLASSICAL_ID, "classic");
      map.set (CLASSICAL_ID, "classical");
      map.set (CLASSICAL_ID, "opera");

      /* country */
      map.set (COUNTRY_ID, "country");

      /* disco */
      map.set (DISCO_ID, "disco");

      /* funk */
      map.set (FUNK_ID, "funk");

      /* rock */
      map.set (ROCK_ID, "rock");
      map.set (ROCK_ID, "heavy");
      map.set (ROCK_ID, "hard");
      map.set (ROCK_ID, "rock and roll");

      /* metal */
      map.set (METAL_ID, "metal");
      map.set (METAL_ID, "heavy");
      map.set (METAL_ID, "heavy metal");

      /*hip hop */
      map.set (HIPHOP_ID, "hip-hop");
      map.set (HIPHOP_ID, "rap");
      map.set (HIPHOP_ID, "rap & hip hop");

      /*house*/
      map.set (HOUSE_ID, "house");
      map.set (HOUSE_ID, "chillout");
      map.set (HOUSE_ID, "minimal");
      map.set (HOUSE_ID, "hard");
      map.set (HOUSE_ID, "electronic");
      map.set (HOUSE_ID, "dance");

      /*new wave*/
      map.set (NEWWAVE_ID, "new-wave");

      /*r-and-b*/
      map.set (RANDB_ID, "r-and-b");
      map.set (RANDB_ID, "r&b");

      /*punk*/
      map.set (PUNK_ID, "punk");
      map.set (PUNK_ID, "punk rock");
      map.set (PUNK_ID, "hardcore");
      map.set (PUNK_ID, "heavy");

      /*jazz*/
      map.set (JAZZ_ID, "jazz");

      /*pop*/
      map.set (POP_ID, "pop");
      
      /*reggae*/
      map.set (REGGAE_ID, "reggae");

      /*soul*/
      map.set (SOUL_ID, "soul");
      map.set (SOUL_ID, "gospel");

      /*techno*/
      map.set (TECHNO_ID, "techno");
      map.set (TECHNO_ID, "minimal");
      map.set (TECHNO_ID, "trance");
      map.set (TECHNO_ID, "chillout");
      map.set (TECHNO_ID, "electronic");
      map.set (TECHNO_ID, "electronica");
      map.set (TECHNO_ID, "dance");

      /*other*/
      map.set (OTHER_ID, "other");
      map.set (OTHER_ID, "african");
      map.set (OTHER_ID, "alternative");
      map.set (OTHER_ID, "ambient");
      map.set (OTHER_ID, "asian");
      map.set (OTHER_ID, "brazilian");
      map.set (OTHER_ID, "celtic");
      map.set (OTHER_ID, "christmas");
      map.set (OTHER_ID, "folk");
      map.set (OTHER_ID, "latin");
      map.set (OTHER_ID, "oldies");
      map.set (OTHER_ID, "soundtrack");
      map.set (OTHER_ID, "traditional");
      map.set (OTHER_ID, "world");

      inverted_map = new HashMap<string, string> ();
      foreach (var key in map.get_keys ())
      {
        var values_collection = map[key];
        foreach (var val in values_collection)
        {
          inverted_map[val] = key;
        }
      }
    }

    public Collection<string> get_genre_synonyms (string genre_id)
    {
      if (map.contains (genre_id))
        return map.get (genre_id);
      
      return new LinkedList<string> ();
    }

    public string get_id_for_genre (string genre)
    {
      return inverted_map[genre] ?? OTHER_ID;
    }
  }
}

