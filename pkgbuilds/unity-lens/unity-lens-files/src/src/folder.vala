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
 * Authored by Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 *
 */

namespace Unity.FilesLens {

  public class Bookmarks : Object
  {

    private List<Bookmark> bookmarks;
    private string bookmarks_file;
    private FileMonitor monitor;

    public signal void changed ();

    public Bookmarks ()
    {
      bookmarks_file = @"$(Environment.get_home_dir())/.config/gtk-3.0/bookmarks";
      update();

      /* Update the bookmarks list whener the bookmarks file changes */
      try {
        monitor = File.new_for_path (bookmarks_file).monitor (FileMonitorFlags.NONE);
        monitor.set_rate_limit (2000); // max 1 update every 2s
        monitor.changed.connect ((mon, file, other_file, event_type) => {
          debug ("Bookmarks changed. Updating.");
          update ();
          changed ();
        });
      } catch (Error e) {
        warning ("Failed to install file monitor on %s. Bookmarks monitoring disabled: %s",
                 bookmarks_file, e.message);
      }
    }

    private void update ()
    {
      bookmarks = new List<Bookmark> ();
      string contents;
      string desktop_uri;
      bool has_desktop_in_favourites = false;

      File desktop_file = File.new_for_path (Environment.get_user_special_dir (UserDirectory.DESKTOP));
      desktop_uri = desktop_file.get_uri ();

      try {
        FileUtils.get_contents (bookmarks_file, out contents);

      } catch (FileError e) {
        warning ("Failed to read favorites: %s", e.message);
        return;
      }

      string[] favorites = contents.split ("\n");
      string mimetype = "inode/directory";

      foreach (var uri in favorites)
      {
        if (uri == "")
          continue;

        // Filter out useless bookmark that is created by Nautilus.
        // https://bugs.launchpad.net/unity-lens-files/+bug/1044309
        if (uri.has_prefix("x-nautilus-desktop:")) continue;

        if (uri == desktop_uri) has_desktop_in_favourites = true;

        string[] parts = uri.split (" ", 2);
        string display_name;

        if (parts.length == 1)
          {
            display_name = Uri.unescape_string (uri);
            display_name = Filename.display_basename (display_name);
          }
        else if (parts.length == 2)
          {
            uri = parts[0];
            display_name = parts[1];
          }
        else
          {
            warning ("Internal error computing display name for favorite '%s'",
                     uri);
            display_name = uri;
          }

        var bookmark = new Bookmark (uri, mimetype, display_name);
        bookmarks.append (bookmark);
      }

      /* Add desktop statically */
      if (!has_desktop_in_favourites)
      {
        var desktop_display_name = Path.get_basename (desktop_file.get_parse_name ());
        var desktop_bookmark = new Bookmark (desktop_uri, mimetype, desktop_display_name);
        bookmarks.append (desktop_bookmark);
      }
    }

    /* makes sure the uris exist on the filesystem (checks only native uris) */
    private GLib.List<Bookmark> filter_bookmarks (List<Bookmark> bm_list)
    {
      var result = new GLib.List<Bookmark> ();

      foreach (var bookmark in bm_list)
      {
        File f = File.new_for_uri (bookmark.uri);
        if (f.is_native () && !f.query_exists ()) continue;

        result.prepend (bookmark);
      }

      result.reverse ();
      return result;
    }
#if 0
    private async GLib.List<Bookmark> filter_bookmarks_async (owned GLib.List<Bookmark> bm_list)
    {
      var result = new GLib.List<Bookmark> ();

      foreach (var bookmark in bm_list)
      {
        File f = File.new_for_uri (bookmark.uri);
        if (f.is_native ())
        {
          bool exists;
          try
          {
            var info = yield f.query_info_async (FileAttribute.STANDARD_TYPE,
                                                 0, Priority.DEFAULT, null);
            exists = info.get_file_type () != FileType.UNKNOWN;
          }
          catch (Error err)
          {
            exists = false;
          }
          if (!exists)
            continue;
        }

        result.prepend (bookmark);
      }
      result.reverse ();
      return result;
    }
#endif

    public List<Bookmark> list ()
    {
      return filter_bookmarks (bookmarks);
    }

    public List<Bookmark> prefix_search (string search)
    {
      var prefix = Utils.normalize_string (search);
      var matches = new List<Bookmark> ();

      foreach (var bookmark in bookmarks)
      {
        foreach (var term in bookmark.get_index_terms ())
        {
          if (term.has_prefix (prefix))
            {
              /* Register a hit for this bookmark */
              matches.append (bookmark);
              break;
            }
        }
      }

      return filter_bookmarks (matches);
    }

    public bool launch_if_bookmark (string uri) throws Error
    {
      if (!uri.has_prefix ("bookmark:"))
        return false;

      var launcher = AppInfo.get_default_for_type ("inode/directory", true);

      uri = uri.offset (9); // Remove "bookmark:" prefix from uri

      if (launcher == null)
        {
          warning ("No default handler for inode/directory. Unable to open bookmark '%s'", uri);
          throw new IOError.FAILED ("No default handler for inode/directory. Unable to open bookmark '%s'", uri);
        }

      var uris = new List<string> ();
      uris.append (uri);

      launcher.launch_uris (uris, null);

      return true;
    }

  }

