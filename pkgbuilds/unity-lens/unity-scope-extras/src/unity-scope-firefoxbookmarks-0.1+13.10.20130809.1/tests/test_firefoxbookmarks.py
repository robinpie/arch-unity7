#! /usr/bin/python3
# -*- coding: utf-8 -*-
from gi.repository import Unity
from unittest import TestCase
import imp


class ResultSet(Unity.ResultSet):
    def __init__(self):
        Unity.ResultSet.__init__(self)
        self.results = []

    def do_add_result(self, result):
        self.results.append({'uri': result.uri,
                             'title': result.title,
                             'comment': result.comment,
                             'icon': result.icon_hint})


class ScopeTestCase(TestCase):
    def init_scope(self, scope_path):
        self.scope_module = imp.load_source('scope', scope_path)
        self.scope = self.scope_module.load_scope()

    def perform_query(self, query, filter_set=Unity.FilterSet.new()):
        result_set = ResultSet()
        ctx = Unity.SearchContext.create(query, 0, filter_set,
                                         None, result_set, None)
        s = self.scope.create_search_for_query(ctx)
        s.run()
        return result_set


class TestFirefoxBookmarks(ScopeTestCase):
    def setUp(self):
        self.init_scope('src/unity_firefoxbookmarks_daemon.py')

    def tearDown(self):
        self.scope = None
        self.scope_module = None

    def test_search(self):
        self.scope_module.BOOKMARKS_PATH = 'tests/data/mock_firefoxbookmarks_pass/'
        expected_results = ["http://www.ubuntulinux.org/",
                            "Ubuntu"]
        results = []
        for s in ['ubuntu']:
            result_set = self.perform_query(s)
            results.append(result_set.results[0]['uri'])
            results.append(result_set.results[0]['title'])
        self.assertEqual(results, expected_results)

    def test_failing_search(self):
        self.scope_module.BOOKMARKS_PATH = 'tests/data/mock_firefoxbookmarks_fail/'
        for s in ['upnriitnyt']:
            result_set = self.perform_query(s)
            self.assertEqual(len(result_set.results), 0)

    def test_activation(self):
        '''
        test_activation(): Tests the scope's do_activate() method
        '''
        result = Unity.ScopeResult()
        self.scope_module.FIREFOX_EXECUTABLE = 'tests/fake-firefox.py'
        result.uri = "http://www.ubuntu.com"
        activation = self.scope.activate(result, Unity.SearchMetadata(), None)
        self.assertEqual(activation.props.goto_uri, None)
        self.assertEqual(activation.props.handled, Unity.HandledType.HIDE_DASH)

if __name__ == '__main__':
    unittest.main()
