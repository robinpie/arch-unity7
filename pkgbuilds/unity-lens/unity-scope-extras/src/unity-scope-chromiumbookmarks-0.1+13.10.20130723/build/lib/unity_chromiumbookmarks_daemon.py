#! /usr/bin/python3
# -*- coding: utf-8 -*-

# Copyright(C) 2013 Mark Tully <markjtully@gmail.com>
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranties of
# MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
# PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>.

from gi.repository import GLib, Gio
from gi.repository import Unity
import gettext
import os
import webbrowser
import json
import re
import sqlite3
import tempfile

APP_NAME = 'unity-scope-chromiumbookmarks'
LOCAL_PATH = '/usr/share/locale/'
gettext.bindtextdomain(APP_NAME, LOCAL_PATH)
gettext.textdomain(APP_NAME)
_ = gettext.gettext

GROUP_NAME = 'com.canonical.Unity.Scope.Webhistory.Chromiumbookmarks'
UNIQUE_PATH = '/com/canonical/unity/scope/webhistory/chromiumbookmarks'

SEARCH_HINT = _('Search Bookmarks')
NO_RESULTS_HINT = _('Sorry, there are no Bookmarks that match your search.')
PROVIDER_CREDITS = _('')
SVG_DIR = '/usr/share/icons/unity-icon-theme/places/svg/'
PROVIDER_ICON = SVG_DIR + 'group-browserbookmarks.svg'
DEFAULT_RESULT_ICON = SVG_DIR + 'group-browserbookmarks.svg'
DEFAULT_RESULT_MIMETYPE = 'text/html'
DEFAULT_RESULT_TYPE = Unity.ResultType.DEFAULT
DEFAULT_BOOKMARKS = os.getenv("HOME") + "/.config/chromium/Default/Bookmarks"
DEFAULT_TOP_SITES = ''
PARSE_ALL_PROFILES = True
CHROMIUM_EXECUTABLE = '/usr/bin/chromium-browser'
TMP = tempfile.mkdtemp()

c1 = {'id': 'bookmarks',
      'name': _('Bookmarks'),
      'icon': SVG_DIR + 'group-installed.svg',
      'renderer': Unity.CategoryRenderer.VERTICAL_TILE}
CATEGORIES = [c1]

FILTERS = []

EXTRA_METADATA = []


def get_chromium_profiles():
    BOOKMARKS_PATH = [DEFAULT_BOOKMARKS]
    if not PARSE_ALL_PROFILES:
        return BOOKMARKS_PATH
    try:
        for f in os.listdir(os.getenv("HOME") + "/.config/chromium/"):
            if f == "Default" or f.startswith('Profile '):
                profile_path = os.getenv("HOME") + "/.config/chromium/"+f+"/Bookmarks"
                if not profile_path in BOOKMARKS_PATH:
                    BOOKMARKS_PATH.append(profile_path)
    except Exception as error:
        print(error)
    return BOOKMARKS_PATH


def get_bookmarks_from_chromium(path):
    '''
     Gets a list of bookmarks from the user's chromium profile
    Args:
       path: the path to Chromium's bookmarks file
    Returns:
      An array of bookmarks in the format [url, name]
    '''
    bookmarks = []
    if os.path.exists(path):
        bookmark_file = open(path)
        try:
            bookmark_json = json.load(bookmark_file)
        except ValueError:
            bookmark_json = "{}"

        def html_for_node(node):
            '''
            For a node in the section, decide whether it's a folder or a bookmark
            '''
            if 'url' in node:
                return html_for_url_node(node)
            elif 'children' in node:
                return html_for_parent_node(node)
            else:
                return ''
    
        def html_for_url_node(node):
            '''
            For a bookmark, add the details
            '''
            if not re.match("javascript:", node['url']):
                bookmark = []
                bookmark.append(node['name'])
                bookmark.append(node['url'])
                bookmarks.append(bookmark)
                return
            else:
                return
    
        def html_for_parent_node(node):
            '''
            For a folder, pass that node to html_for_node
            '''
            for childnode in node['children']:
                html_for_node(childnode)
            return
    
        try:
            html_for_node(bookmark_json['roots']['bookmark_bar'])
            html_for_node(bookmark_json['roots']['other'])
        except TypeError:
            pass
    return bookmarks


def search(search, filters):
    '''
    Search for help documents matching the search string
    '''
    results = []
#    global TMP
#    if not os.path.exists(TMP):
#        tmp_folder = tempfile.mkdtemp()
#        TMP = tmp_folder
#    else:
#        tmp_folder = TMP

    for path in get_chromium_profiles():
        user = path.split('/')[-2]
        bookmarks = get_bookmarks_from_chromium(path)

#        if DEFAULT_TOP_SITES:
#            thumbnail_path = DEFAULT_TOP_SITES
#        else:
#            thumbnail_path = path.replace('Bookmarks', 'Top Sites')
#        conn = sqlite3.connect(thumbnail_path)
#        connection = conn.cursor()

        for bookmark in bookmarks:
            # Search bookmark names for matches
            if search.lower() in bookmark[0].lower() or search.lower() in bookmark[1].lower():
