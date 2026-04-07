/*
 * Copyright (C) 2010 Canonical Ltd
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
 * Authored by Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 *
 */

using GLib;
using Zeitgeist;
using Gee;
using Dee;

namespace Unity.FilesLens.Utils {

    const string icon_attribs = FileAttribute.PREVIEW_ICON + "," +
                                FileAttribute.STANDARD_ICON + "," +
                                FileAttribute.THUMBNAIL_PATH;

    const string file_attribs = FileAttribute.STANDARD_NAME + "," +
                                FileAttribute.STANDARD_DISPLAY_NAME + "," +
                                FileAttribute.STANDARD_CONTENT_TYPE + "," +
                                FileAttribute.STANDARD_IS_HIDDEN + "," +
                                FileAttribute.STANDARD_IS_BACKUP + "," +
                                FileAttribute.STANDARD_SIZE + "," +
                                FileAttribute.TIME_MODIFIED + "," +
                                FileAttribute.TIME_ACCESS;
    
    const string hide_attribs = FileAttribute.STANDARD_IS_BACKUP + "," +
                                FileAttribute.STANDARD_IS_HIDDEN;
    
    const string all_attribs = icon_attribs + "," + file_attribs + "," + hide_attribs;

	  /* Synchronous method to query GIO for a good icon for a uri/mimetype */
    public string get_icon_for_uri (string uri, string mimetype)
    {
      try {

        File f = File.new_for_uri (uri);
        
        /* Don't query icons or thumbnails for non-local files,
         * http:// and friends are *very* expensive */
        if (!f.is_native())
          return ContentType.get_icon(mimetype).to_string();
        
        FileInfo info = f.query_info (icon_attribs,
                                      FileQueryInfoFlags.NONE,
                                      null);
        return check_icon_string (uri, mimetype, info);
      } catch (GLib.Error e) {
        /* We failed to probe the icon. Try looking up by mimetype instead */
        Icon icon2 = ContentType.get_icon (mimetype);
        return icon2.to_string ();
      }
    }

    /* Async method to query GIO for a good icon for a uri/mimetype */
    public async string get_icon_for_uri_async (string uri, string mimetype)
    {
      try {

        File f = File.new_for_uri (uri);
        FileInfo info = yield f.query_info_async (icon_attribs,
                                                  FileQueryInfoFlags.NONE,
                                                  Priority.LOW,
                                                  null);
        return check_icon_string (uri, mimetype, info);
      } catch (GLib.Error e) {
        /* We failed to probe the icon. Try looking up by mimetype instead */
        Icon icon2 = ContentType.get_icon (mimetype);
        return icon2.to_string ();
      }
    }

    public string check_icon_string (string uri, string mimetype, FileInfo info)
    {
      Icon icon = info.get_icon ();
      string thumbnail_path = info.get_attribute_byte_string (FileAttribute.THUMBNAIL_PATH);

      if (thumbnail_path == null && icon != null)
        return icon.to_string ();
      else if (icon == null)
        return ContentType.get_icon (mimetype).to_string ();
      else
        return new FileIcon (File.new_for_path (thumbnail_path)).to_string ();
    }
    
    public int cmp_file_info_by_mtime (FileInfo info1, FileInfo info2)
    {
      TimeVal tv1, tv2;
      tv1 = info1.get_modification_time ();
      tv2 = info2.get_modification_time ();
      long cmp = tv1.tv_sec - tv2.tv_sec;
      return cmp < 0 ? 1 : ( cmp > 0 ? -1 : 0);
    }

    public string get_month_name (DateMonth month)
    {
      switch (month)
        {
          case DateMonth.BAD_MONTH:
            return _("Invalid Month");
          case DateMonth.JANUARY:
            return _("January");
          case DateMonth.FEBRUARY:
            return _("February");
          case DateMonth.MARCH:
            return _("March");
          case DateMonth.APRIL:
            return _("April");
          case DateMonth.MAY:
            return _("May");
          case DateMonth.JUNE:
            return _("June");
          case DateMonth.JULY:
            return _("July");
          case DateMonth.AUGUST:
            return _("August");
          case DateMonth.SEPTEMBER:
            return _("September");
          case DateMonth.OCTOBER:
            return _("October");
          case DateMonth.NOVEMBER:
            return _("November");
          case DateMonth.DECEMBER:
            return _("December");
        }
        
      return "Internal Error";
    }

