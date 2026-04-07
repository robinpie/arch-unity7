#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
Setup file for chromiumbookmarks dash plugin
'''

from distutils.core import setup
from DistUtilsExtra.command import build_extra
from DistUtilsExtra.command import build_i18n

setup(name='unity-scope-chromiumbookmarks',
      version='0.1',
      author='Mark Tully',
      author_email='markjtully@gmail.com',
      url='http://launchpad.net/ubuntu-scopes',
      license='GNU General Public License (GPL)',
      data_files=[('share/dbus-1/services', ['data/unity-scope-chromiumbookmarks.service']),
                  #('share/icons/unity-icon-theme/places/svg', ['data/icons/service-chromiumbookmarks.svg']),
                  ('share/unity-scopes/chromiumbookmarks', ['src/unity_chromiumbookmarks_daemon.py']),
                  ('share/unity-scopes/chromiumbookmarks', ['src/__init__.py']), ],
      cmdclass={'build': build_extra.build_extra,
                'build_i18n': build_i18n.build_i18n, })
