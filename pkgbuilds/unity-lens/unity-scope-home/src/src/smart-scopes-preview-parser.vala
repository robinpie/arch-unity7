/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Authored by Pawel Stolowski <pawel.stolowski@canonical.com>
 *
 */

namespace Unity.HomeScope.SmartScopes {
  public class SmartScopesPreviewParser
  {
    private HashTable<string, PreviewJsonParser?> handlers = new HashTable<string, PreviewJsonParser?> (str_hash, str_equal);

    public SmartScopesPreviewParser ()
    {
      // preview-series is not used, so not supported here
      handlers["preview-generic"] = new PreviewGenericParser ();
      handlers["preview-music"] = new PreviewMusicParser ();
    }

    public Preview? parse (Json.Object obj)
    {
      unowned string renderer_name = obj.get_string_member ("renderer_name");
      var parser = handlers.lookup (renderer_name);
      if (parser != null)
      {
        return parser.parse (obj);
      }

      warning ("No handler for preview type %s", renderer_name);
      return handlers["preview-generic"].parse (obj);
    }
  }

  public interface PreviewJsonParser: Object
  {
    internal void get_base_attributes (Json.Object obj, out string title, out string subtitle, out string description, out GLib.Icon? icon, out string? attribution)
    {
      title = obj.get_string_member ("title");
      subtitle = obj.get_string_member ("subtitle");

      description = "";
      if (obj.has_member ("description_html"))
        description = MarkupCleaner.html_to_pango_markup (obj.get_string_member ("description_html"));

      if (description.length == 0 && obj.has_member ("description"))
        description = obj.get_string_member ("description");

      if (obj.has_member ("attribution"))
      {
        attribution = obj.get_string_member ("attribution");
        if (attribution != null && attribution != "")
          description += "\n──────────────\n<small>" + Markup.escape_text (attribution) + "</small>";
      }

      if (obj.has_member ("image_hint"))
      {
        var icon_uri = obj.get_string_member ("image_hint");
        var icon_file = File.new_for_uri (icon_uri);
        icon = new GLib.FileIcon (icon_file);
      }
      else
      {
        icon = null;
      }
    }

    internal void get_info_hints (Json.Object obj, Preview preview)
    {
      if (obj.has_member ("info"))
      {
        var info_array = obj.get_array_member ("info");

        info_array.foreach_element ((_array, _index, _node) =>
        {
          var info_obj = _node.get_object ();
          var info_hint_id = info_obj.get_string_member ("id");
          var info_icon_hint = info_obj.get_string_member ("icon_hint");
          var info_name = info_obj.get_string_member ("display_name");
          var info_value = info_obj.get_string_member ("value");
          
          GLib.Icon info_icon = null;
          if (info_icon_hint != null && info_icon_hint != "")
          {
            var icon_file = GLib.File.new_for_uri (info_icon_hint);
            info_icon = new GLib.FileIcon (icon_file);
          }
          
          var info = new Unity.InfoHint (info_hint_id, info_name, info_icon, info_value);

          preview.add_info (info);
        });
      }
    }

    internal void get_actions (Json.Object obj, Preview preview)
    {
      if (obj.has_member ("actions"))
      {
        var actions_array = obj.get_array_member ("actions");

        actions_array.foreach_element ((_array, _index, _node) =>
        {
          var action_obj = _node.get_object ();
          var action_icon_hint = action_obj.get_string_member ("icon_hint");
          var action_name = action_obj.get_string_member ("display_name");
          var action_uri = action_obj.get_string_member ("activation_uri");

          var action_icon_file = GLib.File.new_for_uri (action_icon_hint);
          var action_icon = new GLib.FileIcon (action_icon_file);
          
          // pass activation uri in action id; this is workaround for
          // https://bugs.launchpad.net/libunity/+bug/1243623 to intercept
          // action activation calls and have metrics for it;
          // PreviewAction.with_uri can't be used here because it's handled completly
          // in Unity and doesn't call into scope.
          var action = new Unity.PreviewAction (action_uri, action_name, action_icon);
          if (action_obj.has_member ("extra_text"))
            action.extra_text = action_obj.get_string_member ("extra_text");

          preview.add_action (action);
        });
      }
    }

    public abstract Preview? parse (Json.Object obj);
  }

  public class PreviewGenericParser: PreviewJsonParser, Object
  {
    public Preview? parse (Json.Object obj)
    {
      string title, subtitle, description;
      string? attribution;
      GLib.Icon? icon;

      get_base_attributes (obj, out title, out subtitle, out description, out icon, out attribution);
      var preview = new GenericPreview (title, description, icon);
      preview.subtitle = subtitle;
      get_actions (obj, preview);
      get_info_hints (obj, preview);

      return preview;
    }
  }

  public class PreviewMusicParser: PreviewJsonParser, Object
  {
    public Preview? parse (Json.Object obj)
    {
      string title, subtitle, description;
      string? attribution;
      GLib.Icon? icon;

      get_base_attributes (obj, out title, out subtitle, out description, out icon, out attribution);

      var preview = new MusicPreview (title, subtitle, icon);
      get_actions (obj, preview);
      get_info_hints (obj, preview);

      var tracks_array = obj.get_array_member ("tracks");
      tracks_array.foreach_element ((_array, _index, _node) =>
      {
        var track_obj = _node.get_object ();

        var track = new TrackMetadata ();
        track.uri = track_obj.get_string_member ("uri");
        track.title = track_obj.get_string_member ("title");
        track.track_no = (int)track_obj.get_int_member ("track_no");
        track.length = (uint)track_obj.get_int_member ("length");

        preview.add_track (track);       
      });

      return preview;
    }
  }
}
