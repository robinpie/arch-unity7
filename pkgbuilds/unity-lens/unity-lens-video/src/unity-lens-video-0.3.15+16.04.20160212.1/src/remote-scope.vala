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
 * based on python code by David Calle <davidc@framli.eu>
 *
 */

using Config;

namespace Unity.VideoLens
{
  public class RemoteVideoScope : Unity.SimpleScope
  {
    private const int REFRESH_INTERVAL = 3600; // fetch sources & recommendations once an hour
    private const int RETRY_INTERVAL = 60;     // retry sources/recommendations after a minute

    private const string RESULT_PREVIEW_ON_LMB = "lmb-preview";
    private const string RESULT_DETAILS_URI = "details-uri";

    private Soup.Session session;
    Gee.ArrayList<RemoteVideoFile?> recommendations;
    int64 recommendations_last_update = 0;
    Zeitgeist.DataSourceRegistry zg_sources;
    bool use_zeitgeist;

    public RemoteVideoScope ()
    {
        Object (unique_name: "/net/launchpad/scope/remotevideos",
                group_name: "net.launchpad.scope.RemoteVideos");
    }

    protected override void constructed ()
    {
      base.constructed ();

      var schema = new Unity.Schema ();
      schema.add_field (RESULT_PREVIEW_ON_LMB, "b", Unity.Schema.FieldType.OPTIONAL);
      schema.add_field (RESULT_DETAILS_URI, "s", Unity.Schema.FieldType.OPTIONAL);
      this.schema = schema;

      recommendations = new Gee.ArrayList<RemoteVideoFile?> ();

      use_zeitgeist = false;
      try
      {
        zeitgeist_init ();
        use_zeitgeist = true;
      }
      catch (Error e)
      {
        warning ("Failed to initialize Zeitgeist, won't store events");
      }

      session = new Soup.SessionAsync ();
      session.ssl_use_system_ca_file = true;
      session.ssl_strict = true;
      session.user_agent = "Unity Video Lens Remote Scope v" + Config.VERSION;
      session.add_feature_by_type (typeof (SoupGNOME.ProxyResolverGNOME));

      populate_categories ();

      // refresh the search at least once every 30 minutes
      GLib.Timeout.add_seconds (REFRESH_INTERVAL/2, () =>
      {
        results_invalidated (SearchType.DEFAULT);
        return true;
      });

      set_search_async_func (dispatch_search);
      set_preview_async_func (dispatch_preview);
      set_activate_func (activate_result);
    }

    private void populate_categories ()
    {
      var categories = new Unity.CategorySet ();
      var icon_dir = File.new_for_path (Config.ICON_PATH);

      categories.add(new Unity.Category ("online", _("Online"), new FileIcon (icon_dir.get_child ("group-internet.svg")), Unity.CategoryRenderer.VERTICAL_TILE));
      categories.add(new Unity.Category ("more", _("More suggestions"), new FileIcon (icon_dir.get_child ("group-treat-yourself.svg")), Unity.CategoryRenderer.VERTICAL_TILE));

      this.category_set = categories;
    }

    private void dispatch_search (ScopeSearchBase search,
                                  ScopeSearchBaseCallback cb)
    {
      perform_search.begin (search, (obj, res) =>
      {
        try
        {
          perform_search.end (res);
        }
        catch (Error err)
        {
          warning ("Unable to perform search: %s", err.message);
        }
        cb (search);
      });
    }

    private void dispatch_preview (ResultPreviewer previewer,
                                   AbstractPreviewCallback cb)
    {
      preview_result.begin (previewer.result, (obj, res) =>
      {
        var preview = preview_result.end (res);
        cb (previewer, preview);
      });
    }

    public override string normalize_search_query (string search_query)
    {
      return search_query.strip ();
    }

    /* Query the server for a list of sources that will be used
     * to build sources filter options and search queries.
     */
    private async void query_list_of_sources ()
    {
      try
      {
        var sources_msg = new Soup.Message ("GET", UbuntuVideoSearch.sources_uri ());
        var msg = yield queue_soup_message (sources_msg);
        var sources_array = UbuntuVideoSearch.process_sources_results ((string) msg.response_body.data);
        // FIXME: do something with the sources once we support dynamic filters
      }
      catch (Error e)
      {
        warning ("Got invalid json from the server");
      }

      // refresh the sources after a while
      GLib.Timeout.add_seconds (REFRESH_INTERVAL, () =>
      {
        query_list_of_sources.begin (); return false;
      });
    }

