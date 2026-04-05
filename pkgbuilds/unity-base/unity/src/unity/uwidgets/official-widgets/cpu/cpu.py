#!/usr/bin/env python3

from math import pi as PI

import configparser
import psutil
import cairo
import os
from uwidgets import CanvasGravity, TextAlign
from uwidgets.legacy import Graph
from uwidgets.x11 import Canvas, start_event_loop


class AttrDict(dict):
    def __init__(self, *args, **kwargs):
        super(AttrDict, self).__init__(*args, **kwargs)
        self.__dict__ = self

class Fonts:
    UBUNTU_NORMAL = "Ubuntu", cairo.FontSlant.NORMAL, cairo.FontWeight.NORMAL


class Cpu(Canvas):
    SIZE = (256, 256)
    CORE_POLYGON = AttrDict({"height": 30, "length": 20})

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        with open('/proc/cpuinfo', 'r') as fin:
            raw_cpuinfo = fin.read().strip()

        self.coreinfo = [
            {
                k.strip(): v
                for k, v in [p.split(":") for p in core.split('\n')]
            }
            for core in raw_cpuinfo.split('\n\n')
        ]

        self.graph = Graph(0, 110, self.width, 40)

    @staticmethod
    def build(x = 0, y = 0, gravity = CanvasGravity.CENTER):
        return Cpu(x, y, *Cpu.SIZE, gravity = gravity, interval = 2000)

    def on_button_pressed(self, button, *args):
        os.system('stacer')

    def draw_polygon(c, n, x, y, size):
        a = 2 * PI / n

        c.save()

        c.translate(x, y)
        c.move_to(size, 0)
        for i in range(n):
            c.rotate(a)
            c.line_to(size, 0)
        c.stroke()

        c.restore()

    def draw_core_polygon(c, x, y):
        size = Cpu.CORE_POLYGON.height
        length = Cpu.CORE_POLYGON.length

        c.save()

        c.translate(x, y)

        c.set_source_rgb(.8, .8, .8)

        cpus = psutil.cpu_percent(0.1, percpu = True)
        n = len(cpus)
        a = 2 * PI / n

        c.set_line_width(1)
        c.draw_polygon(n, 0, 0, size)

        c.set_line_width(2)
        c.set_source_rgb(1, 1, 1)
        c.move_to(size + length * cpus[-1] / 100, 0)
        for i in range(n):
            c.rotate(a)
            c.line_to(size + length * cpus[i] / 100, 0)
        c.stroke()

        value = int(sum(cpus) / n)
        c.canvas.graph.push_value(value)

        c.set_font_size(18)
        c.write_text(0, 0, '{}%'.format(value), TextAlign.CENTER_MIDDLE)

        c.restore()

        return value

    def draw_processes(c):
        ps = [
            p.info
            for p in psutil.process_iter(attrs=['pid', 'name', 'cpu_percent'])
        ]

        ps = sorted(ps, key=lambda p: p["cpu_percent"], reverse=True)[:5]

        y = 170
        c.save()
        c.select_font_face(*Fonts.UBUNTU_NORMAL)
        c.set_font_size(12)
        for p in ps:
            c.write_text(48, y, str(p["pid"]), align = TextAlign.TOP_RIGHT)
            c.write_text(52, y, p["name"][:24])
            c.write_text(
                c.canvas.width - 15, y, "{}%".format(p["cpu_percent"]),
                align=TextAlign.TOP_RIGHT
            )
            y += 18

        c.restore()

    def draw_cpu_name(c):
        c.save()
        c.set_font_size(12)
        c.write_text(
            15, 110,
            c.canvas.coreinfo[0]["model name"].strip()
            .replace("(TM)", "™")
            .replace("(R)", "©")
        )
        c.restore()

    def draw_background(c):
        size = c.canvas.get_size()
        c.rectangle(0, 0, *size)
        c.set_source_rgba(0, 0, 0, 0.6)
        c.fill()

        c.rectangle(0, 0, c.canvas.width, 3)
        c.set_source_rgba(1, 1, 1, 1)
        c.fill()

    def on_draw(self, c):
        c.draw_background()

        c.select_font_face(*Fonts.UBUNTU_NORMAL)
        c.set_font_size(36)
        c.set_source_rgb(1, 1, 1)

        w, h = Cpu.SIZE

        y_poly = (Cpu.CORE_POLYGON.height + Cpu.CORE_POLYGON.length)

        c.write_text(15, y_poly, "CPU", align = TextAlign.TOP_LEFT)
        c.draw_core_polygon(w - y_poly, y_poly)
        c.draw_processes()
        c.draw_cpu_name()

        c.set_source_rgb(1, 1, 1)
        self.graph.draw(c)


if __name__ == "__main__":
    config = configparser.ConfigParser()
    config.read('settings.ini')
    Cpu.build(int(config.get('settings', 'margin_x')),
              int(config.get('settings', 'margin_y')),
              gravity = getattr(CanvasGravity, config.get('settings', 'gravity'))).show()
    start_event_loop()
