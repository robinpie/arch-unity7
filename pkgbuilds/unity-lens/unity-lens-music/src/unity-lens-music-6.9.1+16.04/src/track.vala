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

namespace Unity.MusicLens {
  
  public enum TrackType
  {
    SONG,
    RADIO
  }

  public class Track : GLib.Object
  {
    public TrackType type_track { get; set; }
    public string title { get; set; }
    public string uri { get; set; }
    public string artist { get; set; }
    public string mime_type { get; set; }
    public string artwork_path { get; set; }
    public string album { get; set; }
    public string album_artist { get; set; }
    public string genre { get; set; }
    public int track_number { get; set; }
    public int year { get; set; }
    public int play_count { get; set; }
    public int duration { get; set; }
  }
}
