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
 * Authored by Didier Roche <didrocks@ubuntu.com>
 *
 */

using Unity.MusicLens;
using Assertions;

public class Main
{
  public static int main (string[] args)
  {
    Test.init (ref args);

    Test.add_data_func ("/Unit/ParserChecker/Radios", test_radios);
    Test.add_data_func ("/Unit/ParserChecker/Songs", test_songs);
    Test.add_data_func ("/Unit/ParserChecker/LazyLoad", test_lazy_load);
    Test.add_data_func ("/Unit/ParserChecker/InvalidTag", test_invalid_tag);

    Test.run ();

    return 0;
  }

  /* FIXME: this test is broken - it passes if iradio tag is not parsed
   * because all checks are done in a callback that's never called in such case.
   */
  private static void test_radios ()
  {
    var parser = new RhythmboxCollection.XmlParser ();

    string collection_to_parse = """<?xml version="1.0" standalone="yes"?>
<rhythmdb version="1.8">
  <entry type="iradio">
    <title>Absolute Radio 80s (Modem)</title>
    <genre>80's</genre>
    <artist></artist>
    <album></album>
    <location>http://network.absoluteradio.co.uk/core/audio/ogg/live.pls?service=a8</location>
    <play-count>6</play-count>
    <last-played>1339693331</last-played>
    <bitrate>32</bitrate>
    <date>0</date>
    <media-type>application/octet-stream</media-type>
  </entry>
</rhythmdb>""";

    parser.track_info_ready.connect ((track) =>
    {
      assert_cmpstr (track.title, CompareOperator.EQ, "Absolute Radio 80s (Modem)");
      assert_cmpstr (track.genre, CompareOperator.EQ, "other");
      assert_cmpstr (track.artist, CompareOperator.EQ, "");
      assert_cmpstr (track.album, CompareOperator.EQ, "");
      assert_cmpstr (track.uri, CompareOperator.EQ, "http://network.absoluteradio.co.uk/core/audio/ogg/live.pls?service=a8");
      assert_cmpint (track.year, CompareOperator.EQ, 0);
      assert (track.type_track == TrackType.RADIO);
    });
    parser.parse(collection_to_parse, collection_to_parse.length);
  }


  private static void test_songs ()
  {
    var parser = new RhythmboxCollection.XmlParser ();

    string collection_to_parse = """<?xml version="1.0" standalone="yes"?>
<rhythmdb version="1.8">
  <entry type="song">
    <title>LA PASSION</title>
    <genre>Dance</genre>
    <artist>GIGI D'AGOSTINO</artist>
    <album>Loulou 007</album>
    <duration>228</duration>
    <file-size>3661842</file-size>
    <location>file:///home/moi/Gigi%20d'agostino%20Passion.mp3</location>
    <mtime>1338273042</mtime>
    <first-seen>1338536342</first-seen>
    <last-seen>1340378542</last-seen>
    <bitrate>128</bitrate>
    <date>730142</date>
    <media-type>audio/mpeg</media-type>
    <comment>http://www.danceparadise.ca.tc</comment>
  </entry>
</rhythmdb>""";

    parser.track_info_ready.connect ((track) =>
    {
      assert_cmpstr (track.title, CompareOperator.EQ, "LA PASSION");
      assert_cmpstr (track.genre, CompareOperator.EQ, "techno");
      assert_cmpstr (track.artist, CompareOperator.EQ, "GIGI D'AGOSTINO");
      assert_cmpstr (track.album, CompareOperator.EQ, "Loulou 007");
      assert_cmpstr (track.uri, CompareOperator.EQ, "file:///home/moi/Gigi%20d'agostino%20Passion.mp3");
      assert_cmpint (track.year, CompareOperator.EQ, 2000);
      assert_cmpstr (track.mime_type, CompareOperator.EQ, "audio/mpeg");
      assert (track.type_track == TrackType.SONG);
    });
    parser.parse(collection_to_parse, collection_to_parse.length);
  }


  private static void test_lazy_load ()
  {
    var parser = new RhythmboxCollection.XmlParser ();

    string collection_to_parse_chunk = """<?xml version="1.0" standalone="yes"?>
<rhythmdb version="1.8">
  <entry type="song">
    <title>LA PASSION</title>
    <genre>Dance</genre>
    <artist>GIGI D'AGOSTINO</artist>
""";
    parser.parse(collection_to_parse_chunk, collection_to_parse_chunk.length);

    collection_to_parse_chunk = """
    <album>Loulou 007</album>
    <duration>228</duration>
    <file-size>3661842</file-size>
    <location>file:///home/moi/Gigi%20d'agostino%20Passion.mp3</location>
    <mtime>1338273042</mtime>
    <first-seen>1338536342</first-seen>
    <last-seen>1340378542</last-seen>
    <bitrate>128</bitrate>
    <date>730142</date>
    <media-type>audio/mpeg</media-type>
    <comment>http://www.danceparadise.ca.tc</comment>
  </entry>""";
    parser.track_info_ready.connect ((track) =>
    {
      assert_cmpstr (track.title, CompareOperator.EQ, "LA PASSION");
      assert_cmpstr (track.album, CompareOperator.EQ, "Loulou 007");
      assert (track.type_track == TrackType.SONG);
    });  
    parser.parse(collection_to_parse_chunk, collection_to_parse_chunk.length);
  }


  private static void test_invalid_tag ()
  {
    var parser = new RhythmboxCollection.XmlParser ();

    string collection_to_parse_chunk = """<?xml version="1.0" standalone="yes"?>
<rhythmdb version="1.8">
  <entry type="song">
    <title>LA PASSION</title>
    <genre>Dance</genre>
  <entry type="song">
    <title>le poisson</title>
    <genre>Dance</genre>
  </entry>""";

    parser.track_info_ready.connect ((track) =>
    {
      assert_cmpstr (track.title, CompareOperator.EQ, "le poisson");
      assert (track.type_track == TrackType.SONG);
    });     
    parser.parse(collection_to_parse_chunk, collection_to_parse_chunk.length);
  }

}
