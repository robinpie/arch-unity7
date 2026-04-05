#!/usr/bin/python
#
# When a device filter term is added to a subscription and there is no device
# present to match the filter, the term causes ALL devices to be selected
# instead of NONE. This is the reverse to how filters should work.
#
# The error occurred when filtering on  any of GEIS_DEVICE_ATTRIBUTE_NAME,
# GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH, or GEIS_DEVICE_ATTRIBUTE_INDEPENDENT_TOUCH
# attributes.
#
# Here's how the test works.  There are two devices:  device1 is the device on
# which all input is actaully occurring.  We have subscribed to device2 by name
# but not device1, so we shouldn;t be seeing any gesture events coming in.  If
# we do, the bug has obtained and we fail.  If we don't and the timer goes off,
# assume success.
#
# Just to assure everything is working, there's an option to subscribe to
# device1 as well, in which case we should see events only on device1.
#
# Because this test tool talks to kernel input devices (through evdev), it must
# be run as root.
#

#
# Copyright 2011 Canonical Ltd.
#
# This program is free software: you can redistribute it and/or modify it 
# under the terms of the GNU General Public License version 3, as published 
# by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranties of 
# MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
# PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along 
# with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import argparse
import evemu
import geis
import glib
import os
import sys


class Tester(object):

    def __init__(self, args):
        super(Tester, self).__init__()
        self._args = args
        self._has_filter = False
        self._device_1_id = 0

        # need to guarabtee there is a device we can feed events to
        self._device1 = evemu.Device(
            self._find_data_file("../recordings/test_device1/device.prop"))
        device2 = evemu.Device(
            self._find_data_file("../recordings/test_device2/device.prop"))
        self._device_2_name = device2.name

        # need a geis instance
        self._geis = geis.Geis(geis.GEIS_INIT_TRACK_DEVICES)
        self._sub = geis.Subscription(self._geis)


    def _find_data_file(self, filename):
      f = os.path.join(os.path.dirname(os.path.realpath(sys.argv[0])), filename)
      return open(f)

    def get_fd(self):
        return self._geis.get_configuration(geis.GEIS_CONFIGURATION_FD)

    
    def create_subscriptions(self):
        """
        Creates a filter to allow only gesture events from device2.
        """
        print "create_subscriptions"
        filt = geis.Filter(self._geis, "lp-891731")
        filt.add_term(geis.GEIS_FILTER_DEVICE,
            (geis.GEIS_DEVICE_ATTRIBUTE_NAME,
             geis.GEIS_FILTER_OP_EQ,
             self._device_2_name))
        self._sub.add_filter(filt)
        self._has_filter = True
        print ".. self._args.force=%s" % self._args.force
        if self._args.force:
            filt = geis.Filter(self._geis, "force")
            filt.add_term(geis.GEIS_FILTER_DEVICE,
                (geis.GEIS_DEVICE_ATTRIBUTE_NAME,
                 geis.GEIS_FILTER_OP_EQ,
                 self._device1.name))
            self._sub.add_filter(filt)
       

    def do_init_complete(self, event):
        """
        Creates a subscription ONLY on the second device and generates input
        ONLY from the first device.
        """
        print "do_init_complete"
        self.create_subscriptions()
        self._sub.activate()
        self._device1.play(
            self._find_data_file("../recordings/test_device1/events.record"))


    def do_device_available(self, event):
        """
        Verifies and remembers the two devices as their reported.
        """
        device = event.attrs()[geis.GEIS_EVENT_ATTRIBUTE_DEVICE]
        print "do_device_available: %s" % device.name()
        if device.name() == self._device1.name:
            self._device_1_id = device.id()
            print ".. device id = %s" % self._device_1_id

    def do_gesture_event(self, event):
        print "do_gesture_event"
        if not (self._device_1_id > 0 and self._has_filter):
            print "devices and filter not set up before other events received."
            sys.exit(1)
        if self._args.dry_run:
            return
        groupset = event.attrs()[geis.GEIS_EVENT_ATTRIBUTE_GROUPSET]
        if self._args.force:
            for group in groupset:
                for frame in group:
                    for (k, v) in frame.attrs().iteritems():
                        if (k == geis.GEIS_GESTURE_ATTRIBUTE_DEVICE_ID
                           and v != self._device_1_id):
                            print "test fail: incorrect device ID, expected %d got %d" % (self._device_1_id, v)
                            sys.exit(1)
            print "test successful (forced)"
            sys.exit(0)
        print "test failed: unsubscribed events received"
        sys.exit(1)


    def do_other_event(self, event):
        print "do_other_event"


    def dispatch_events(self):
        """ Performs GEIS event loop processing. """
        print "Tester.dispatch_events"
        geis_event_action = {
            geis.GEIS_EVENT_INIT_COMPLETE:    self.do_init_complete.im_func,
            geis.GEIS_EVENT_GESTURE_BEGIN:    self.do_gesture_event.im_func,
            geis.GEIS_EVENT_GESTURE_UPDATE:   self.do_gesture_event.im_func,
            geis.GEIS_EVENT_GESTURE_END:      self.do_gesture_event.im_func,
            geis.GEIS_EVENT_DEVICE_AVAILABLE: self.do_device_available.im_func,
        }

        status = self._geis.dispatch_events()
        while status == geis.GEIS_STATUS_CONTINUE:
            status = self._geis.dispatch_events()

        try:
            while True:
                event = self._geis.next_event()
                geis_event_action.get(event.type(),
                                      self.do_other_event.im_func)(self, event)
        except geis.NoMoreEvents:
            pass
        return True


def dispatch_test_events(fd, condition, g):
    print "dispatch_test_events"
    return g.dispatch_events()


def test_timeout():
    print "timed out, assuming success"
    exit(0)


class Options(argparse.ArgumentParser):

    def __init__(self):
        super(Options, self).__init__(description='test case for LP:891731')
        self.add_argument('-d', '--dry-run', action='store_true', dest='dry_run',
              help='force the test to bypass by subscribing to no input source')
        self.add_argument('-f', '--force', action='store_true', dest='force',
              help='force the test to pass by subscribing to the input source')
        self.add_argument('-t', '--timeout', action='store', default='5000',
              dest='timeout', type=int,
              help='milliseconds to wait before ending test')

if __name__ == '__main__':
    try:
        options = Options()
        args = options.parse_args()

        tester = Tester(args)
        ml = glib.MainLoop()
        glib.timeout_add(args.timeout, test_timeout)
        glib.io_add_watch(tester.get_fd(),
                          glib.IO_IN,
                          dispatch_test_events,
                          tester)
        ml.run()

    except KeyboardInterrupt:
        pass

    except argparse.ArgumentError as ex:
        print ex
        sys,exit(1)