    public string get_day_name (DateWeekday weekday)
    {
      switch (weekday)
        {
        case GLib.DateWeekday.MONDAY:
          return _("Monday");
        case GLib.DateWeekday.TUESDAY:
          return  _("Tuesday");
        case GLib.DateWeekday.WEDNESDAY:
          return _("Wednesday");
        case GLib.DateWeekday.THURSDAY:
          return _("Thursday");
        case GLib.DateWeekday.FRIDAY:
          return _("Friday");
        case GLib.DateWeekday.SATURDAY:
          return _("Saturday");
        case GLib.DateWeekday.SUNDAY:
          return _("Sunday");
        default:
          return "Internal Error";
        }
    }
    
#if 0
    /* Calculate the time group of the event returning the group id
     * of the group @event belongs to. It may be necessary for this method
     * to add a row to @groups_model if the group is one of the variadic
     * types such as */
    public uint get_time_group (Zeitgeist.Event event,
                                Dee.Model groups_model,
                                out string comment=null)
    {
      // FIXME: We need to calculate bounds of previous midnight, week, month etc
      //        This impl is just a quick hack to have something to show
      
      var t = event.get_timestamp ();
      var now = Timestamp.now ();
      var delta = now - t;
      
      /* Set limit to the previous midnight */
      var limit = Timestamp.prev_midnight (now);
      if (t > limit)
        {
          /* Fuzzy hour match */
          var hour = Timestamp.HOUR;

          if (delta > hour * 7)
            comment = _("Earlier today");
          else if (delta > hour * 6)
            comment = _("Five hours ago");
          else if (delta > hour * 5)
            comment = _("Four hours ago");
          else if (delta > hour * 4)
            comment = _("Three hours ago");
          else if (delta > hour * 3)
            comment = _("Two hours ago");
          else if (delta > hour * 2)
            comment = _("1 hour ago");
          else
            comment = _("Past hour");
          
          return Group.TODAY;
        }
      else if (t > limit - Zeitgeist.Timestamp.DAY)
        {
          comment = _("Yesterday");
          return Group.YESTERDAY;
        }
      
      /* Set limit to start of calendar week (monday) */
      var date = Date();
      Timestamp.to_date (now, out date);
      limit = limit - (date.get_weekday() - 1)*Timestamp.DAY;

      /* Try and give the name of the day */
      var datethen = Date ();
      Timestamp.to_date (t, out datethen);
      comment = get_day_name (datethen.get_weekday ());

      if (t > limit)
        return Group.THIS_WEEK;
      else if (t > limit - Zeitgeist.Timestamp.WEEK)
        return Group.LAST_WEEK;
      
      /* Set limit to start of current calendar month */
      date.set_day (1);
      limit = Timestamp.from_date (date);
      if (t > limit)
        {
          var days = datethen.days_between (date);

          /* Try and give a fuzzy weeks time */
          if (days < 7 * 3)
            comment = _("Three weeks ago");
          else if (days < 7 * 4)
            comment = _("A month ago");
          return Group.THIS_MONTH;
        }

      /* At this point let's just fall back to month name */
      comment = get_month_name (datethen.get_month ());
     
      /* Set limit to start of previous calendar month */
      date.subtract_months (1);
      limit = Timestamp.from_date (date);
      if (t > now - 6*30*Zeitgeist.Timestamp.DAY)
        {
          return Group.PAST_SIX_MONTHS;
        }
      
      /* Set limit to 1st pf January in the current year. We need to 
       * reset 'date' because date.subtract_months(1) above might have
       * turned a year */
      Timestamp.to_date (now, out date);
      date.set_month (DateMonth.JANUARY);
      date.set_day (1);
      limit = Timestamp.from_date (date);
      if (t > limit)
        {
          return Group.THIS_YEAR;
        }
      else
        return Group.LAST_YEAR;
    }
#endif
 