  public class Bookmark : Object
  {
    public string uri { get; set construct; }
    public string icon { get; set construct; }
    public string mimetype { get; set construct; }
    public string display_name { get; set construct; }
    public string dnd_uri { get; set construct; }

    private List<string> index_terms;

    public Bookmark (string uri, string mimetype, string display_name)
    {
      Object (uri:"bookmark:"+uri, icon:Utils.get_icon_for_uri (uri, mimetype),
              mimetype:mimetype, display_name:display_name, dnd_uri:uri);

      index_terms = new List<string> ();
      index_terms.append (Utils.normalize_string (Path.get_basename (uri)));
      index_terms.append (Utils.normalize_string (display_name));
    }

    public unowned List<string> get_index_terms ()
    {
      return index_terms;
    }

	public static bool is_bookmark_uri (string uri)
	{
	  return uri.has_prefix ("bookmark:");
	}

	public static string uri_from_bookmark (string uri)
	{
	  return uri.substring (9);
	}
  }


  public class Devices : Object
  {
    private List<Device> devices;
    private VolumeMonitor volume_monitor;

    public signal void changed ();

    public Devices ()
    {
      volume_monitor = VolumeMonitor.get ();

      update();

      volume_monitor.volume_added.connect ((mon, volume) => {
          update ();
          changed ();
       });

      volume_monitor.volume_removed.connect ((mon, volume) => {
          update ();
          changed ();
       });

      volume_monitor.volume_changed.connect ((mon, volume) => {
          update ();
          changed ();
       });
    }

    private void update ()
    {
      devices = new List<Device> ();

      foreach ( Volume volume in volume_monitor.get_volumes ())
      {
        var label = volume.get_identifier (VolumeIdentifier.LABEL);
        var uuid = volume.get_identifier (VolumeIdentifier.UUID);
  
        if ((label == null || label.length == 0) && (uuid == null || uuid.length == 0))
          continue;
 
        var device = new Device (volume);
        devices.append (device);
      }
    }

    public List<Device> list ()
    {
      var result = new GLib.List<Device> ();

      foreach (var device in devices)
      {
        result.append (device);
      }

      return result;
    }

    public List<Device> search (string search)
    {
      var normalized_search = Utils.normalize_string (search);
      var matches = new List<Device> ();

      foreach (var device in devices)
      {
        foreach (var term in device.get_index_terms ())
        {
          if (term.contains (normalized_search))
          {
            matches.append (device);
            break;
          }
        }
      }

      return matches;
    }

    public bool launch_if_device (string uri) throws Error
    {
      if (!Device.is_device_uri (uri))
        return false;

      foreach (var device in devices)
      {
        if (device.uri == uri)
        {
          device.mount_and_open ();
          return true;
        }
      }

      return false;
    }
 
    public Device? get_device_from_uri (string uri)
    {
      if (!Device.is_device_uri (uri))
        return null;

      foreach (var device in devices)
      {
        if (device.uri == uri)
          return device;
      }

      return null;
    }
  }


  public class Device : Object
  {
    public Volume volume { get; set construct; }
    public string name { get; set construct; }
    public string uri { get; set construct; }
    public Icon icon { get; set construct; }
    public string icon_name { get; set construct; }
    public string display_name { get; set construct; }
    public string dnd_uri { get; set construct; }

    private List<string> index_terms_;

    public Device (Volume volume)
    {
      var name = volume.get_name ();
      var icon = volume.get_icon ();
      var icon_name = icon.to_string ();
      var label = volume.get_identifier (VolumeIdentifier.LABEL) ?? "";
      var uuid = volume.get_identifier (VolumeIdentifier.UUID) ?? "";
      var id = "device://" + uuid + "-" + label;
 
      Object (volume:volume, name:name, uri:id, icon:icon,icon_name:icon_name, display_name:name, dnd_uri:id);

      index_terms_ = new List<string> ();
      index_terms_.append (Utils.normalize_string (name));
    }

    public unowned List<string> get_index_terms ()
    {
      return index_terms_;
    }

    public static bool is_device_uri (string uri)
    {
      return uri.has_prefix ("device://");
    }

    public void mount_and_open () throws Error
    {
      if (is_mounted ())
      {
        open_in_file_manager ();
      }
      else
      {
        volume.mount.begin (MountMountFlags.NONE, null, null, (obj, res) =>  {
          try {
            if (volume.mount.end (res))
              open_in_file_manager ();
          } catch (Error e) {
            warning ("Failed to mount %s: %s", display_name, e.message);
          }
        });
      } 
    }

    private bool is_mounted ()
    {
      var mount = volume.get_mount ();
      return mount != null;
    }

    private void open_in_file_manager () throws Error
    {
      if (!is_mounted ())
        return;

      AppInfo.launch_default_for_uri (get_volume_uri (), null);
    }

    private string? get_volume_uri ()
    {
      var root = get_root_file ();

      if (root == null)
        return null;

      return root.get_uri ();
    }

    public File? get_root_file ()
    {
      if (is_mounted())
      {
        var mount = volume.get_mount ();

        if (mount == null)
          return null;

        return mount.get_root ();
      }
      else
      {
        return volume.get_activation_root ();
      }
    }
  }

} /* end: namespace */
