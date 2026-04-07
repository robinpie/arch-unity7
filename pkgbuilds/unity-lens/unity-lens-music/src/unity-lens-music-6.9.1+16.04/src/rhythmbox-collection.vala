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
 * Authored by David Calle <davidc@framli.eu>
 *             Michal Hruby <michal.hruby@canonical.com>
 *
 */

using Dee;
using Gee;

namespace Unity.MusicLens
{
    const string UNITY_ICON_PATH = Config.PKGDATADIR + "/icons";
    const string ALBUM_MISSING_ICON_PATH = UNITY_ICON_PATH + "/album_missing.png";
    const string ALBUM_MISSING_PREVIEW_ICON_PATH = UNITY_ICON_PATH + "/album_missing_preview.png";

    private enum Columns
    {
        TYPE,
        URI,
        TITLE,
        ARTIST,
        ALBUM,
        ARTWORK,
        MIMETYPE,
        GENRE,
        ALBUM_ARTIST,
        TRACK_NUMBER,
        YEAR,
        PLAY_COUNT,
        DURATION,

        N_COLUMNS
    }

    class RhythmboxCollection : Object
    {
        const string UNKNOWN_ALBUM = _("Unknown");

        SequenceModel all_tracks;
        ModelTag<int> album_art_tag;
        FilterModel tracks_by_play_count;
        HashTable<string, GenericArray<ModelIter>> album_to_tracks_map;

        TDB.Database album_art_tdb;
        FileMonitor tdb_monitor;
        int current_album_art_tag;

        HashTable<unowned string, Variant> variant_store;
        HashTable<int, Variant> int_variant_store;
        Variant row_buffer[13];

        Analyzer analyzer;
        Index? index;
        ICUTermFilter ascii_filter;

        string media_art_dir;

        public class XmlParser: Object
        {
          const MarkupParser parser =
          {
            start_tag,
            end_tag,
            process_text,
            null,
            null
          };

          // contains genre maps
          Genre genre = new Genre ();

          MarkupParseContext context;
          bool is_rhythmdb_xml = false;

          construct
          {
            context = new MarkupParseContext (parser, 0, this, null);
          }

          public bool parse (string content, size_t len) throws MarkupError
          {
            return context.parse (content, (ssize_t) len);
          }

          bool processing_track;
          Track current_track;
          int current_data = -1;

          private void start_tag (MarkupParseContext context, string name,
            [CCode (array_length = false, array_null_terminated = true)] string[] attr_names, [CCode (array_length = false, array_null_terminated = true)] string[] attr_values)
            throws MarkupError
          {
            if (!processing_track)
            {
              switch (name)
              {
                case "rhythmdb": is_rhythmdb_xml = true; break;
                case "entry":
                  string accepted_element_name = null;
                  for (int i = 0; attr_names[i] != null; i++)
                  {
                    if (attr_names[i] == "type" && (attr_values[i] == "song"
                      /*|| attr_values[i] == "iradio"*/)) // disable radio stations
                      accepted_element_name = attr_values[i];
                  }
                  if (accepted_element_name == null) return;
                  processing_track = true;
                  current_track = new Track ();
                  current_track.type_track = accepted_element_name == "song" ?
                                             TrackType.SONG : TrackType.RADIO;
                  break;
              }
            }
            else
            {
              switch (name)
              {
                case "location": current_data = Columns.URI; break;
                case "title": current_data = Columns.TITLE; break;
                case "duration": current_data = Columns.DURATION; break;
                case "artist": current_data = Columns.ARTIST; break;
                case "album": current_data = Columns.ALBUM; break;
                case "genre": current_data = Columns.GENRE; break;
                case "track-number": current_data = Columns.TRACK_NUMBER; break;
                case "play-count": current_data = Columns.PLAY_COUNT; break;
                case "date": current_data = Columns.YEAR; break;
                case "media-type": current_data = Columns.MIMETYPE; break;
                case "album-artist": current_data = Columns.ALBUM_ARTIST; break;
                default: current_data = -1; break;
              }
            }
          }

          public signal void track_info_ready (Track track);

