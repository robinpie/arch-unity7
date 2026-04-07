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
using GLib;
using Sqlite;

namespace Unity.MusicLens {

  public errordomain DatabaseError {
    FAILED_TO_OPEN
  }

  /**
   * Abstracts talking to the banshee collection database
   */
  public class BansheeCollection : Object
  {
    private const int MAX_RESULTS = 100;

    private Database db;

    public BansheeCollection () throws DatabaseError
    {
      int rc = Database.open ("%s/banshee-1/banshee.db".printf (Environment.get_user_config_dir ()), out db);

      if (rc != Sqlite.OK) {
	printerr ("failed to open db, %d, %s\n", rc, db.errmsg ());
	throw new DatabaseError.FAILED_TO_OPEN ("Failed to open banshee database");
      }
    }

    /**
     * Performs a search on the banshee db
     */
    public void search (DeprecatedScopeSearch search,
                        SearchType search_type,
                        GLib.List<FilterParser>? filters = null,
                        int max_results = -1,
                        int category_override = -1)
    {
      const int TRACK_TITLE = 0;
      const int TRACK_URI = 1;
      const int TRACK_MIMETYPE = 2;
      const int ALBUM_TITLE = 3;
      const int ALBUM_ARTWORKID = 4;
      const int ARTIST_NAME = 5;

      int rc = 0;
      Statement stmt;

      var results_model = search.results_model;
      var helper_model = results_model;
      var empty_asv = new Variant.array (VariantType.VARDICT.element (), {});
      if (category_override >= 0)
      {
        helper_model = new Dee.SequenceModel ();
        helper_model.set_schema_full (results_model.get_schema ());
      }

      // use a tree set to ensure we don't duplicate albums
      TreeSet<string> albums = new TreeSet<string> ();
      string filters_sql = build_sql_from_filters (filters);

      // BUILD SQL STATEMENT
      string sql = """SELECT CoreTracks.Title, CoreTracks.Uri, CoreTracks.MimeType, CoreAlbums.Title, CoreAlbums.ArtworkID, CoreArtists.Name                      FROM CoreTracks 
                      CROSS JOIN CoreArtists, CoreAlbums 
                      WHERE CoreArtists.ArtistID = CoreTracks.ArtistID 
                                                   AND CoreAlbums.AlbumID = CoreTracks.AlbumID 
                                                   AND CoreTracks.PrimarySourceID = 1 
						   AND ((CoreArtists.NameLowered LIKE '%%%s%%' ESCAPE '\' 
						  	 AND CoreArtists.NameLowered IS NOT NULL) 
						        OR (CoreAlbums.TitleLowered LIKE '%%%s%%' ESCAPE '\' 
						  	    AND CoreAlbums.TitleLowered IS NOT NULL) 
						  	OR (CoreTracks.TitleLowered LIKE '%%%s%%' ESCAPE '\' 
						  	    AND CoreTracks.TitleLowered IS NOT NULL)
                                                       )
                                 %s
                      ORDER BY CoreTracks.Score DESC
                      LIMIT 0, %d;""".printf (search.search_string,
                                              search.search_string,
                                              search.search_string,
                                              filters_sql,
                                              max_results == -1 ? MAX_RESULTS : max_results);

      rc = execute_sql (sql, out stmt);
      if (stmt == null)
	return;

      do {
	rc = stmt.step ();
	switch (rc) {
	case Sqlite.DONE:
	  break;
	case Sqlite.ROW:
      string artwork_path = get_album_artwork_path (stmt.column_text (ALBUM_ARTWORKID));
      if (artwork_path == null || artwork_path == "" || !FileUtils.test(artwork_path, FileTest.EXISTS))
          artwork_path = ALBUM_MISSING_ICON_PATH;

      Track track = new Track ();
	  track.title = stmt.column_text (TRACK_TITLE);
	  track.artist = stmt.column_text (ARTIST_NAME);
	  track.uri = stmt.column_text (TRACK_URI);
	  track.mime_type = stmt.column_text (TRACK_MIMETYPE);
	  track.artwork_path = artwork_path;

	  Album album = new Album ();
	  album.title = stmt.column_text (ALBUM_TITLE);
	  album.artist = stmt.column_text (ARTIST_NAME);
	  album.uri = "album://%s/%s".printf (album.artist, album.title);
	  album.artwork_path = artwork_path;

	  uint category_id = Category.SONGS;
	  if (category_override >= 0)
	    category_id = category_override;

	  helper_model.append (track.uri, artwork_path, category_id,
                               Unity.ResultType.PERSONAL,
                               track.mime_type, track.title, track.artist,
                               track.uri, empty_asv);

	  if (albums.add (album.artist + album.title)) {
	    StringBuilder uri_list_builder = new StringBuilder ();
	    foreach (string uri in get_track_uris (album)) {
	      uri_list_builder.append ("'");
	      uri_list_builder.append (uri);
	      uri_list_builder.append ("' ");
	    }

            category_id = Category.ALBUMS;
            if (category_override >= 0)
              category_id = category_override;

            results_model.append (album.uri, album.artwork_path, category_id,
                                  Unity.ResultType.PERSONAL,
                                  "audio/mp3", album.title, album.artist,
                                  uri_list_builder.str, empty_asv);
	  }

	  break;
	default:
	  break;
	}
      } while (rc == Sqlite.ROW);

      if (helper_model == results_model) return;

      // we need to do this because the dash doesn't care about position
      // of a newly added row in the model - it just appends it
      var iter = helper_model.get_first_iter ();
      var last = helper_model.get_last_iter ();
      while (iter != last)
      {
        var row = helper_model.get_row (iter);
        results_model.append_row (row);
        iter = helper_model.next (iter);
      }
    }

