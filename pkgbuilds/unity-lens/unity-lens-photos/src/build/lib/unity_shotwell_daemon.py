#!/usr/bin/python3
# -*- coding: utf-8 -*-

#    Copyright (c) 2012 David Calle <davidc@framli.eu>

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

import gettext
import locale
import sys, os
from gi.repository import GLib, GObject, Gio, Unity, UnityExtras
import time, datetime
import shutil
import sqlite3

APP_NAME = "unity-lens-photos"
LOCAL_PATH = "/usr/share/locale/"

gettext.bindtextdomain(APP_NAME, LOCAL_PATH)
gettext.textdomain(APP_NAME)
_ = gettext.gettext

# Translatable strings
SOURCE = _("This Computer")
NO_RESULTS_HINT = _("Sorry, there are no photos that match your search.")
BUS_NAME = "com.canonical.Unity.Scope.Photos.Shotwell"
CACHE = "%s/unity-lens-photos" % GLib.get_user_cache_dir()
THUMB_CACHE = "%s/shotwell/thumbs/" % GLib.get_user_cache_dir()
HOME_FOLDER = GLib.get_home_dir()
CAT_MINE = _("My Photos")
CAT_FRIENDS = _("Friends Photos")
CAT_ONLINE = _("Online Photos")
CAT_GLOBAL = _("Photos")
CAT_RECENT = _("Recent")
FILTER_DATE = _("Date")
FILTER_OPTION_7DAYS = _("Last 7 days")
FILTER_OPTION_30DAYS = _("Last 30 days")
FILTER_OPTION_6MONTHS = _("Last 6 months")
FILTER_OPTION_OLDER = _("Older")
THEME = "/usr/share/icons/unity-icon-theme/places/svg/"

class Scope(Unity.DeprecatedScope):

    last_result = None

    def do_preview_result(self, result, callback):
        """Temporarily save the ScopeResult so it is available for
        preview-uri signal handlers."""
        # Wrap the AsyncReadyCallback to handle dummy user_data argument.
        def wrapped_callback(object, async_result, user_data):
            return callback(object, async_result)

        self.last_result = result
        try:
            return Unity.DeprecatedScope.do_preview_result(
                self, result, wrapped_callback, None)
        finally:
            self.last_result = None


class Daemon:

    """Creation of the Shotwell scope."""
    def __init__ (self):
        self._enabled = True
        self._scope = Scope (dbus_path="/com/canonical/unity/scope/photos/shotwell", id="shotwell")
        self._scope.connect ("search-changed", self.on_search_changed)
        self._scope.connect("notify::active", self.on_lens_active)
        self._scope.connect("notify::filtering", self.on_filtering_changed)
        self._scope.connect('preview-uri', self.on_preview_uri)
        source_name = SOURCE
        self.last_db_change = None
        self._sources_options = []
        self._sources_options.append(source_name)
        self._scope.props.sources.add_option(source_name,source_name, None)
        print (self._scope.props.sources.options[0].props.id)
        filters = Unity.FilterSet.new()
        f2 = Unity.RadioOptionFilter.new ("date", FILTER_DATE, Gio.ThemedIcon.new("input-keyboard-symbolic"), False)
        f2.add_option ("7", FILTER_OPTION_7DAYS, None)
        f2.add_option ("30", FILTER_OPTION_30DAYS, None)
        f2.add_option ("180", FILTER_OPTION_6MONTHS, None)
        f2.add_option ("100000",FILTER_OPTION_OLDER, None)
        filters.add (f2)
        cats = Unity.CategorySet.new()
        cats.add (Unity.Category.new ('recent',
                                      CAT_RECENT,
                                      Gio.ThemedIcon.new(THEME + "group-recent.svg"),
                                      Unity.CategoryRenderer.VERTICAL_TILE))
        cats.add (Unity.Category.new ('mine',
                                      CAT_MINE,
                                      Gio.ThemedIcon.new(THEME + "group-photos.svg"),
                                      Unity.CategoryRenderer.VERTICAL_TILE))
        cats.add (Unity.Category.new ('friends',
                                      CAT_FRIENDS,
                                      Gio.ThemedIcon.new(THEME + "group-friends.svg"),
                                      Unity.CategoryRenderer.VERTICAL_TILE))
        cats.add (Unity.Category.new ('online',
                                      CAT_ONLINE,
                                      Gio.ThemedIcon.new(THEME + "group-online.svg"),
                                      Unity.CategoryRenderer.VERTICAL_TILE))
        cats.add (Unity.Category.new ('global',
                                      CAT_GLOBAL,
                                      Gio.ThemedIcon.new(THEME + "group-photos.svg"),
                                      Unity.CategoryRenderer.VERTICAL_TILE))
        self._scope.props.categories = cats
        self._scope.props.filters = filters
        self._scope.export()
        self.tagdb = []
        self.db = None
        self.cancel_running_search = False

    def callback(object, result, user_data):
        object.preview_result_finish(result)

    def on_filtering_changed(self, *_):
        """Update results when a filter change is notified."""
        for source in self._sources_options:
            filtering = self._scope.props.sources.props.filtering
            active = self._scope.props.sources.get_option(source).props.active
            if (active and filtering) or (not active and not filtering):
                if not self._enabled:
                    self._enabled = True
                    self._scope.queue_search_changed(Unity.SearchType.DEFAULT)
            else:
                self._enabled = False
                self._scope.queue_search_changed(Unity.SearchType.DEFAULT)
            print ("    %s enabled : %s" % (source, self._enabled))


    def on_lens_active(self, *_):
        """ Update results when the lens is opened """