          private void end_tag (MarkupParseContext content, string name)
            throws MarkupError
          {
            switch (name)
            {
              case "location":
              case "title":
              case "duration":
              case "artist":
              case "album":
              case "genre":
              case "track-number":
              case "play-count":
              case "date":
              case "media-type":
              case "album-artist":
                if (current_data >= 0) current_data = -1;
                break;
              case "hidden":
                if (processing_track) processing_track = false;
                break;
              case "entry":
                if (processing_track && current_track != null)
                {
                  track_info_ready (current_track);
                }
                processing_track = false;
                break;
            }
          }

          private void process_text (MarkupParseContext context,
                                     string text, size_t text_len)
            throws MarkupError
          {
            if (!processing_track || current_data < 0) return;
            switch (current_data)
            {
              case Columns.URI: current_track.uri = text; break;
              case Columns.TITLE: current_track.title = text; break;
              case Columns.ARTIST: current_track.artist = text; break;
              case Columns.ALBUM: current_track.album = text; break;
              case Columns.ALBUM_ARTIST: 
                current_track.album_artist = text;
                break;
              case Columns.GENRE:
                current_track.genre = genre.get_id_for_genre (text.down ());
                break;
              case Columns.MIMETYPE:
                current_track.mime_type = text;
                break;
              case Columns.YEAR:
                current_track.year = int.parse (text) / 365;
                break;
              case Columns.PLAY_COUNT:
                current_track.play_count = int.parse (text);
                break;
              case Columns.TRACK_NUMBER:
                current_track.track_number = int.parse (text);
                break;
              case Columns.DURATION:
                current_track.duration = int.parse (text);
                break;
            }
          }
        }

        construct
        {
          static_assert (13 == Columns.N_COLUMNS); // sync with row_buffer size
          media_art_dir = Path.build_filename (
              Environment.get_user_cache_dir (), "media-art");

          variant_store = new HashTable<unowned string, Variant> (str_hash,
                                                                  str_equal);
          int_variant_store = new HashTable<int, Variant> (direct_hash,
                                                           direct_equal);
          all_tracks = new SequenceModel ();
          // the columns correspond to the Columns enum
          all_tracks.set_schema ("i", "s", "s", "s", "s", "s", "s",
                                 "s", "s", "i", "i", "i", "i");
          assert (all_tracks.get_schema ().length == Columns.N_COLUMNS);
          album_art_tag = new ModelTag<int> (all_tracks);
          album_to_tracks_map =
            new HashTable<string, GenericArray<ModelIter>> (str_hash,
                                                            str_equal);

          var filter = Dee.Filter.new_sort ((row1, row2) =>
          {
            int a = row1[Columns.PLAY_COUNT].get_int32 ();
            int b = row2[Columns.PLAY_COUNT].get_int32 ();

            return b - a; // higher play count first
          });
          tracks_by_play_count = new FilterModel (all_tracks, filter);

          ascii_filter = new ICUTermFilter.ascii_folder ();
          analyzer = new TextAnalyzer ();
          analyzer.add_term_filter ((terms_in, terms_out) =>
          {
            foreach (unowned string term in terms_in)
            {
              var folded = ascii_filter.apply (term);
              terms_out.add_term (term);
              if (folded != term) terms_out.add_term (folded);
            }
          });
          initialize_index ();
        }

        private void initialize_index ()
        {
          var reader = ModelReader.new ((model, iter) =>
          {
            var s ="%s\n%s\n%s".printf (model.get_string (iter, Columns.TITLE),
                                        model.get_string (iter, Columns.ARTIST),
                                        model.get_string (iter, Columns.ALBUM));
            return s;
          });

          index = new TreeIndex (all_tracks, analyzer, reader);
        }