    /**
     * Computes path for album artwork.
     */
    private string get_album_artwork_path (string artwork_id)
    {
        string album_art_dir = "%s/media-art/".printf (Environment.get_user_cache_dir ());
        string artwork_path = "%s/%s.jpg".printf (album_art_dir, artwork_id);
        return artwork_path;
    }

    /**
     * Creates Track object out of single row of Statement result data.
     */
    private Track get_track (Statement stmt)
    {
        const int TRACK_URI = 0;
        const int TRACK_TITLE = 1;
        const int ARTIST_NAME = 2;
        const int ALBUM_TITLE = 3;
        const int ALBUM_ARTWORKID = 4;
        const int TRACK_MIMETYPE = 5;
        const int TRACK_GENRE = 6;
        const int TRACK_NUMBER = 7;
        const int TRACK_YEAR = 8;
        const int TRACK_PLAYCOUNT = 9;
        const int TRACK_DURATION = 10;

        Track track = new Track();

        track.uri = stmt.column_text (TRACK_URI);
        track.title = stmt.column_text (TRACK_TITLE);
        track.artist = stmt.column_text (ARTIST_NAME);
        track.album = stmt.column_text (ALBUM_TITLE);
        track.artwork_path = get_album_artwork_path (stmt.column_text (ALBUM_ARTWORKID));
        track.mime_type = stmt.column_text (TRACK_MIMETYPE);
        track.genre = stmt.column_text (TRACK_GENRE);
        track.track_number = stmt.column_int (TRACK_NUMBER);
        track.year = stmt.column_int (TRACK_YEAR);
        track.play_count = stmt.column_int (TRACK_PLAYCOUNT);
        track.duration = stmt.column_int (TRACK_DURATION) / 1000;

        return track;
    }

    /**
     * Returns single track.
     */
    public Track? get_album_track (string uri)
    {
        string sql = "SELECT 
                         CoreTracks.Uri,
                         CoreTracks.Title,
                         CoreArtists.Name,
                         CoreAlbums.Title,
                         CoreAlbums.ArtworkID,
                         CoreTracks.MimeType,
                         CoreTracks.Genre,
                         CoreTracks.TrackNumber,
                         CoreTracks.Year,
                         CoreTracks.PlayCount,
                         CoreTracks.Duration
                    FROM CoreTracks
                    CROSS JOIN CoreAlbums, CoreArtists
                    WHERE CoreTracks.URI IS ?
                          AND CoreArtists.ArtistID = CoreTracks.ArtistID
                          AND CoreAlbums.AlbumID = CoreTracks.AlbumID";

        Statement stmt;
        int rc = execute_sql (sql, out stmt);
        stmt.bind_text (1, uri);
        rc = stmt.step ();
        if (rc == Sqlite.ROW)
        {
            return get_track (stmt);
        }
        return null;
    }