#        if self._scope.props.active:
        self._scope.queue_search_changed(Unity.SearchType.DEFAULT)


    def on_search_changed(self, scope, search, search_type, *_):
        """Run another search when a filter change is notified."""
        self.cancel_running_search = True
        model = search.props.results_model
        search.set_reply_hint ("no-results-hint", GLib.Variant.new_string(NO_RESULTS_HINT))
        model.clear()
        search_string = search.props.search_string.strip()
        print ("Search changed to \"%s\"" % search_string)
        if self._enabled:
            date = self.check_date_filter (search)
            if search_type is Unity.SearchType.GLOBAL:
                if len(search_string) > 0:
                    cats = [4]
                    GLib.idle_add(self.update_results_model,search_string, model, cats, date, search)
                else:
                    search.emit('finished')
            else:
                if search_string or date:
                    cats = [1]
                else:
                    cats = [0,1]
                GLib.idle_add(self.update_results_model,search_string, model, cats, date, search)
        else:
            search.emit('finished')
        


    def update_results_model(self, search, model, cats, date, s):
        """Update model with Shotwell results"""
        for cat in cats:
            if cat == 0:
                limit = 50
            else:
                limit = 100
            for i in self.shotwell(search, date, limit):
                if not self.cancel_running_search:
                    thumb_id = "thumb%016x" % i[4]
                    try:
                        extension = i[1].split('.')[-1].lower ()
                    except:
                        extension = "jpg"
                    icon_hint = THUMB_CACHE + "thumbs128/" + thumb_id + "." + extension
                    if not self.is_file (icon_hint):
                        icon_hint = THUMB_CACHE + "thumbs360/" + thumb_id + "." + extension
                        if not self.is_file (icon_hint):
                            icon_hint = THUMB_CACHE + "thumbs128/" + thumb_id + ".jpg"
                            if not self.is_file (icon_hint):
                                 icon_hint = THUMB_CACHE + "thumbs360/" + thumb_id + ".jpg"
                                 if not self.is_file (icon_hint):
                                    icon_hint = i[1]
                                    if not self.is_file (icon_hint):
                                        icon_hint = "image"
                    title = i[0]
                    comment = i[3]
                    uri = "file://"+i[1]
                    model.append (uri=uri,
                                  icon_hint=icon_hint,
                                  category=cat,
                                  mimetype="text/html",
                                  title=title,
                                  comment=comment,
                                  dnd_uri=uri,
                                  result_type=Unity.ResultType.PERSONAL)
        s.emit('finished')


    def on_preview_uri(self, scope, uri):
            """Preview request handler"""
            preview = None
            if scope.last_result.uri == uri:
                title = scope.last_result.title
                desc = ''
                out_of_drive = False
                if self.is_file (uri.replace("file://", "")):
                    title = self.getTitle (uri.replace("file://", ""), scope.last_result.title)
                    # Test existence of a jpg thumb for raw files
                    raw_thumb = uri.replace("file://", "")
                    raw_thumb = raw_thumb.replace(".", "_")+"_shotwell.jpg"
                    if self.is_file(raw_thumb):
                        image = "file://"+raw_thumb
                    else:
                        image = uri
                else:
                    image = "file://"+scope.last_result.icon_hint
                    desc = _("<b>This photo is missing.</b>\nYou can open Shotwell to retrieve it or remove it from your library.")
                    out_of_drive = True
                preview = Unity.GenericPreview.new(title, desc, None)
                preview.props.image_source_uri = image
                subtitle = scope.last_result.comment.split("_ulp-date_")[1]
                if subtitle:
                    preview.props.subtitle = subtitle
                db = self.getDB ()
                if db:
                    photo = self.getPhotoForUri (db, uri)
                    if photo:
                        width = str(photo[2])
                        height = str(photo[3])
                        filesize = self.humanize_bytes(photo[4])
                        dimensions = _("%s x %s pixels") % (width, height)
                        tags_raw = self.getTagsForPhotoId(db, photo[0])
                        tags = ', '.join(tags_raw.split("__"))[2:]
                        if dimensions:
                            preview.add_info(Unity.InfoHint.new("dimensions", _("Dimensions"), None, dimensions))
                        if filesize:
                            preview.add_info(Unity.InfoHint.new("size", _("Size"), None, filesize))
                        if tags:
                            preview.add_info(Unity.InfoHint.new("tags", _("Tags"), None, tags))
                if not out_of_drive:
                    view_action = Unity.PreviewAction.new("view", _("View"), None)
                    view_action.connect('activated', self.view_action)
                    preview.add_action(view_action)
                    
                    show_action = Unity.PreviewAction.new("show", _("Show in Folder"), None)
                    show_action.connect('activated', self.show_action)
                    preview.add_action(show_action)
                    
                    email_action = Unity.PreviewAction.new("email", _("Email"), None)
                    email_action.connect('activated', self.email_action)
                    preview.add_action(email_action)
                    
                    print_action = Unity.PreviewAction.new("print", _("Print"), None)
                    print_action.connect('activated', self.print_action)
                    preview.add_action(print_action)
                else:
                    shotwell_action = Unity.PreviewAction.new("shotwell", _("Open Shotwell"), None)
                    shotwell_action.connect('activated', self.shotwell_action)
                    preview.add_action(shotwell_action)
            if preview == None:
                print ("Couldn't find model row for requested preview uri: '%s'", uri)
            return preview


    def print_action (self, scope, uri):
        """On item clicked, print photo"""
        GLib.spawn_async(["/usr/bin/lpr", uri.replace("file://", "")])
        return Unity.ActivationResponse(goto_uri='', handled=2 )


    def email_action (self, scope, uri):
        """On item clicked, email photo"""
        mail_app = Gio.AppInfo.get_default_for_uri_scheme("mailto")
        if mail_app:
            attach = Gio.file_new_for_uri(uri)
            mail_app.launch ([attach], None)
        return Unity.ActivationResponse(goto_uri='', handled=2 )


    def show_action (self, scope, uri):
        """On item clicked, show photo in folder"""
        GLib.spawn_async(["/usr/bin/nautilus", uri])
        return Unity.ActivationResponse(goto_uri='', handled=2 )

    def view_action (self, scope, uri):
        """On item clicked, view photo"""
        return
    
    def shotwell_action (self, scope, uri):
        """On item clicked, open Shotwell"""
        GLib.spawn_async(["/usr/bin/shotwell"])
        return Unity.ActivationResponse(goto_uri='', handled=2 )


    def check_date_filter(self, search):
        """Get active option for a filter name"""
        try:
            date = search.get_filter("date").get_active_option().props.id
            date = int(date)*86400
            now = time.time()
            date = now - date
        except (AttributeError):
            date = None
        return date


    def is_file(self, uri):
        """Check if the photo is an actual existing file"""
        g_file = Gio.file_new_for_path(uri)
        if g_file.query_exists(None):
            file_type = g_file.query_file_type(Gio.FileQueryInfoFlags.NONE,
                None)
            if file_type is Gio.FileType.REGULAR:
                return True
    
    def last_modification(self, uri):
        """Check if the photo is an actual existing file"""
        g_file = Gio.file_new_for_path(uri)
        time = g_file.query_info(Gio.FILE_ATTRIBUTE_TIME_MODIFIED,
                                 Gio.FileQueryInfoFlags.NONE,
                                 None).get_attribute_uint64('time::modified')
        return time


    def shotwell(self, search, date, limit):
        """Create a list of results by querying the DB and parsing Shotwell data"""
        data_list = []
        db = self.getDB ()
        if db:
            self.cancel_running_search = False

            matching_events = []
            matching_tags = set()
            search_items = []
            if search:
                search_items = search.lower().split(" ")
                # Make list of event ids for event names matching ALL search items
                allevents = self.getAllEventNames(db)
                for event in allevents:
                    match = True
                    for item in search_items:
                        if item not in event[1].lower():
                            match = False
                            break
                    if match:
                        matching_events.append(event[0])
                # Make list of thumbs where tags match all items
                first = True
                for item in search_items:
                    newset = set()
                    # Build set of all thumbs with tags matching this item
                    for tag in self.tagdb:
                        if item in tag[0].lower():
                            if tag[1]:
                                newset.update(tag[1].split(','))
                    if first:
                        matching_tags = newset
                        first = False
                    else:
                        # Keep only thumbs matching this and previous search items
                        matching_tags.intersection_update(newset)
            try:
                photos = self.getPhotos (db, date)
            except:
                photos = []
            i = 0
            for photo in photos:
                if photo[16] != 4 and photo[16] != 8 and not self.cancel_running_search:
                    pid = photo[0]
                    uri = photo[1]
                    event_id = photo[10]
                    title = photo[19]
                    if not title:
                        title = uri.split("/")[-1]
                    match = False
                    thumb_id = "thumb%016x" % pid
                    if not search or event_id in matching_events or thumb_id in matching_tags:
                        match = True
                    else:
                        match = True
                        for item in search_items:
                            if item not in title.lower():
                                match = False
                                break
                    if match:
                        icon_hint = photo[1]
                        timestamp = photo[6]
                        if timestamp is None or timestamp == 0:
                            timestamp = photo[5]
                        if timestamp is None or timestamp == 0:
                            date = ""
                        else:
                            date = datetime.datetime.fromtimestamp(timestamp).strftime('%d %b %Y %H:%M')

                        item_list = []
                        item_list.append(title)
                        item_list.append(uri)
                        item_list.append(icon_hint)
                        item_list.append(str(timestamp)+"_ulp-date_"+date)
                        item_list.append(pid)
                        data_list.append(item_list)
                        i += 1
                        if i >= limit:
                            break
        return data_list

    def isInTagDB (self, tags, term, photo):
        for tag in tags:
            if tag[0].lower().find(term)  > -1:
                thumb_id = "thumb%016x" % photo
                if tag[1]:
                    if thumb_id in tag[1]:
                        return True
        return False

    def getDB (self):
        """Check existence of our copy of Shotwell DB"""
        db = None
        newdb = False
        shotwell_desktop = "/usr/share/applications/org.gnome.Shotwell.desktop"
        if self.is_file(shotwell_desktop):
            if not Gio.file_new_for_path(CACHE).query_exists(None):
                Gio.file_new_for_path(CACHE).make_directory(None)
            shotwell_db_oldversion = HOME_FOLDER +"/.shotwell/data/photo.db"
            shotwell_db = HOME_FOLDER +"/.local/share/shotwell/data/photo.db"
            if self.is_file(shotwell_db):
                if self.last_db_change != self.last_modification (shotwell_db):
                    print ("Shotwell: DB update detected")
                    shutil.copy2(shotwell_db, CACHE+"/photos.db")
                    self.last_db_change = self.last_modification (shotwell_db)
                    newdb = True
                if self.is_file(CACHE+"/photos.db") and newdb:
                    db = sqlite3.connect(CACHE+"/photos.db")
                else:
                   db = self.db
            elif self.is_file(shotwell_db_oldversion):
                if self.last_db_change != self.last_modification (shotwell_db_oldversion):
                    print ("Shotwell: DB update detected")
                    shutil.copy2(shotwell_db_oldversion, CACHE+"/photos.db")
                    self.last_db_change = self.last_modification (shotwell_db_oldversion)
                    newdb = True
                if self.is_file(CACHE+"/photos.db") and newdb:
                    db = sqlite3.connect(CACHE+"/photos.db")
                else:
                    db = self.db
            else:
                pass
        if db and newdb:
            self.getAllTags (db)
            self.db = db
        return db


    def getPhotos (self, db, date):
        """Get all photos in DB"""
        if date:
            if date > 0:
                sql = 'select * from PhotoTable where exposure_time > '+str(date)+' or timestamp > '+str(date)+' order by timestamp desc, exposure_time desc'
            else:
                date = 180*86400
                now = int(time.time())
                date = now - date
                sql = 'select * from PhotoTable where exposure_time < '+str(date)+' and exposure_time != 0 or timestamp < '+str(date)+' and exposure_time = 0 order by timestamp desc, exposure_time desc'
        else:
            sql='select * from PhotoTable order by timestamp desc, exposure_time desc'
        cursor = db.cursor()
        photos = cursor.execute(sql).fetchall()
        cursor.close ()
        return photos


    def getPhotoForUri (self, db, uri):
        filename = uri.replace("file://", "")
        print (filename)
        sql='select * from PhotoTable where filename = \"'+filename+'\"'
        cursor = db.cursor()
        photo = cursor.execute(sql).fetchone()
        return photo


    def getAllTags (self, db):
        """Get all tags"""
        sql='SELECT name, photo_id_list FROM TagTable'
        cursor = db.cursor()
        tags = cursor.execute(sql).fetchall()
        cursor.close ()
        self.tagdb = tags


    def getEventNameForEventId (self, db, event_id, cursor):
        """Get event name related for a photo's event id"""
        raw_event = []
        event = None
        sql='select name from EventTable where id='+event_id
        raw_event = cursor.execute(sql).fetchone()
        if raw_event:
            event = raw_event[0]
        return str(event)

    def getAllEventNames (self, db):
        """Get all non-null event names with ids"""
        sql='SELECT id, name FROM EventTable WHERE length(name)>0'
        cursor = db.cursor()
        events = cursor.execute(sql).fetchall()
        cursor.close()
        return events


    def getTitle(self, uri, title):
        """Get date from timestamp in db or from file"""
        if not title:
            g_file = Gio.file_new_for_path(uri)
            title = g_file.query_info(Gio.FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
                                    Gio.FileQueryInfoFlags.NONE,
                                    None).get_attribute_string('standard::display-name')
        return title

    def getDate(self, uri, time):
        """Get date from timestamp in db or from file"""
        if time > 0:
            timestamp = time
        else:
            g_file = Gio.file_new_for_path(uri)
            timestamp = g_file.query_info(Gio.FILE_ATTRIBUTE_TIME_CREATED,
                                    Gio.FileQueryInfoFlags.NONE,
                                    None).get_attribute_uint64('time::created')
        if timestamp > 0:
            date = datetime.datetime.fromtimestamp(timestamp).strftime('%d %b %Y %H:%M')
        else:
            date = ''
        return date, timestamp


    def getTagsForPhotoId (self, db, photo):
        """Get all tags related to a photo"""
        thumb_id = "thumb%016x" % photo
        sql='SELECT name FROM TagTable WHERE photo_id_list LIKE ?'
        args=['%'+thumb_id+'%']
        cursor = db.cursor ()
        tags = cursor.execute(sql, args).fetchall()
        cursor.close ()
        string_of_tags = ""
        if len(tags) > 0:
            for t in tags:
                string_of_tags = string_of_tags +"__"+t[0]
        return string_of_tags


    def humanize_bytes(self, bytes, precision=1):
        """Get a humanized string representation of a number of bytes."""
        abbrevs = ((10**15, 'PB'),(10**12, 'TB'),(10**9, 'GB'),(10**6, 'MB'),(10**3, 'kB'),(1, 'b'))
        if bytes == 1:
            return '1 b'
        for factor, suffix in abbrevs:
            if bytes >= factor:
                break
        return '%.*f%s' % (precision, bytes / factor, suffix)

if __name__ == '__main__':
    daemon = UnityExtras.dbus_own_name(BUS_NAME, Daemon, None)
    if daemon:
        GLib.unix_signal_add(0, 2, lambda x: daemon.quit(), None)
        daemon.run([])