        private string? check_album_art_tdb (string artist, string album)
        {
          if (album_art_tdb == null) return null;

          uint8 null_helper[1] = { 0 };
          ByteArray byte_arr = new ByteArray ();
          byte_arr.append ("album".data);
          byte_arr.append (null_helper);
          byte_arr.append (album.data);
          byte_arr.append (null_helper);
          byte_arr.append ("artist".data);
          byte_arr.append (null_helper);
          byte_arr.append (artist.data);
          byte_arr.append (null_helper);

          TDB.Data key = TDB.NULL_DATA;
          key.data = byte_arr.data;
          var val = album_art_tdb.fetch (key);

          if (val.data != null)
          {
            Variant v = Variant.new_from_data<int> (new VariantType ("a{sv}"), val.data, false);
            var file_variant = v.lookup_value ("file", VariantType.STRING);
            if (file_variant != null)
            {
              return file_variant.get_string ();
            }
          }

          return null;
        }

        private string? get_albumart (Track track)
        {
            string filename;
            var artist = track.album_artist ?? track.artist;
            var album = track.album;

            var artist_norm = artist.normalize (-1, NormalizeMode.NFKD);
            var album_norm = album.normalize (-1, NormalizeMode.NFKD);

            filename = check_album_art_tdb (artist, album);
            if (filename != null)
            {
              filename = Path.build_filename (Environment.get_user_cache_dir (),
                                              "rhythmbox", "album-art",
                                              filename);

              if (FileUtils.test (filename, FileTest.EXISTS)) return filename;
            }

            var artist_md5 = Checksum.compute_for_string (ChecksumType.MD5,
                                                          artist_norm);
            var album_md5 = Checksum.compute_for_string (ChecksumType.MD5,
                                                         album_norm);

            filename = Path.build_filename (media_art_dir,
                "album-%s-%s".printf (artist_md5, album_md5));
            if (FileUtils.test (filename, FileTest.EXISTS)) return filename;

            var combined = "%s\t%s".printf (artist, album).normalize (-1, NormalizeMode.NFKD);
            filename = Path.build_filename (media_art_dir,
                "album-%s.jpg".printf (Checksum.compute_for_string (
                    ChecksumType.MD5, combined)));
            if (FileUtils.test (filename, FileTest.EXISTS)) return filename;

            if (track.uri.has_prefix ("file://"))
            {
              // Try Nautilus thumbnails
              try
              {
                File artwork_file = File.new_for_uri (track.uri);
                var info = artwork_file.query_info (FileAttribute.THUMBNAIL_PATH, 0, null);
                var thumbnail_path = info.get_attribute_string (FileAttribute.THUMBNAIL_PATH);
                if (thumbnail_path != null) return thumbnail_path;
              } catch {}
            }

            // Try covers folder
            string artwork = Path.build_filename (
                Environment.get_user_cache_dir (), "rhythmbox", "covers",
                "%s - %s.jpg".printf (track.artist, track.album));
            if (FileUtils.test (artwork, FileTest.EXISTS)) return artwork;

            return null;
        }

        public SList<unowned string> get_album_tracks (string album_key)
        {
          SList<unowned string> results = new SList<unowned string> ();

          var iter_arr = album_to_tracks_map[album_key];
          if (iter_arr != null)
          {
            for (int i = iter_arr.length - 1; i >= 0; i--)
            {
              results.prepend (all_tracks.get_string (iter_arr[i], Columns.URI));
            }
          }

          return results;
        }

        private Track get_track (ModelIter iter)
        {
            Track track = new Track();

            track.uri = all_tracks.get_string (iter, Columns.URI);
            track.title = all_tracks.get_string (iter, Columns.TITLE);
            track.artist = all_tracks.get_string (iter, Columns.ARTIST);
            track.album = all_tracks.get_string (iter, Columns.ALBUM);
            track.artwork_path = all_tracks.get_string (iter, Columns.ARTWORK);
            track.mime_type = all_tracks.get_string (iter, Columns.MIMETYPE);
            track.genre = all_tracks.get_string (iter, Columns.GENRE);
            track.album_artist = all_tracks.get_string (iter, Columns.ALBUM_ARTIST);
            track.track_number = all_tracks.get_int32 (iter, Columns.TRACK_NUMBER);
            track.year = all_tracks.get_int32 (iter, Columns.YEAR);
            track.play_count = all_tracks.get_int32 (iter, Columns.PLAY_COUNT);
            track.duration = all_tracks.get_int32 (iter, Columns.DURATION);

            return track;
        }