    /**
     * Returns all tracks of an album.
     */
    public SList<Track> get_album_tracks_detailed (string album_title, string album_artist)
    {
               string sql = "SELECT 
                         CoreTracks.Uri,
                         CoreTracks.Title,
                         CoreArtists.Name,
                         CoreAlbums.Title,
                         CoreAlbums.ArtworkID,
                         CoreTracks.MimeType,
                         CoreTracks.Genre,
                         CoreTracks.TrackNumber,
                         CoreTracks.Year,
                         CoreTracks.PlayCount,
                         CoreTracks.Duration
                    FROM CoreTracks
                    CROSS JOIN CoreAlbums, CoreArtists
                    WHERE CoreArtists.ArtistID = CoreTracks.ArtistID
                          AND CoreAlbums.AlbumID = CoreTracks.AlbumID
                          AND CoreAlbums.Title IS ?
                          AND CoreArtists.Name IS ?
                          AND CoreTracks.URI IS NOT NULL
                    ORDER BY CoreTracks.TrackNumber ASC";

        Statement stmt;
        int rc = execute_sql (sql, out stmt);
        stmt.bind_text (1, album_title);
        stmt.bind_text (2, album_artist);

        SList<Track> tracks = new SList<Track>();

        do {
            rc = stmt.step ();
            switch (rc) {
                case Sqlite.DONE:
                    break;
                case Sqlite.ROW:
                    tracks.append (get_track (stmt));
                    break;
                default:
                    break;
            }
        } while (rc == Sqlite.ROW);

        return tracks;
    }

    /**
     * returns an array like {uri://, uri://, ...}
     */
    public string[] get_track_uris (Album album)
    {
      const int URI_COLUMN = 0;

      int rc;
      Statement stmt;

      string sql = "SELECT CoreTracks.Uri 
                    FROM CoreTracks 
                    CROSS JOIN CoreAlbums, CoreArtists
                    WHERE CoreArtists.ArtistID = CoreTracks.ArtistID
                          AND CoreAlbums.AlbumID = CoreTracks.AlbumID
                          AND CoreAlbums.Title IS '%s'
                          AND CoreArtists.Name IS '%s'
                          AND CoreTracks.URI IS NOT NULL
                    ORDER BY CoreTracks.TrackNumber ASC".printf (album.title, album.artist);

      rc = execute_sql (sql, out stmt);

      ArrayList<string> uris = new ArrayList<string> ();

      do {
	rc = stmt.step ();
	switch (rc) {
	case Sqlite.DONE:
	  break;
	case Sqlite.ROW:
	  uris.add (stmt.column_text (URI_COLUMN));
	  break;
	default:
	  break;
	}
      } while (rc == Sqlite.ROW);

      return uris.to_array ();
    }

    /**
     * returns a string like "AND (Table.Column IS filter OR Table.OtherCol IS filter2) 
     * AND (Table.OtherColAgain IS AnotherFilter)" 
     */
    private string build_sql_from_filters (GLib.List<FilterParser> filters)
    {
      if (filters == null || filters.length () == 0)
	return "";

      var builder = new StringBuilder ();

      foreach (FilterParser parser in filters)
      {
	BansheeFilterParser bparser;
	if (parser is GenreFilterParser)
	  bparser = new BansheeGenreFilterParser (parser as GenreFilterParser);
	else if (parser is DecadeFilterParser)
	  bparser = new BansheeDecadeFilterParser (parser as DecadeFilterParser);
	else
	  {
	    warning ("Recieved an unimplemented filter type");
	    continue;
	  }

        string parsed = bparser.parse ();

        if (parsed == null || parsed == "")
          continue;

        builder.append (" AND ");
        builder.append (parsed);
      }

      builder.append (" ");
      return builder.str;
    }

    private int execute_sql (string sql, out Statement stmt)
    {
      int rc;
      debug ("preparing to execute sql %s\n", sql);

      if ((rc = db.prepare_v2 (sql, -1, out stmt, null)) == 1)
	{
	  warning ("SQL Error: %d, %s\n", rc, db.errmsg ());
        }

      return rc;
    }
  }
}