#                sqlite_query = '''SELECT thumbnail FROM thumbnails WHERE url = "%s"''' % bookmark[1]
                icon = None
#                connection.execute(sqlite_query)
#                thumb = connection.fetchall()
#                thumbname = bookmark[1].replace('/', '')
#                if thumb:
#                    imageinfo = thumb[0][0]
#                    if imageinfo:
#                        open('%s/%s' % (tmp_folder, thumbname) , 'wb').write(imageinfo)
#                        icon = '%s/%s' % (tmp_folder, thumbname)
#                else:
#                    if os.path.exists('%s/%s' % (tmp_folder, thumbname)):
#                        os.remove('%s/%s' % (tmp_folder, thumbname))
                results.append({'uri': bookmark[1],
                                'icon': icon,
                                'category': 0,
                                'title': bookmark[0],
                                'user':user})
#        connection.close()
    return results


def activate(result, metadata, id):
    """ Open the url in the default webbrowser
    Args:
      uri: The url to be opened
    """
    parameters = [CHROMIUM_EXECUTABLE, result.uri]
    GLib.spawn_async(parameters)
    return Unity.ActivationResponse(handled=Unity.HandledType.HIDE_DASH, goto_uri=None)


class Preview(Unity.ResultPreviewer):
    '''
    Creates the preview for the result
    '''
    def do_run(self):
        '''
        Create a preview and return it
        '''
        preview = Unity.GenericPreview.new(self.result.title, '', None)
        preview.props.subtitle = self.result.uri
        
        if os.path.exists(self.result.icon_hint):
            preview.props.image_source_uri = 'file://' + self.result.icon_hint
        else:
            preview.props.image = Gio.ThemedIcon.new('gtk-about')
        show_action = Unity.PreviewAction.new("show", _("Open"), None)
        preview.add_action(show_action)
        return preview

# Classes below this point establish communication
# with Unity, you probably shouldn't modify them.


class MySearch(Unity.ScopeSearchBase):
    def __init__(self, search_context):
        super(MySearch, self).__init__()
        self.set_search_context(search_context)

    def do_run(self):
        '''
        Adds results to the model
        '''
        try:
            result_set = self.search_context.result_set
            for i in search(self.search_context.search_query,
                            self.search_context.filter_state):
                if not 'uri' in i or not i['uri'] or i['uri'] == '':
                    continue
                if not 'icon' in i or not i['icon'] or i['icon'] == '':
                    i['icon'] = DEFAULT_RESULT_ICON
                if not 'mimetype' in i or not i['mimetype'] or i['mimetype'] == '':
                    i['mimetype'] = DEFAULT_RESULT_MIMETYPE
                if not 'result_type' in i or not i['result_type'] or i['result_type'] == '':
                    i['result_type'] = DEFAULT_RESULT_TYPE
                if not 'category' in i or not i['category'] or i['category'] == '':
                    i['category'] = 0
                if not 'title' in i or not i['title']:
                    i['title'] = ''
                if not 'comment' in i or not i['comment']:
                    i['comment'] = ''
                if not 'dnd_uri' in i or not i['dnd_uri'] or i['dnd_uri'] == '':
                    i['dnd_uri'] = i['uri']
                i['metadata'] = {}
                if EXTRA_METADATA:
                    for e in i:
                        for m in EXTRA_METADATA:
                            if m['id'] == e:
                                i['metadata'][e] = i[e]
                i['metadata']['provider_credits'] = GLib.Variant('s', PROVIDER_CREDITS)
                result_set.add_result(**i)
        except Exception as error:
            print(error)


class Scope(Unity.AbstractScope):
    def __init__(self):
        Unity.AbstractScope.__init__(self)

    def do_get_search_hint(self):
        return SEARCH_HINT

    def do_get_schema(self):
        '''
        Adds specific metadata fields
        '''
        schema = Unity.Schema.new()
        if EXTRA_METADATA:
            for m in EXTRA_METADATA:
                schema.add_field(m['id'], m['type'], m['field'])
        #FIXME should be REQUIRED for credits
        schema.add_field('provider_credits', 's', Unity.SchemaFieldType.OPTIONAL)
        return schema

    def do_get_categories(self):
        '''
        Adds categories
        '''
        cs = Unity.CategorySet.new()
        if CATEGORIES:
            for c in CATEGORIES:
                cat = Unity.Category.new(c['id'], c['name'],
                                         Gio.ThemedIcon.new(c['icon']),
                                         c['renderer'])
                cs.add(cat)
        return cs

    def do_get_filters(self):
        '''
        Adds filters
        '''
        fs = Unity.FilterSet.new()
        #if FILTERS:
        #
        return fs

    def do_get_group_name(self):
        return GROUP_NAME

    def do_get_unique_name(self):
        return UNIQUE_PATH

    def do_create_search_for_query(self, search_context):
        se = MySearch(search_context)
        return se

    def do_activate(self, result, metadata, id):
        return activate(result, metadata, id)

    def do_create_previewer(self, result, metadata):
        '''
        Creates a preview when a resut is right-clicked
        '''
        result_preview = Preview()
        result_preview.set_scope_result(result)
        result_preview.set_search_metadata(metadata)
        return result_preview


def load_scope():
    return Scope()