    public Unity.ActivationResponse? activate_result (ScopeResult result,
                                                      SearchMetadata metadata,
                                                      string? action_id)
    {
      var realcat = result.metadata.lookup (RESULT_PREVIEW_ON_LMB);
      // activation of More Suggestions should display a preview.
      if (action_id == null && realcat != null && realcat.get_boolean ())
      {
        return new Unity.ActivationResponse (Unity.HandledType.SHOW_PREVIEW);
      }

      return on_activate_uri (result);
    }

    private Unity.ActivationResponse on_activate_uri (Unity.ScopeResult result)
    {
      zeitgeist_insert_event (result.uri, result.title, result.icon_hint);
      // let unity open the uri for us
      return new Unity.ActivationResponse (Unity.HandledType.NOT_HANDLED);
    }

    private Unity.Preview? build_preview (ScopeResult result, RemoteVideoDetails? details)
    {
      string title = result.title;
      string subtitle = "";
      string description = "";

      if (details != null)
      {
        title = details.title;
        description = details.description;

        if (details.release_date != null && details.release_date != "")
          subtitle = details.release_date;

        if (details.duration > 0)
        {
          string duration = ngettext ("%d min", "%d mins", details.duration).printf (details.duration);
          if (subtitle != "")
            subtitle += ", " + duration;
          else
            subtitle = duration;
        }
      }

      GLib.Icon thumbnail;
      if (details != null)
      {
        thumbnail = new GLib.FileIcon (GLib.File.new_for_uri (details.image));
      }
      else
      {
        try
        {
          thumbnail = GLib.Icon.new_for_string (result.icon_hint);
        }
        catch (Error err) { thumbnail = null; }
      }

      if (subtitle == "") subtitle = result.comment;
      var real_preview = new Unity.MoviePreview (title, subtitle,
                                                 description, thumbnail);
      var play_video = new Unity.PreviewAction ("play", _("Play"), null);
      real_preview.add_action (play_video);

      // For now, rating == -1 and num_ratings == 0 hides the rating widget from the preview
      real_preview.set_rating (-1, 0);

      if (details != null)
      {
        //TODO: For details of future source types, factor out common detail key/value pairs
        if (details.directors.length > 0)
          real_preview.add_info (new Unity.InfoHint ("directors", ngettext ("Director", "Directors", details.directors.length), null, string.joinv (", ", details.directors)));

        if (details.starring != null && details.starring != "")
          real_preview.add_info (new Unity.InfoHint ("cast", _("Cast"), null, details.starring));

        if (details.genres != null && details.genres.length > 0)
          real_preview.add_info (new Unity.InfoHint ("genres", ngettext("Genre", "Genres", details.genres.length), null, string.joinv (", ", details.genres)));

        // TODO: Add Vimeo & YouTube details for v1 of JSON API
        if (details.uploaded_by != null && details.uploaded_by != "")
          real_preview.add_info (new Unity.InfoHint ("uploaded-by", _("Uploaded by"), null, details.uploaded_by));

        if (details.date_uploaded != null && details.date_uploaded != "")
          real_preview.add_info (new Unity.InfoHint ("uploaded-on", _("Uploaded on"), null, details.date_uploaded));
      }

      return real_preview;
    }

    public async Preview? preview_result (ScopeResult result)
    {
      var uri_variant = result.metadata.lookup (RESULT_DETAILS_URI);
      var details_uri = uri_variant.get_string ();
      if (details_uri != null && details_uri != "")
      {
        try
        {
          var details = yield get_details (details_uri);
          return build_preview (result, details);
        }
        catch (Error e)
        {
          warning ("Failed to fetch video details: %s", e.message);
        }
      }
      else
      {
        warning ("Missing details uri for: '%s'", result.uri);
      }
      return build_preview (result, null);
    }

    private async RemoteVideoDetails? get_details (string url) throws Error
    {
      try
      {
        var msg = yield queue_soup_message (new Soup.Message ("GET", url));
        var details = UbuntuVideoSearch.process_details_results ((string) msg.response_body.data);
        return details;
      }
      catch (Error err)
      {
        warning ("Unable to get details from the server: %s", err.message);
      }
      return null;
    }

    /* Query the server with the search string and the list of sources.
     */
    private async void perform_search (ScopeSearchBase search) throws Error
    {
      if (search.search_context.search_type != Unity.SearchType.DEFAULT) return;
      var search_string = search.search_context.search_query;
      var result_set = search.search_context.result_set;

      if ((search_string == null || search_string == "") && (recommendations.size > 0))
      {
        var time = new DateTime.now_utc ();
        // do we have cached recommendations?
        if (time.to_unix () - recommendations_last_update < REFRESH_INTERVAL)
        {
          debug ("Updating search results with recommendations");
          push_results (result_set, recommendations);
          result_set.ttl = REFRESH_INTERVAL;
          return;
        }
      }

      var cancellable = search.search_context.cancellable.get_gcancellable ();
      var url = UbuntuVideoSearch.build_search_uri (search_string, null, search.search_context.search_metadata.form_factor);
      debug ("Querying the server: %s", url);

      bool is_treat_yourself = (search_string == null || search_string == "");

      var msg = yield queue_soup_message (new Soup.Message ("GET", url), cancellable);

      var results = UbuntuVideoSearch.process_search_results ((string)msg.response_body.data, is_treat_yourself);
      if (results != null)
      {
        if (search_string == null || search_string.strip () == "")
        {
          debug ("Empty search, updating recommendations");
          var time = new DateTime.now_utc ();
          recommendations = results;
          recommendations_last_update = time.to_unix ();
        }
        push_results (result_set, results);
      }
    }

