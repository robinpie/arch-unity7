#!/usr/bin/python3
#
# Copyright (C) 2013 Canonical Ltd
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Authored by Pawel Stolowski <pawel.stolowski@canonical.com>
#
#

from wsgiref.simple_server import make_server
import cgi, sys, json
import argparse
import os, signal

#
# Simulates:
#   /smartscopes/v1/remote-scopes
#   /smartscopes/v1/search?
#   /smartscopes/v1/feedback
# requests.
#
# Search request handler returns content of predefined json files in a round-robin manner
# Both /search and /feedback handler can optionally dump some data useful for testing
# The server runs forver or serves a limited number of requests (with --requests .. argument) and then exits.
#
# Usage example:
# ./server.py --port 8888 --scopes remote_scopes.txt --search dump1.txt --feedback dump2.txt --requests 5 search_results1.txt search_results2.txt
#
# To test unity-home-scope with it, set SMART_SCOPES_SERVER=127.0.0.1:8888 environment variable before running unity-home-scope.
#
class FakeServerApp:
  def __init__ (self, remote_scopes, search_results, search_req_dump_file, feedback_req_dump_file):
    self._current_result = 0
    self._remote_scopes_file = remote_scopes
    self._search_results_files = search_results
    self._search_dump_file = search_req_dump_file
    self._feedback_dump_file = feedback_req_dump_file
    # truncate dump files
    if search_req_dump_file != None:
      f = open (search_req_dump_file, "w+")
      f.close ()
    if feedback_req_dump_file != None:
      f = open (feedback_req_dump_file, "w+")
      f.close()

  def __call__ (self, environ, start_response):
    method = environ.get ("PATH_INFO").rstrip ('/')
    routes = {
      "/smartscopes/v1/search": self.search,
      "/smartscopes/v1/feedback": self.feedback,
      "/smartscopes/v1/remote-scopes": self.remote_scopes
    }
    if method in routes:
      return routes[method](environ, start_response)
      
    return self.error ('404 NOT FOUND', 'Unknown method', environ, start_response)

  def error (self, status, message, environ, start_response):
    start_response (status, [('Content-Type', 'text/plain'), ('Content-Length', str (len (message)))])
    return [message]

  def search (self, environ, start_response):
    url = environ.get ("QUERY_STRING", "")
    params = dict (cgi.parse_qsl (url, keep_blank_values = True))
    self.dump_search_request (url)
    results = open (self._search_results_files[self._current_result])
    response_body = results.read ()
    results.close ()
    # advance to next results file
    self._current_result += 1
    if self._current_result >= len (self._search_results_files):
      self._current_result = 0
    response_headers = [('Content-Type', 'application/json'), ('Content-Length', str(len(response_body)))]
    start_response ('200 OK', response_headers)
    return [response_body]

  def remote_scopes (self, environ, start_response):
    results = open (self._remote_scopes_file)
    response_body = results.read ()
    results.close ()
    response_headers = [('Content-Type', 'application/json'), ('Content-Length', str(len(response_body)))]
    start_response ('200 OK', response_headers)
    return [response_body]

  def feedback (self, environ, start_response):
    if environ['REQUEST_METHOD'] != 'POST' and 'CONTENT_LENGTH' not in environ:
      return self.error('400 Bad Request', 'Bad Request', environ, start_response)

    response_headers = [('Content-Type', 'application/json'), ('Content-Length', str(len("")))]
    self.dump_feedback (environ)
    start_response ('200 OK', response_headers)
    return [""]

  def dump_feedback (self, environ):
    if self._feedback_dump_file != None:
      f = open (self._feedback_dump_file, 'a+')
      content_length = int(environ['CONTENT_LENGTH'])
      content = environ['wsgi.input'].read(content_length)
      f.write (content.replace ("\n", "") + "\n")
      f.close ()

  def dump_search_request (self, url):
    if self._search_dump_file != None:
      f = open (self._search_dump_file, 'a+')
      f.write (url + "\n")
      f.close ()

def timeout_handler (signum, frame):
  sys.exit (0)

def main ():
  parser = argparse.ArgumentParser ()
  parser.add_argument ('--requests', type=int, default=0, help='Number of requests to serve')
  parser.add_argument ('--port', metavar='N', type=int, default=8888, help='Port number')
  parser.add_argument ('--scopes', metavar='REMOTE_SCOPES', type=str, default=None, required=True, help='Results file for remote-scopes call')
  parser.add_argument ('result_files', metavar='RESULT_FILE', nargs='+', type=str)
  parser.add_argument ('--feedback', type=str, default=None, help='Dump feedback responses to a file')
  parser.add_argument ('--search', type=str, default=None, help='Dump search requests to a file')
  parser.add_argument ('--timeout', type=int, default=0, help='Quit after reaching timeout (in seconds)')
  args = parser.parse_args ()

  app = FakeServerApp (args.scopes, args.result_files, args.search, args.feedback)
  srv = make_server('', args.port, app)

  if args.timeout > 0:
    signal.signal (signal.SIGALRM, timeout_handler)
    signal.alarm (args.timeout)

  if args.requests > 0:
    for i in range (0, args.requests):
      srv.handle_request()
  else:
    srv.serve_forever ()

if __name__ == '__main__':
  main ()
