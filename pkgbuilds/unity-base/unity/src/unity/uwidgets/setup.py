#!/usr/bin/env python3

"""
This file is part of "uwidgets" which is released under GPL.

See file LICENCE or go to http://www.gnu.org/licenses/ for full license
details.

uwidgets is a desktop widget creation and management library for Python 3.

Copyright (c) 2022 Rudra Saraswat <rs2009@ubuntu.com>.
Copyright (c) 2018 Gabriele N. Tornetta <phoenix1987@gmail.com>.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

from setuptools import Extension, find_packages, setup

x11 = Extension('uwidgets._x11',
    include_dirs       = ['/usr/include/cairo/'],
    libraries          = ['cairo', 'X11', 'Xinerama'],
    extra_compile_args = ['-std=c99'],
    sources            = [
        'uwidgets/x11/_x11module.c',
        'uwidgets/x11/atelier.c',
        'uwidgets/x11/base_canvas.c',
    ]
)


setup(
    name             = 'uwidgets',
    version          = '1.0.0',
    description      = 'Desktop Widget Manager for Unity, based on Blighty.',
    author           = 'Rudra Saraswat',
    author_email     = 'rs2009@ubuntu.com',
    url              = 'https://unityd.org',
    classifiers=[
        'Development Status :: 5 - Production/Stable',

        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',

        'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',

        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.11',
    ],
    keywords         = 'desklet widget infotainment',
    packages         = find_packages(exclude=['contrib', 'docs']),
    ext_modules      = [x11],
    install_requires = ['pycairo'],
    scripts          = ['uwidgets-runner'],
)
