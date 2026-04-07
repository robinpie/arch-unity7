#! /usr/bin/python3
# -*- coding: utf-8 -*-
import imp
import os
from unittest import TestCase

from gi.repository import Unity

class ResultSet(Unity.ResultSet):
    def __init__(self):
        Unity.ResultSet.__init__(self)
        self.results = []

    def do_add_result(self, result):
        self.results.append({'uri':result.uri,
                             'title':result.title,
                             'comment':result.comment,
                             'icon':result.icon_hint})

class ScopeTestCase(TestCase):
    def init_scope(self, scope_path):
        self.scope_module = imp.load_source('scope', scope_path)
        self.scope = self.scope_module.load_scope()

    def perform_query(self, query, filter_set = Unity.FilterSet.new()):
        result_set = ResultSet()
        ctx = Unity.SearchContext.create(query, 0, filter_set,
                                         None, result_set, None)
        s = self.scope.create_search_for_query(ctx)
        s.run()
        return result_set


class TestCalculator(ScopeTestCase):
    def setUp(self):
        self.init_scope('src/unity_calculator_daemon.py')
        self.scope_module.CALCULATOR_EXECUTABLE = os.path.join(
            os.path.dirname(__file__), "fake-calculator.py")

    def tearDown(self):
        self.scope = None
        self.scope_module = None

    def assertResultTitle(self, query, expected):
        result_set = self.perform_query(query)
        titles = [result['title'] for result in result_set.results]
        self.assertEqual(titles, expected)

    def test_valid_searches(self):
        self.assertResultTitle('0+0', ['0'])
        self.assertResultTitle('tan5', ['0.087488664'])
        # XXX: Disabled since it fails in a non-UTF-8 locale, such as
        # the testing environment.
        #self.assertResultTitle('sqrtpi', ['1.772453851'])
        self.assertResultTitle('1,23%+10', ['10.0123'])

    def test_activation(self):
        result = Unity.ScopeResult()
        result.uri = "3"
        activation = self.scope.activate(
            result, Unity.SearchMetadata(), None)
        self.assertEqual(activation.props.goto_uri, None)
        self.assertEqual(activation.props.handled, Unity.HandledType.HIDE_DASH)

    def test_failing_search(self):
        self.assertResultTitle('12', [])
        self.assertResultTitle('1/0', [])
        self.assertResultTitle('bob', [])
        self.assertResultTitle('1**1', [])

    def test_preview(self):
        result = Unity.ScopeResult()
        result.uri = "3"
        result.title = "3"
        result.comment = "1+2"
        metadata = Unity.SearchMetadata()
        previewer = self.scope.create_previewer(result, metadata)
        preview = previewer.run()
        self.assertEqual(preview.props.title, "3")
        self.assertEqual(preview.props.subtitle, "1+2")
        self.assertNotEqual(preview.props.image, None)
        self.assertEqual(preview.props.image.get_names(), ['gnome-calculator', 'gnome-calculator-symbolic'])

if __name__ == '__main__':
    unittest.main()
