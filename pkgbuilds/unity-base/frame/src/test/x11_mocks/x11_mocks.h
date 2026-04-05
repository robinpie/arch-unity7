#ifndef X11_MOCKS_H
#define X11_MOCKS_H

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/sync.h>
#include <stdint.h>
#include <map>

extern int xmock_xi2_opcode;
extern int xmock_xi2_event_base;
extern int xmock_xi2_error_base;
extern int xmock_xsync_event_base;
extern int xmock_xsync_error_base;
extern Display *xmock_display;
extern uint64_t xmock_server_time;

/* to be filled by user. A copy of it will be returned by each
   XIQueryDevice call  */
extern XIDeviceInfo *xmock_devices;
extern int xmock_devices_count;

/* Adds the given XEvent to the xmock event queue.
   The Diplay connection will signal that there are
   pending events */
extern void xmock_add_to_event_queue(const XEvent* event);

/*
  Each call to:
  XIAllowTouchEvents(display, device_id, touch_id, window_id, touch_acceptance)
  Causes:
  xmock_touch_acceptance[device_id][touch_id][window_id] = touch_acceptance;
 */
extern std::map<int, std::map<unsigned int, std::map<Window, int> > > xmock_touch_acceptance;

/*
  Returns -1 if XIAllowTouchEvents wasn't called with the given parameters.
 */
int xmock_get_touch_acceptance(int device_id, unsigned int touch_id, Window window);

#endif