    private async SList<FileInfo> list_dir_internal (File folder,
        string? name_filter) throws Error
    {
      string? filter = null;
      if (name_filter != null) filter = name_filter.strip ().casefold ();

      var result = new SList<FileInfo> ();
      var e = yield folder.enumerate_children_async (Utils.all_attribs,
                                                     0, Priority.DEFAULT,
                                                     null);

      while (true) {
        var file_infos = yield e.next_files_async (10, Priority.DEFAULT,
                                                   null);
        if (file_infos == null)
          break;

        foreach (var info in file_infos) {
          if (info.get_is_hidden() || info.get_is_backup ())
            continue;
          if (filter != null &&
            info.get_display_name ().casefold ().index_of (filter) < 0)
            continue;
          var colkey = info.get_display_name().collate_key_for_filename ();
          info.set_attribute_string ("unity::collation-key", colkey);
          result.prepend (info);
        }
      }
      
      /* No need to reverse the result list even though we prepend to it,
       * we are going to re-sort it now anyway */
      CompareFunc cmpfunc = (info1, info2) => {
        return GLib.strcmp ((info1 as FileInfo).get_attribute_string("unity::collation-key"),
               (info2 as FileInfo).get_attribute_string("unity::collation-key"));
      };
      result.sort (cmpfunc);
      return result;
    }
    /**
     * Asynchronously list the contents of a directory given by a path,
     * and return a list of FileInfos filled with the attributes specified
     * by Utils.all_attribs
     */
    public async SList<FileInfo> list_dir (File folder) throws Error
    {
      return yield list_dir_internal (folder, null);
    }

    /**
     * Similar to list_dir, but only return list of FileInfos matching
     * name_filter name.
     */
    public async SList<FileInfo> list_dir_filtered (File folder,
        string name_filter) throws Error
    {
      return yield list_dir_internal (folder, name_filter);
    }

    public bool file_info_matches_any (FileInfo info, string[] types)
    {
      foreach (unowned string type_id in types)
      {
        if (file_info_matches_type (info, type_id)) return true;
      }
      return false;
    }

    public bool file_info_matches_type (FileInfo info, string type_id)
    {
      unowned string interpretation;
      unowned string content_type = info.get_content_type ();
      if (content_type == null)
        content_type = info.get_attribute_string (FileAttribute.STANDARD_FAST_CONTENT_TYPE);

      if (content_type == null) content_type = "application/octet-stream";

      interpretation = Zeitgeist.interpretation_for_mimetype (content_type);
      if (interpretation == null) return type_id == "other" || type_id == "all";

      /* type_id is one of: documents, folders, images, audio, videos,
       *   presentations, other */
      switch (type_id)
      {
        case "documents":
          return Zeitgeist.Symbol.is_a (interpretation, Zeitgeist.NFO.DOCUMENT)
            && !Zeitgeist.Symbol.is_a (interpretation, Zeitgeist.NFO.PRESENTATION);
        case "folders":
          return content_type == "inode/directory";
        case "images":
          return Zeitgeist.Symbol.is_a (interpretation, Zeitgeist.NFO.IMAGE);
        case "audio":
          return Zeitgeist.Symbol.is_a (interpretation, Zeitgeist.NFO.AUDIO);
        case "videos":
          return Zeitgeist.Symbol.is_a (interpretation, Zeitgeist.NFO.VIDEO);
        case "presentations":
          return Zeitgeist.Symbol.is_a (interpretation, Zeitgeist.NFO.PRESENTATION);
        case "other":
          bool is_recognized = false;
          is_recognized |= content_type == "inode/directory";
          // DOCUMENT includes PRESENTATION
          is_recognized |= Zeitgeist.Symbol.is_a (interpretation, Zeitgeist.NFO.DOCUMENT);
          // MEDIA includes AUDIO, VIDEO, IMAGE
          is_recognized |= Zeitgeist.Symbol.is_a (interpretation, Zeitgeist.NFO.MEDIA);
          return !is_recognized;
        case "all":
          return true;
        default:
          warning ("Unrecognized type: \"%s\"", type_id);
          return false;
      }
    }
    
    /* Extract a hash set of all subject uris in a Zeitgeist.ResultSet */
    public Set<string> get_uri_set (Zeitgeist.ResultSet results)
    {
      Set<string> uris = new HashSet<string> ();
      
      foreach (var ev in results)
        {
          for (int i = 0; i  < ev.num_subjects(); i++)
            uris.add (ev.get_subject (i).uri);
        }
      
      return uris;
    }
    
    public string normalize_string (string input)
    {
      return input.normalize (-1, NormalizeMode.ALL_COMPOSE).casefold ();
    }
   
} /* namespace */
