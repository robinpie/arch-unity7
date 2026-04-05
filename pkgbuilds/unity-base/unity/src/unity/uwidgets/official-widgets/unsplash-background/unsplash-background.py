#!/usr/bin/env python3

"""
This file is part of "blighty" and "uwidgets" which is released under GPL.

See file LICENCE or go to http://www.gnu.org/licenses/ for full license
details.

uwidgets is a desktop widget creation and management library for Python 3.

Copyright (c) 2022 Rudra Saraswat <rs2009@ubuntu.com>.
Copyright (c) 2018 Gabriele N. Tornetta <phoenix1987@gmail.com>.
All rights reserved.

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

from uwidgets import CanvasGravity
from uwidgets.x11 import Canvas, start_event_loop
from uwidgets.settings import UnityWallpaper

import cairo
import subprocess
import configparser

from gi.repository import GLib


class Fonts:
    UBUNTU_NORMAL = "Ubuntu", cairo.FontSlant.NORMAL, cairo.FontWeight.NORMAL


class UnBackground(Canvas):
    def on_button_pressed(self, button, state, x, y):
        if button == 1:    # Left button
            self.change_wallpaper()
        elif button == 3:  # Right button
            subprocess.run(['unity-control-center', 'appearance'])

    def change_wallpaper(ctx):
        UnityWallpaper().set_wallpaper_from_url("https://source.unsplash.com/random/3840x2160")

    def draw_background(ctx):
        size = ctx.canvas.get_size()
        ctx.rectangle(0, 0, *size)
        ctx.set_source_rgba(0, 0, 0, 0.6)
        ctx.fill()

    def draw_decoration(ctx):
        ctx.rectangle(0, 0, ctx.canvas.width, 3)
        ctx.set_source_rgba(1, 1, 1, 1)
        ctx.fill()

    def draw_info(ctx):
        ctx.set_source_rgb(0.9, 0.9, 0.9)

        ctx.select_font_face(*Fonts.UBUNTU_NORMAL)
        ctx.set_font_size(24)
        ctx.move_to(20, 42)
        ctx.show_text('Unsplash | Random Background')

        ctx.set_source_rgb(0.5, 0.5, 0.5)
        ctx.set_font_size(15)
        ctx.move_to(20, 64)
        ctx.show_text('Click this to set a random Unsplash background (requires internet).')

    def on_draw(self, ctx):
        ctx.draw_background()
        ctx.draw_decoration()
        ctx.draw_info()


if __name__ == "__main__":
    config = configparser.ConfigParser()
    config.read('settings.ini')
    unbg = UnBackground(
        x = int(config.get('settings', 'margin_x')),
        y = int(config.get('settings', 'margin_y')),
        width = 540,
        height = 80,
        gravity = getattr(CanvasGravity, config.get('settings', 'gravity')),
        interval = 1000
    )

    unbg.show()
    start_event_loop()
