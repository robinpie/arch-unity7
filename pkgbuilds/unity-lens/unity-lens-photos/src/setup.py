#!/usr/bin/env python
#
from distutils.core import setup
from DistUtilsExtra.command import build_extra
import build_i18n_ext as build_i18n

setup(name="unity-lens-photos",
      version="1.0",
      author="David Calle",
      author_email="davidc@framli.eu",
      url="http://launchpad.net/unity-lens-photos",
      license="GNU General Public License (GPL)",
      data_files=[
    ('share/unity-scopes/shotwell', ['src/unity_shotwell_daemon.py']),
    ('share/dbus-1/services', ['data/unity-scope-shotwell.service']),
    ('share/applications', ['unity-lens-photos.desktop']),
    ('share/pixmaps', ['unity-lens-photos.png']),
    ], cmdclass={"build":  build_extra.build_extra,
                 "build_i18n": build_i18n.build_i18n,})
