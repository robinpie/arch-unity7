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
 * Authored by Pawel Stolowski <pawel.stolowski@canonical.com>
 */

using Gst;
using Gst.PbUtils;

namespace PreviewPlayer
{
  public errordomain PreviewPlayerError
  {
    GST_INIT_FAILED
  }

  public class PreviewPlayer
  {
    // Track states; keep in sync with libunity - TrackState enum
    private enum TrackState
    {
      STOPPED,
      PLAYING,
      PAUSED
    }

    private static uint64 GST_STATE_QUERY_TIMEOUT = 1000000;

    // gstreamer - playbin plugin flags; run 'gst-inspect1.0 playbin' for more flags & description
    private static int GST_PLAYBIN_AUDIO = 0x02;
    private static int GST_PLAYBIN_SOFT_VOLUME = 0x10;

    private Gst.Pipeline gst_pipeline;
    private Gst.Element gst_playbin;
    private Gst.Element gst_sink;
    private Gst.Bus gst_bus;
    private uint timer_id;
    private string uri;
    private bool error;
    private bool update_registry = false;
    
    public signal void progress (string uri, uint32 state, double value);

    public PreviewPlayer () throws PreviewPlayerError
    {
      timer_id = 0;
      init_gst ();
    }

    internal void init_gst () throws PreviewPlayerError
    {
      gst_pipeline = new Gst.Pipeline ("preview-player-pipeline");
      gst_playbin = Gst.ElementFactory.make ("playbin", "playbin");
      gst_sink = Gst.ElementFactory.make ("pulsesink", "sink");
      if (gst_sink == null)
      {
        gst_sink = Gst.ElementFactory.make ("alsasink", "sink");
        if (gst_sink == null)
        {
          throw new PreviewPlayerError.GST_INIT_FAILED ("Can't create backend sink");
        }
      }
      
      gst_playbin.set_property ("audio-sink", gst_sink);
      gst_playbin.set_property ("flags", GST_PLAYBIN_AUDIO | GST_PLAYBIN_SOFT_VOLUME);
      gst_playbin.notify["source"].connect (playbin_setup);
      gst_pipeline.add (gst_playbin);

      gst_bus = gst_pipeline.get_bus ();
      gst_bus.add_watch (GLib.Priority.DEFAULT, gst_bus_message_cb);
    }

    internal void playbin_setup (GLib.Object source, GLib.ParamSpec spec)
    {
      GLib.Value source_element = GLib.Value (GLib.Type.from_name("GstElement"));
      gst_playbin.get_property ("source", ref source_element);
      Gst.Element obj = (Gst.Element)source_element.get_object();
    }

    internal bool gst_bus_message_cb (Gst.Bus bus, Gst.Message message)
    {
      switch (message.type)
      {
        case Gst.MessageType.EOS:
          gst_pipeline.set_state (Gst.State.READY);
          progress (uri, TrackState.PLAYING, 1.0f);
          break;
        case Gst.MessageType.ERROR:
          gst_pipeline.set_state (Gst.State.READY);
          error = true;
          break;
        case Gst.MessageType.ELEMENT:
          if (Gst.PbUtils.is_missing_plugin_message (message))
          {
            handle_missing_plugin.begin (message);
          }
          break;
        default:
          // there are more meesages gstreamer may send over bus, but we're not interested in them
          break;
      }
      return true;
    }

    private async void handle_missing_plugin (Gst.Message message)
    {
      var detail = Gst.PbUtils.missing_plugin_message_get_installer_detail (message);
      var descr = Gst.PbUtils.missing_plugin_message_get_description (message);
      warning ("Missing plugin: '%s': %s\n", descr, detail);

      string[] details = {detail};

      var status = Gst.PbUtils.install_plugins_async (details, null, codec_install_finished);
      if (status == Gst.PbUtils.InstallPluginsReturn.STARTED_OK)
      {
        var dash_proxy = new DashProxy ();
        try
        {
          yield dash_proxy.hide_dash ();
        }
        catch (Error e)
        {
          warning ("Failed to hide dash: %s", e.message);
        }
      }
      else
      {
        warning ("Failed to start codec installation");
      }
    }

    private void codec_install_finished (Gst.PbUtils.InstallPluginsReturn result)
    {
      debug ("Codec install returned: %d\n", result);

      if (result == Gst.PbUtils.InstallPluginsReturn.SUCCESS)
      {
        /* In theory we should get here after codec installation successfully finished and then calling Gst.update_registry() would
           pick the changes. It turns out we get SUCCESS before apt actually finishes installation... As a workaround set a flag
           and postpone registry update till next call to play(). Note that this may result in another codec installation popup for same codec
           if user requests playback again before installation ends.
        */
        update_registry = true;
      }
    }

    private void remove_progress_cb ()
    {
      if (timer_id > 0)
      {
        GLib.Source.remove(timer_id);
        timer_id = 0;
      }
    }

    private bool progress_report_cb ()
    {
      Gst.State current_state;
      Gst.State pending_state;

      var status = gst_pipeline.get_state (out current_state, out pending_state, GST_STATE_QUERY_TIMEOUT);
      
      if (status != Gst.StateChangeReturn.FAILURE)
      {
        if (current_state == Gst.State.PLAYING || pending_state == Gst.State.PLAYING)
        {
          progress (uri, TrackState.PLAYING, gst_progress ());
          return true;
        }
        if (current_state == Gst.State.PAUSED || pending_state == Gst.State.PAUSED)
        {
          return true;
        }
        // FIXME: set TrackState.ERROR if error=true (and STOPPED if false) when we have ERROR state supported by libunity & unity core
        progress (uri, TrackState.STOPPED, 0.0f);             
      }
      
      return false;
    }