        public Track? get_album_track (string uri)
        {
            var iter = all_tracks.get_first_iter ();
            var end_iter = all_tracks.get_last_iter ();

            // FIXME: linear search, change to insert_sorted / find_sorted
            while (iter != end_iter)
            {
                if (all_tracks.get_string (iter, Columns.URI) == uri) {
                    return get_track (iter);
                }
                iter = all_tracks.next (iter);
            }
            return null;
        }

        public SList<Track> get_album_tracks_detailed (string album_key)
        {
          var results = new SList<Track> ();

          var iter_arr = album_to_tracks_map[album_key];
          if (iter_arr != null)
          {
            for (int i = iter_arr.length - 1; i >= 0; i--)
            {
                results.prepend (get_track (iter_arr[i]));
            }
          }

          return results;
        }

        private Variant cached_variant_for_string (string? input)
        {
          unowned string text = input != null ? input : "";
          Variant? v = variant_store[text];
          if (v != null) return v;

          v = new Variant.string (text);
          // key is owned by value... awesome right?
          variant_store[v.get_string ()] = v;
          return v;
        }

        private Variant cached_variant_for_int (int input)
        {
          Variant? v = int_variant_store[input];
          if (v != null) return v;

          v = new Variant.int32 (input);
          // let's not cache every random integer
          if (input < 128)
            int_variant_store[input] = v;
          return v;
        }

        private void prepare_row_buffer (Track track)
        {
          Variant type = cached_variant_for_int (track.type_track);
          Variant uri = new Variant.string (track.uri);
          Variant title = new Variant.string (track.title);
          Variant artist = cached_variant_for_string (track.artist);
          Variant album_artist = cached_variant_for_string (track.album_artist);
          Variant album = cached_variant_for_string (track.album);
          Variant mime_type = cached_variant_for_string (track.mime_type);
          Variant artwork = cached_variant_for_string (track.artwork_path);
          Variant genre = cached_variant_for_string (track.genre);
          Variant track_number = cached_variant_for_int (track.track_number);
          Variant year = cached_variant_for_int (track.year);
          Variant play_count = cached_variant_for_int (track.play_count);
          Variant duration = cached_variant_for_int (track.duration);

          row_buffer[0] = type;
          row_buffer[1] = uri;
          row_buffer[2] = title;
          row_buffer[3] = artist;
          row_buffer[4] = album;
          row_buffer[5] = artwork;
          row_buffer[6] = mime_type;
          row_buffer[7] = genre;
          row_buffer[8] = album_artist;
          row_buffer[9] = track_number;
          row_buffer[10] = year;
          row_buffer[11] = play_count;
          row_buffer[12] = duration;
        }

        public void parse_metadata_file (string path)
        {
          if (album_art_tdb != null) return;

          if (tdb_monitor == null)
          {
            var tdb_file = File.new_for_path (path);
            try
            {
              tdb_monitor = tdb_file.monitor (FileMonitorFlags.NONE);
              tdb_monitor.changed.connect (() =>
              {
                if (album_art_tdb == null) parse_metadata_file (path);
                else current_album_art_tag++;
              });
            }
            catch (Error err)
            {
              warning ("%s", err.message);
            }
          }

          var flags = TDB.OpenFlags.INCOMPATIBLE_HASH | TDB.OpenFlags.SEQNUM | TDB.OpenFlags.NOLOCK;
          album_art_tdb = new TDB.Database (path, 999, flags,
                                            Posix.O_RDONLY, 0600);
          if (album_art_tdb == null)
          {
            warning ("Unable to open album-art DB!");
            return;
          }

          /*
          album_art_tdb.traverse ((db, key, val) =>
          {
            var byte_arr = new ByteArray.sized ((uint) val.data_size);
            byte_arr.append (val.data);
            Variant v = Variant.new_from_data<ByteArray> (new VariantType ("a{sv}"), byte_arr.data, false, byte_arr);
            message ("value: %s", v.print (true));

            return 0;
          });
          */
        }

