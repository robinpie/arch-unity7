#! /usr/bin/python3
# -*- coding: utf-8 -*-

# Copyright (C) 2013 David Callé <davidc@framli.eu>
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

from gi.repository import Unity, UnityExtras
from gi.repository import Gio, GLib
import gettext

APP_NAME = 'unity-scope-calculator'
LOCAL_PATH = '/usr/share/locale/'
gettext.bindtextdomain(APP_NAME, LOCAL_PATH)
gettext.textdomain(APP_NAME)
_ = gettext.gettext

GROUP_NAME = 'com.canonical.Unity.Scope.Info.Calculator'
UNIQUE_PATH = '/com/canonical/unity/scope/info/calculator'
SEARCH_HINT = _('Calculate')
NO_RESULTS_HINT = _('Sorry, there are no results that match your search.')
PROVIDER_CREDITS = _('')
SVG_DIR = '/usr/share/icons/unity-icon-theme/places/svg/'
PROVIDER_ICON = SVG_DIR+'service-calculator.svg'
DEFAULT_RESULT_ICON = SVG_DIR+'result-info.svg'
DEFAULT_RESULT_MIMETYPE = 'text/html'
DEFAULT_RESULT_TYPE = Unity.ResultType.PERSONAL

c1 = {'id'      :'results',
      'name'    :_('Result'),
      'icon'    :SVG_DIR+'group-installed.svg',
      'renderer':Unity.CategoryRenderer.VERTICAL_TILE}
CATEGORIES = [c1]
FILTERS = []
EXTRA_METADATA = []

CALCULATOR_EXECUTABLE = '/usr/bin/gnome-calculator'

def search(search, filters):
    '''
    Any search method returning results as a list of tuples.
    Available tuple fields:
    uri (string)
    icon (string)
    title (string)
    comment (string)
    dnd_uri (string)
    mimetype (string)
    category (int)
    result_type (Unity ResultType)
    extras metadata fields (variant)
    '''
    results = []
    search = search.replace(' ','')
    search = search.replace(',','.')
    search = search.replace('pi','π')
    search = search.replace('sqrt','√')
    search = search.replace('"','')
    search = search.replace('\\','')
    search = search.replace('&','')
    search = search.replace('|','')
    search = search.replace(';','')
    search = search.replace('&','')
    hint = search
    hint = hint.replace('\(','(')
    hint = hint.replace('\)',')')
    hint = hint.replace('sqrt','√')
    print ("Operation: %s" % hint)
    try:
        result = GLib.spawn_sync(None,
                                 [CALCULATOR_EXECUTABLE,
                                 '--solve',
                                 search],
                                 None, 0, None, None)
    except Exception as error:
        print (error)
        return results
    r = result[1].decode('utf-8').replace("\n", "")
    if r.startswith("Error") or r==hint:
        return results
    results.append({'uri':r,
                    'title':r,
                    'comment':hint,
                    'icon':'gnome-calculator'})
    return results


# Classes below this point establish communication
# with Unity, you probably shouldn't modify them.


class MySearch (Unity.ScopeSearchBase):
    def __init__(self, search_context):
        super (MySearch, self).__init__()
        self.set_search_context (search_context)

    def do_run (self):
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
                result_set.add_result(**i)
        except Exception as error:
            print (error)

class Preview (Unity.ResultPreviewer):

    def do_run(self):
        image = Gio.ThemedIcon.new('gnome-calculator')
        preview = Unity.GenericPreview.new(self.result.title, '', image)
        preview.props.subtitle = self.result.comment
        open_action = Unity.PreviewAction.new("open", _("Open in calculator"), None)
        preview.add_action(open_action)
        return preview


class Scope (Unity.AbstractScope):
    def __init__(self):
        Unity.AbstractScope.__init__(self)

    def do_get_search_hint (self):
        return SEARCH_HINT

    def do_get_schema (self):
        '''
        Adds specific metadata fields
        '''
        schema = Unity.Schema.new ()
        if EXTRA_METADATA:
            for m in EXTRA_METADATA:
                schema.add_field(m['id'], m['type'], m['field'])
        #FIXME should be REQUIRED for credits
        schema.add_field('provider_credits', 's', Unity.SchemaFieldType.OPTIONAL)
        return schema

    def do_get_categories (self):
        '''
        Adds categories
        '''
        cs = Unity.CategorySet.new ()
        if CATEGORIES:
            for c in CATEGORIES:
                cat = Unity.Category.new (c['id'], c['name'],
                                          Gio.ThemedIcon.new(c['icon']),
                                          c['renderer'])
                cs.add (cat)
        return cs

    def do_get_filters (self):
        '''
        Adds filters
        '''
        fs = Unity.FilterSet.new ()
#        if FILTERS:
#            
        return fs

    def do_get_group_name (self):
        return GROUP_NAME

    def do_get_unique_name (self):
        return UNIQUE_PATH

    def do_create_search_for_query (self, search_context):
        se = MySearch (search_context)
        return se

    def do_create_previewer(self, result, metadata):
        rp = Preview()
        rp.set_scope_result(result)
        rp.set_search_metadata(metadata)
        return rp

    def do_activate(self, result, metadata, id):
        GLib.spawn_async([CALCULATOR_EXECUTABLE, "-e", result.uri])
        return Unity.ActivationResponse(handled=Unity.HandledType.HIDE_DASH, goto_uri=None)

def load_scope():
    return Scope()