    private double gst_progress ()
    {
      int64 pos;
      int64 dur;

      var fmt = Gst.Format.TIME;
      if (gst_pipeline.query_position (fmt, out pos))
      {
        if (gst_pipeline.query_duration (fmt, out dur))
        {
          if (dur > 0)
          {
            return pos / (double)dur;
          }
        }
      }
      return 0.0f;
    }

    private bool is_playing ()
    {
      Gst.State current_state;
      Gst.State pending_state;

      var status = gst_pipeline.get_state (out current_state, out pending_state, GST_STATE_QUERY_TIMEOUT);
      return status != Gst.StateChangeReturn.FAILURE && (current_state == Gst.State.PLAYING || pending_state == Gst.State.PLAYING);
    }

    public void play (string uri)
    {
      if (update_registry)
      {
        update_registry = false;
        debug ("Updating gstreamer plugin registry\n");
        Gst.update_registry ();
        debug ("Finished gstreamer plugin registry update\n");
      }

      // resume playback for same uri
      if (uri == this.uri)
      {
        Gst.State current_state;
        Gst.State pending_state;
      
        var status = gst_pipeline.get_state (out current_state, out pending_state, GST_STATE_QUERY_TIMEOUT);
        if (status != Gst.StateChangeReturn.FAILURE)
        {
          if (current_state == Gst.State.PAUSED || pending_state == Gst.State.PAUSED)
          {
            gst_pipeline.set_state (Gst.State.PLAYING);
            progress (uri, TrackState.PLAYING, gst_progress ());
            return;
          }
        }
      }

      // start new playback
      remove_progress_cb ();
      error = false;
      this.uri = uri;
      gst_pipeline.set_state (Gst.State.NULL);
      gst_playbin.set_property ("uri", uri);
      gst_pipeline.set_state (Gst.State.PLAYING);
      
      timer_id = GLib.Timeout.add_seconds (1, progress_report_cb);
      progress_report_cb();
    }

    public void stop ()
    {
      Gst.State current_state;
      Gst.State pending_state;

      var status = gst_pipeline.get_state (out current_state, out pending_state, GST_STATE_QUERY_TIMEOUT);
      if (status != Gst.StateChangeReturn.FAILURE)
      {
        if (current_state == Gst.State.PAUSED || current_state == Gst.State.PLAYING ||
            pending_state == Gst.State.PAUSED || pending_state == Gst.State.PLAYING)
        {
          gst_pipeline.set_state (Gst.State.READY);
          progress (uri, TrackState.STOPPED, 0.0f);
        }
      }
    }

    public void pause ()
    {
      if (is_playing())
      {
        gst_pipeline.set_state (Gst.State.PAUSED);
        progress (uri, TrackState.PAUSED, gst_progress ());
      }
    }

    public void resume ()
    {
      Gst.State current_state;
      Gst.State pending_state;
      
      var status = gst_pipeline.get_state (out current_state, out pending_state, GST_STATE_QUERY_TIMEOUT);
      if (status != Gst.StateChangeReturn.FAILURE)
      {
        if (current_state == Gst.State.PAUSED || pending_state == Gst.State.PAUSED)
        {
          gst_pipeline.set_state (Gst.State.PLAYING);
          progress (uri, TrackState.PLAYING, gst_progress ());
        }
      }
    }
    
    public void pause_resume()
    {
      Gst.State current_state;
      Gst.State pending_state;
      
      var status = gst_pipeline.get_state (out current_state, out pending_state, GST_STATE_QUERY_TIMEOUT);
      
      if (status != Gst.StateChangeReturn.FAILURE)
      {
        TrackState track_state = TrackState.STOPPED;
        Gst.State gst_state = Gst.State.NULL;
        
        if (current_state == Gst.State.PLAYING || pending_state == Gst.State.PLAYING)
        {
          track_state = TrackState.PAUSED;
          gst_state = Gst.State.PAUSED;
          gst_pipeline.set_state (Gst.State.PAUSED);
        }
        else if (current_state == Gst.State.PAUSED || pending_state == Gst.State.PAUSED)
        {
          track_state = TrackState.PLAYING;
          gst_state = Gst.State.PLAYING;
        }

        if (gst_state != Gst.State.NULL)
        {
          gst_pipeline.set_state (gst_state);
          progress (uri, track_state, gst_progress ());
        }
      }
    }
     
    public HashTable<string, Variant> get_video_file_props (string uri)
    {
      var props = new HashTable<string, Variant> (str_hash, str_equal);

      try
      {
        var discoverer = new Gst.PbUtils.Discoverer (2 * 1000000000); //2 seconds
        var discoverer_info = discoverer.discover_uri (uri);
        var video_streams = discoverer_info.get_video_streams ();

        // note: the container may have multiple video and audio streams, we just get properties of the first one
        if (video_streams != null && video_streams.length () > 0)
        {
          var vstream = video_streams.nth_data (0) as Gst.PbUtils.DiscovererVideoInfo;
          props.insert ("width", new GLib.Variant.uint32 (vstream.get_width ()));
          props.insert ("height", new GLib.Variant.uint32 (vstream.get_height ()));
          props.insert ("codec", new GLib.Variant.string (Gst.PbUtils.get_codec_description (vstream.get_caps ())));
        }
      }
      catch (GLib.Error e)
      {
        warning ("Failed to get video file properties for '%s': '%s'\n", uri, e.message);
      }

      return props;
    }
  }
}