        public void parse_file (string path)
        {
          // this could be really expensive if the index was already built, so
          // we'll destroy it first
          index = null;
          all_tracks.clear ();
          initialize_index ();
          current_album_art_tag = 0;
          album_to_tracks_map.remove_all ();

          var parser = new XmlParser ();
          parser.track_info_ready.connect ((track) =>
          {
            // Get cover art
            string albumart = get_albumart (track);
            if (albumart != null)
              track.artwork_path = albumart;

            prepare_row_buffer (track);
            var iter = all_tracks.append_row (row_buffer);

            if (track.album == "" || track.album == UNKNOWN_ALBUM) return;
            var album_key = "%s - %s".printf (track.album, track.album_artist != null ? track.album_artist : track.artist);
            var arr = album_to_tracks_map[album_key];
            if (arr == null)
            {
              arr = new GenericArray<ModelIter> ();
              album_to_tracks_map[album_key] = arr;
            }
            arr.add ((owned) iter);
          });

          var file = File.new_for_path (path);

          try
          {
            var stream = file.read (null);
            uint8 buffer[65536];

            size_t bytes_read;
            while ((bytes_read = stream.read (buffer, null)) > 0)
            {
              parser.parse ((string) buffer, bytes_read);
            }
          }
          catch (Error err)
          {
            warning ("Error while parsing rhythmbox DB: %s", err.message);
          }

          GLib.List<unowned string> all_albums = album_to_tracks_map.get_keys ();
          foreach (unowned string s in all_albums)
          {
            album_to_tracks_map[s].sort_with_data ((a, b) =>
            {
              var trackno1 = all_tracks.get_int32 (a, Columns.TRACK_NUMBER);
              var trackno2 = all_tracks.get_int32 (b, Columns.TRACK_NUMBER);

              return trackno1 - trackno2;
            });
          }
        }

        private enum ResultType
        {
          ALBUM,
          SONG,
          RADIO
        }

        private void add_result (Model results_model, Model model,
                                 ModelIter iter, ResultType result_type,
                                 uint category_id)
        {
          // check for updated album art
          var tag = album_art_tag[model, iter];
          if (tag < current_album_art_tag)
          {
            unowned string album = model.get_string (iter, Columns.ALBUM);
            unowned string artist = model.get_string (iter,
                                                      Columns.ALBUM_ARTIST);
            if (artist == "")
              artist = model.get_string (iter, Columns.ARTIST);

            var album_art_string = check_album_art_tdb (artist, album);
            if (album_art_string != null && album_art_string != "")
            {
              string filename;
              filename = Path.build_filename (Environment.get_user_cache_dir (),
                                              "rhythmbox", "album-art",
                                              album_art_string);
              album_art_string = FileUtils.test (filename, FileTest.EXISTS) ?
                filename : ALBUM_MISSING_ICON_PATH;
            }
            else
            {
              album_art_string = ALBUM_MISSING_ICON_PATH;
            }

            if (album_art_string != model.get_string (iter, Columns.ARTWORK))
            {
              model.set_value (iter, Columns.ARTWORK,
                               cached_variant_for_string (album_art_string));
            }
            
            album_art_tag[model, iter] = current_album_art_tag;
          }

          var title_col = (result_type == ResultType.SONG
                           || result_type == ResultType.RADIO) ?
            Columns.TITLE : Columns.ALBUM;
          unowned string title = model.get_string (iter, title_col);
          var uri = model.get_string (iter, Columns.URI);
          var dnd_uri = model.get_string (iter, Columns.URI);
          if (result_type == ResultType.ALBUM)
          {
            if (title == "" || title == UNKNOWN_ALBUM) return;
            unowned string artist = model.get_string (iter,
                                                      Columns.ALBUM_ARTIST);
            if (artist == "")
              artist = model.get_string (iter, Columns.ARTIST);
            var album_key = "%s - %s".printf (title, artist);
            StringBuilder sb = new StringBuilder ();
            foreach (unowned string track_uri in get_album_tracks (album_key))
            {
              sb.append_printf ("%s\r\n", track_uri);
            }
            dnd_uri = (owned) sb.str;
            uri = "album://%s".printf (album_key);
          }

          var artwork = model.get_string (iter, Columns.ARTWORK);
          if (artwork == null || artwork == "")
            artwork = ALBUM_MISSING_ICON_PATH;
          var empty_asv = new Variant.array (VariantType.VARDICT.element (), {});
          results_model.append (uri,
                                artwork,
                                category_id,
                                Unity.ResultType.PERSONAL,
                                model.get_string (iter, Columns.MIMETYPE),
                                title,
                                model.get_string (iter, Columns.ARTIST),
                                dnd_uri,
                                empty_asv);
        }

