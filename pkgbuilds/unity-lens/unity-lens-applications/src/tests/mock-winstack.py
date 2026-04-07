#!/usr/bin/env python3
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
# This is a simple simulator/mock of com.canonical.Unity.WindowStack DBus service,
# meant for testing of unity-scope-runningapps. It doesn't implement all the API calls -
# only what's needed by the scope.
#
# Usage:
# - to run indefinitely, with window events every 2 secs.
# ./mock-winstack.py
#
# - to run with specific number of simulation steps and given interval:
# ./mock-winstack.py <interval> <steps>
#   (where interval is in seconds; use steps = 0 for infinity).
#


import dbus
import dbus.service
import sys
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib, GObject

WINSTACK_DBUS_IFACE = "com.canonical.Unity.WindowStack"
WINSTACK_DBUS_NAME = "com.canonical.Unity.WindowStack"
WINSTACK_DBUS_PATH = "/com/canonical/Unity/WindowStack"

START = 0
FOCUS = 1
CLOSE = 2
APP_WEB = 'org.mozilla.firefox'
APP_EDITOR = 'org.gnome.gedit'
APP_NOTES = 'com.canonical.notes' 
APP_GALLERY = 'com.canonical.gallery'

main_loop = GLib.MainLoop()

class WinStackSim (GObject.GObject):
    
    __gsignals__ = {
        'window_created': (GObject.SIGNAL_RUN_FIRST, None, (int, str,)),
        'window_destroyed': (GObject.SIGNAL_RUN_FIRST, None, (int, str,)),
        'focused_window_changed': (GObject.SIGNAL_RUN_FIRST, None, (int, str, int))
    }
    #
    # window events to be generated in every simulation step;
    # every step contains at least one event.
    _SIM_STEPS = (
        (
            (START, APP_WEB),
            (START, APP_NOTES),
            (FOCUS, APP_WEB)
        ),
        (
            (START, APP_EDITOR),
        ),
        (
            (FOCUS, APP_NOTES),
        ),
        (
            (FOCUS, APP_WEB),
        ),
        (
            (CLOSE, APP_WEB),
            (FOCUS, APP_NOTES)
        ),
        (
            (CLOSE, APP_NOTES),
        ),
        (
            (START, APP_GALLERY),
            (FOCUS, APP_GALLERY)
        ),
        (
            (FOCUS, APP_EDITOR),
        ),
        (
            (CLOSE, APP_EDITOR),
            (FOCUS, APP_GALLERY),
            (CLOSE, APP_GALLERY)
        )
        # make sure that the last step leaves window stack in a state
        # that makes sense when all the steps are repeated
        # in infinite loop.
    )
    
    def __init__(self):
        GObject.GObject.__init__(self)
        self._windows = []
        self._total_step = 0
        self._sim_step = 0
        self._steps = 0
        self._focused = None

    def start_sim(self, interval_secs, steps = 0):
        self._sim_step = 0
        self._steps = steps
        GLib.timeout_add_seconds(interval_secs, self.do_sim_step)

    def do_sim_step(self):
        sim_data = self._SIM_STEPS[self._sim_step]
        for s in sim_data:
            if s[0] == START:
                self.emit('window_created', 0, s[1])
                self._windows.append(s[1])
            elif s[0] == CLOSE:
                self.emit('window_destroyed', 0, s[1])
                self._windows.remove(s[1])
            elif s[0] == FOCUS:
                self._focused = s[1]
                self.emit('focused_window_changed', 0, s[1], 0)

        self._total_step += 1
        self._sim_step = (self._sim_step + 1) % len(self._SIM_STEPS)
        if self._steps > 0 and self._total_step >= self._steps:
            main_loop.quit()
        return True

    def get_window_stack(self):
        return [(0, win, self._focused == win, 0) for win in self._windows]

class WinStackServiceMock (dbus.service.Object):
    
    def __init__(self, sim):
        dbus.service.Object.__init__(self)
        self._sim = sim
        self._sim.connect('window_created', self.on_window_created)
        self._sim.connect('window_destroyed', self.on_window_destroyed)
        self._sim.connect('focused_window_changed', self.on_focused_window_changed)
        
        bus_name = dbus.service.BusName(WINSTACK_DBUS_NAME, bus=dbus.SessionBus())
        dbus.service.Object.__init__(self, bus_name, WINSTACK_DBUS_PATH)

    def on_window_created(self, sim, window_id, app_id):
        self.WindowCreated(window_id, app_id)

    def on_window_destroyed(self, sim, window_id, app_id):
        self.WindowDestroyed(window_id, app_id)

    def on_focused_window_changed(self, sim, window_id, app_id, stage):
        self.FocusedWindowChanged(window_id, app_id, stage)

    @dbus.service.signal(dbus_interface=WINSTACK_DBUS_IFACE, signature='us')
    def WindowCreated(self, window_id, app_id):
        pass

    @dbus.service.signal(dbus_interface=WINSTACK_DBUS_IFACE, signature='us')
    def WindowDestroyed(self, window_id, app_id):
        pass

    @dbus.service.signal(dbus_interface=WINSTACK_DBUS_IFACE, signature='usu')
    def FocusedWindowChanged(self, window_id, app_id, stage):
        pass
        
    @dbus.service.method(WINSTACK_DBUS_NAME, out_signature='a(usbu)')
    def GetWindowStack(self):
        return self._sim.get_window_stack()

##################################################### main #######################################################
        
interval = 2
steps = 0        
if len(sys.argv) > 2:
    (interval, steps) = int(sys.argv[1]), int(sys.argv[2])

print("Starting WinStack mock service with interval = %d and %s simulation steps" % (interval, steps if steps > 0 else 'unlimited'))
    
sim = WinStackSim()
DBusGMainLoop(set_as_default=True)
myservice = WinStackServiceMock(sim)
sim.start_sim(interval, steps)
main_loop.run()