    private async Soup.Message? queue_soup_message (Soup.Message msg,
                                                    GLib.Cancellable? cancellable = null)
      throws Error
    {
      Soup.Message? result_msg = null;
      session.queue_message (msg, (session_, msg_) =>
      {
        result_msg = msg_;
        queue_soup_message.callback ();
      });

      var cancelled = false;
      ulong cancel_id = 0;
      if (cancellable != null)
      {
        cancel_id = cancellable.cancelled.connect (() =>
        {
          cancelled = true;
          session.cancel_message (msg, Soup.KnownStatusCode.CANCELLED);
        });
      }

      yield;

      if (cancelled)
      {
        // we can't disconnect right away, as that would deadlock (cause
        // cancel_message doesn't return before invoking the callback)
        Idle.add (queue_soup_message.callback);
        yield;
        cancellable.disconnect (cancel_id);
        throw new IOError.CANCELLED ("Cancelled");
      }

      if (cancellable != null)
        cancellable.disconnect (cancel_id);

      if (result_msg.status_code < 100)
      {
        throw new IOError.FAILED ("Request failed with error %u",
                                  result_msg.status_code);
      }

      if (result_msg.status_code != 200)
      {
        throw new IOError.FAILED ("Request returned status code %u",
                                  result_msg.status_code);
      }

      return result_msg;
    }

    private void push_results (Unity.ResultSet result_set, Gee.ArrayList<RemoteVideoFile?> results)
    {
      foreach (var video in results)
      {
        if (video.uri.has_prefix ("http"))
        {
          var result_icon = video.icon;

          if (video.category == CAT_INDEX_MORE && video.price != null && video.price != "")
          {
            var anno_icon = new Unity.AnnotatedIcon (new FileIcon (File.new_for_uri (result_icon)));
            anno_icon.category = Unity.CategoryType.MOVIE;
            anno_icon.ribbon = video.price;
            result_icon = anno_icon.to_string ();
          }

          // aggregator scope remaps categories, so we won't get real category back;
          // put real category into metadata
          var realcat = new Variant.dict_entry (RESULT_PREVIEW_ON_LMB,
            new Variant.variant (video.category == CAT_INDEX_MORE));
          var details_uri = new Variant.dict_entry (RESULT_DETAILS_URI,
            new Variant.variant (video.details_uri));
          var metadata = new Variant.array (VariantType.VARDICT.element (), {realcat, details_uri});

          var result = new Variant ("(ssuussss@a{sv})", video.uri,
                                    result_icon, video.category,
                                    ResultType.DEFAULT, "text/html",
                                    video.title, video.comment,
                                    video.uri, metadata);

          result_set.add_result_from_variant (result);
        }
      }
    }

    private void zeitgeist_init () throws Error
    {
      zg_sources = new Zeitgeist.DataSourceRegistry ();
      var templates = new PtrArray.sized(1);
      var ev = new Zeitgeist.Event.full (Zeitgeist.ZG_ACCESS_EVENT, Zeitgeist.ZG_USER_ACTIVITY, "lens://unity-lens-video");
      templates.add ((ev as GLib.Object).ref());
      var data_source = new Zeitgeist.DataSource.full ("98898", "Unity Video Lens", "", templates);
      zg_sources.register_data_source.begin (data_source, null);
    }

    private void zeitgeist_insert_event (string uri, string title, string icon)
    {
      if (!use_zeitgeist) return;

      var subject = new Zeitgeist.Subject.full (uri, Zeitgeist.NFO_VIDEO, Zeitgeist.NFO_REMOTE_DATA_OBJECT, "", uri, title, icon);
      var event = new Zeitgeist.Event.full (Zeitgeist.ZG_ACCESS_EVENT, Zeitgeist.ZG_USER_ACTIVITY, "lens://unity-lens-video");
      event.add_subject (subject);

      var ev_array = new PtrArray.sized(1);
      ev_array.add ((event as GLib.Object).ref ());
      Zeitgeist.Log.get_default ().insert_events_from_ptrarray.begin (ev_array, null);
    }
  }
}
