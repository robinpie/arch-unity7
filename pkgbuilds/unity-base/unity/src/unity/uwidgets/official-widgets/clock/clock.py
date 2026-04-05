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

from uwidgets import CanvasGravity, brush
from uwidgets.x11 import Canvas, start_event_loop

import datetime
import subprocess
import configparser

from math import pi as PI


class Clock(Canvas):
    def on_button_pressed(self, button, state, x, y):
        if button == 3:    # Right button
            subprocess.run(['unity-control-center', 'datetime'])

    def draw_circle_background(ctx):
        ctx.arc(2, 1, 90, 0, 2*PI)
        ctx.set_source_rgba(0, 0, 0, 0.6)
        ctx.fill()

    def draw_rect_background(ctx):
        ctx.rectangle(-180, -180, 360, 360)
        ctx.set_source_rgba(0, 0, 0, 0.6)
        ctx.fill()

        ctx.rectangle(-180, -100, 360, 3)
        ctx.set_source_rgba(1, 1, 1, 1)
        ctx.fill()

    @brush
    def hand(ctx, angle, length, thickness):
        ctx.save()
        ctx.set_source_rgba(1, 1, 1, 1)
        ctx.set_line_width(thickness)
        ctx.rotate(angle)
        ctx.move_to(0, length * .2)
        ctx.line_to(0, -length)
        ctx.stroke()
        ctx.restore()

    def on_draw(self, ctx):
        now = datetime.datetime.now()

        ctx.translate(self.width >> 1, self.height >> 1)

        getattr(ctx, f"draw_{config.get('settings', 'clock_style')}_background")()

        ctx.hand(
            angle = now.second / 30 * PI,
            length = ((self.height >> 1) * .9) - 20,
            thickness = 1
        )

        mins = now.minute + now.second / 60
        ctx.hand(
            angle = mins / 30 * PI,
            length = ((self.height >> 1) * .8) - 20,
            thickness = 3
        )

        hours = (now.hour % 12) + mins / 60
        ctx.hand(
            angle = hours / 6 * PI,
            length = ((self.height >> 1) * .5) - 20,
            thickness = 4.5
        )

if __name__ == "__main__":
    config = configparser.ConfigParser()
    config.read('settings.ini')
    clock = Clock(int(config.get('settings', 'margin_x')),
                  int(config.get('settings', 'margin_y')),
                  200,
                  200,
                  gravity = getattr(CanvasGravity, str(config.get('settings', 'gravity'))))
    clock.show()
    start_event_loop()