        public void search (DeprecatedScopeSearch search,
                            SearchType search_type, 
                            GLib.List<FilterParser>? filters = null,
                            int max_results = -1,
                            int category_override = -1)
        {
            int num_results = 0;
            var empty_search = search.search_string.strip () == "";
            int min_year;
            int max_year;
            int category_id;
            ResultType result_type;

            Model model = all_tracks;
            get_decade_filter (filters, out min_year, out max_year);
            var active_genres = get_genre_filter (filters);

            // we need this to be able to sort the albums properly
            var helper_model = search.results_model;
            if (category_override >= 0)
            {
              helper_model = new Dee.SequenceModel ();
              helper_model.set_schema_full (search.results_model.get_schema ());
            }

            if (empty_search)
            {
                // display a couple of most played songs
                model = tracks_by_play_count;
                var iter = model.get_first_iter ();
                var end_iter = model.get_last_iter ();
                var albums_list_nosearch = new HashSet<string> ();

                while (iter != end_iter)
                {
                    int year = model.get_int32 (iter, Columns.YEAR);
                    unowned string genre = model.get_string (iter, Columns.GENRE);

                    // check filters
                    if (year < min_year || year > max_year)
                    {
                        iter = model.next (iter);
                        continue;
                    }
                    
                    // check filters
                    if (active_genres != null) {
                        if (!(genre in active_genres)) {
                            iter = model.next (iter);
                            continue;
                        }
                    }

                    if (model.get_int32 (iter, Columns.TYPE) == TrackType.SONG)
                    {
                      unowned string album = model.get_string (iter,
                                                               Columns.ALBUM);
                      // it's not first as in track #1, but first found from album
                      bool first_track_from_album = !(album in albums_list_nosearch);
                      albums_list_nosearch.add (album);
                    
                      if (first_track_from_album)
                      {
                          category_id = category_override >= 0 ?
                              category_override : Category.ALBUMS;
                          
                          add_result (search.results_model, model, iter,
                                      ResultType.ALBUM, category_id);
                      }

                      category_id = Category.SONGS;
                      result_type = ResultType.SONG;
                    }
                    else
                    {
                      category_id = Category.RADIOS;
                      result_type = ResultType.RADIO;
                    }
                    if (category_override >= 0)
                      category_id = category_override;

                    // Do not show radios in Dash because previews
                    // is not working yet. Re-enable once this bug is fixed:
                    // https://bugs.launchpad.net/unity-lens-music/+bug/1044325
                    if (category_id != Category.RADIOS) {
                      add_result (helper_model, model, iter,
                                  result_type, category_id);

                      num_results++;
                      if (max_results >= 0 && num_results >= max_results) break;
                    }
                    iter = model.next (iter);
                }
                return;
            }


            var term_list = Object.new (typeof (Dee.TermList)) as Dee.TermList;
            // search only the folded terms, FIXME: is that a good idea?
            analyzer.tokenize (ascii_filter.apply (search.search_string),
                               term_list);

            var matches = new Sequence<Dee.ModelIter> ();
            bool first_pass = true;
            foreach (unowned string term in term_list)
            {
                // FIXME: use PREFIX search only for the last term?
                var result_set = index.lookup (term, TermMatchFlag.PREFIX);

                CompareDataFunc<Dee.ModelIter> cmp_func = (a, b) =>
                {
                    return a == b ? 0 : ((void*) a > (void*) b ? 1 : -1);
                };

                // intersect the results (cause we want to AND the terms)
                var remaining = new Sequence<Dee.ModelIter> ();
                foreach (var item in result_set)
                {
                    if (first_pass)
                        matches.insert_sorted (item, cmp_func);
                    else if (matches.lookup (item, cmp_func) != null)
                        remaining.insert_sorted (item, cmp_func);
                }
                if (!first_pass) matches = (owned) remaining;
                // final result set empty already?
                if (matches.get_begin_iter () == matches.get_end_iter ()) break;

                first_pass = false;
            }

            // matches now contain iterators into the all_tracks model which
            // match the search string
            var seq_iter = matches.get_begin_iter ();
            var seq_end_iter = matches.get_end_iter ();
            
            var albums_list = new HashSet<string> ();
            while (seq_iter != seq_end_iter)
            {
                var model_iter = seq_iter.get ();
                int year = model.get_int32 (model_iter, Columns.YEAR);
                string genre = model.get_string (model_iter, Columns.GENRE);

                // check filters
                if (year < min_year || year > max_year)
                {
                    seq_iter = seq_iter.next ();
                    continue;
                }
                
                // check filters
                if (active_genres != null) {
                    bool genre_match = (genre in active_genres);
                    if (!genre_match) {
                        seq_iter = seq_iter.next ();
                        continue;
                    }
                }


                if (model.get_int32 (model_iter, Columns.TYPE) == TrackType.SONG)
                {
                  unowned string album = model.get_string (model_iter,
                                                           Columns.ALBUM);
                  // it's not first as in track #1, but first found from album
                  bool first_track_from_album = !(album in albums_list);
                  albums_list.add (album);
  
                  if (first_track_from_album)
                  {
                      category_id = category_override >= 0 ?
                          category_override : Category.ALBUMS;
  
                      add_result (search.results_model, model, model_iter,
                                  ResultType.ALBUM, category_id);
                  }

                  category_id = Category.SONGS;
                  result_type = ResultType.SONG;
                }
                else
                {
                  category_id = Category.RADIOS;
                  result_type = ResultType.RADIO;
                }
                if (category_override >= 0)
                  category_id = category_override;

                // Do not show radios in Dash because previews
                // is not working yet. Re-enable once this bug is fixed:
                // https://bugs.launchpad.net/unity-lens-music/+bug/1044325
                if (category_id != Category.RADIOS) {
                  add_result (helper_model, model, model_iter,
                              result_type, category_id);
                  num_results++;
                }
                if (max_results >= 0 && num_results >= max_results) break;

                seq_iter = seq_iter.next ();
            }

            if (helper_model == search.results_model) return;

            // we need to do this because the dash doesn't care about position
            // of a newly added rows in the model - it just appends it
            var iter = helper_model.get_first_iter ();
            var last = helper_model.get_last_iter ();
            while (iter != last)
            {
              var row = helper_model.get_row (iter);
              search.results_model.append_row (row);
              iter = helper_model.next (iter);
            }
        }

        private void get_decade_filter (GLib.List<FilterParser> filters,
                                        out int min_year, out int max_year)
        {
            Filter? filter = null;
            foreach (var parser in filters)
            {
                if (parser is DecadeFilterParser) filter = parser.filter;
            }

            if (filter == null || !filter.filtering)
            {
                min_year = 0;
                max_year = int.MAX;
                return;
            }

            var mrf = filter as MultiRangeFilter;
            min_year = int.parse (mrf.get_first_active ().id);
            // it's supposed to be a decade, so 2000-2009
            max_year = int.parse (mrf.get_last_active ().id) + 9;
        }
        
        private Set<string>? get_genre_filter (GLib.List<FilterParser> filters)
        {
            Filter? filter = null;
            foreach (var parser in filters)
            {
                if (parser is GenreFilterParser) filter = parser.filter;
            }
            if (filter == null || !filter.filtering)
            {
                return null;
            }

            var active_genres = new HashSet<string> ();
            var all_genres = filter as CheckOptionFilterCompact;
            foreach (FilterOption option in all_genres.options)
            {
                if (option.id == null || !option.active) continue;
                active_genres.add (option.id);
            }

            return active_genres;
        }
    }
}
