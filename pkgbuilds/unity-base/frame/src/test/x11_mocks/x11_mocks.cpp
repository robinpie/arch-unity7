/* needed to break into 'Display' struct internals. */
#define XLIB_ILLEGAL_ACCESS
#include <X11/Xlib.h>

#include "x11_mocks.h"

#include <sys/eventfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int xmock_xi2_opcode = 42;
int xmock_xi2_event_base = 40000;
int xmock_xi2_error_base = 40000;
int xmock_xsync_event_base = 50000;
int xmock_xsync_error_base = 50000;
Display *xmock_display = NULL;
uint64_t xmock_server_time = 0;

XIDeviceInfo *xmock_devices = NULL;
int xmock_devices_count = 0;

std::map<int, std::map<unsigned int, std::map<Window, int> > > xmock_touch_acceptance;

/* id to be used for the next alarm that gets created */
XSyncAlarm _xmock_next_alarm = 1;

struct EventQueueItem
{
  XEvent event;
  struct EventQueueItem *next;
} *xmock_event_queue = NULL;

#define XMOCK_PRINT_FUNCTION _xmock_print_function(__func__)
void _xmock_print_function(const char *function)
{
  static int debug_enabled = -1;
  if (debug_enabled == -1)
  {
    if (getenv("XMOCK_DEBUG"))
      debug_enabled = 1;
    else
      debug_enabled = 0;
  }

  if (debug_enabled)
    printf("XMOCK: %s mock called.\n", function);
}

void xmock_add_to_event_queue(const XEvent *event)
{
  struct EventQueueItem *new_item = reinterpret_cast<struct EventQueueItem *>(malloc(sizeof(struct EventQueueItem)));
  new_item->event = *event;
  new_item->next = NULL;

  if (!xmock_event_queue)
  {
    xmock_event_queue = new_item;
  }
  else
  {
    struct EventQueueItem *last_item = xmock_event_queue;
    while (last_item->next)
    {
      last_item = last_item->next;
    }
    last_item->next = new_item;
  }

  static const uint64_t num = 1;
  if (write(xmock_display->fd, &num, sizeof(num)) != sizeof(num))
  {
    fprintf(stderr, "ERROR: failed to update eventfd instance,\n");
    exit(1);
  }
}

int xmock_get_touch_acceptance(int device_id, unsigned int touch_id, Window window)
{
  if (xmock_touch_acceptance.find(device_id) == xmock_touch_acceptance.end())
    return -1;

  std::map<unsigned int, std::map<Window, int> > &touch_map =
    xmock_touch_acceptance[device_id];

  if (touch_map.find(touch_id) == touch_map.end())
    return -1;

  std::map<Window, int> &window_map = touch_map[touch_id];

  if (window_map.find(window) == window_map.end())
    return -1;

  return window_map[window];
}

Display *XOpenDisplay(_Xconst char *display_name)
{
  XMOCK_PRINT_FUNCTION;
  (void)display_name;

  Display *display = (Display*)calloc(1, sizeof(Display));
  display->fd = eventfd(0, EFD_NONBLOCK);
  display->default_screen = 0;
  display->nscreens = 1;
  display->screens = (Screen*)calloc(1, sizeof(Screen));
  display->screens[0].root = 1;

  xmock_display = display;

  return display;
}

int XCloseDisplay(Display *display)
{
  XMOCK_PRINT_FUNCTION;

  close(display->fd);
  free(display->screens);
  free(display);

  xmock_display = NULL;

  return 0;
}

int XSync(Display *display, Bool discard)
{
  (void)display;
  (void)discard;
  return 0;
}

int XFlush(Display *display)
{
  (void)display;
  return 0;
}

Bool XQueryExtension(Display *display, const char *name,
    int *major_opcode_return, int *first_event_return, int *first_error_return)
{
  XMOCK_PRINT_FUNCTION;
  (void)display;
  (void)name; /* assuming name == "XInputExtension" */

  *major_opcode_return = xmock_xi2_opcode;
  *first_event_return = xmock_xi2_event_base;
  *first_error_return = xmock_xi2_error_base;

  return True;
}

int XPending(Display *display)
{
  XMOCK_PRINT_FUNCTION;
  (void)display;

  int pending_events_count = 0;
  struct EventQueueItem *item = xmock_event_queue;
  while (item != NULL)
  {
    ++pending_events_count;
    item = item->next;
  }
  return pending_events_count;
}

int XNextEvent(Display *display, XEvent *event_return)
{
  XMOCK_PRINT_FUNCTION;
  (void)display;


  if (xmock_event_queue)
  {
    uint64_t num = 1;
    ssize_t bytes_read = read(xmock_display->fd, &num, sizeof(num));
    (void)bytes_read;

    *event_return = xmock_event_queue->event;

    struct EventQueueItem *removed_item = xmock_event_queue;
    xmock_event_queue = xmock_event_queue->next;
    free(removed_item);
  }
  else
  {
    /* not going to block... */
  }

  return 0;
}

Bool XGetEventData(Display *display, XGenericEventCookie *cookie)
{
  XMOCK_PRINT_FUNCTION;
  (void)display;
  (void)cookie;
  return True;
}

void XFreeEventData(Display *display, XGenericEventCookie *cookie)
{
  XMOCK_PRINT_FUNCTION;
  (void)display;

  if (cookie->data && cookie->extension == xmock_xi2_opcode)
  {
    if (cookie->evtype == XI_TouchBegin
        || cookie->evtype == XI_TouchUpdate
        || cookie->evtype == XI_TouchEnd)
    {
      XIDeviceEvent *device_event = (XIDeviceEvent*) cookie->data;
      free(device_event->valuators.mask);
      free(device_event->valuators.values);
    }
    free(cookie->data);
  }
}

XIDeviceInfo* XIQueryDevice(Display * display,
                            int deviceid,
                            int * ndevices_return)
{
  XMOCK_PRINT_FUNCTION;
  (void)display;
  (void)deviceid; /* assuming XIAllDevices */

  XIDeviceInfo *devices;

  devices = reinterpret_cast<XIDeviceInfo*>(calloc(xmock_devices_count, sizeof(XIDeviceInfo)));

  for (int i = 0; i < xmock_devices_count; ++i) {
    devices[i] = xmock_devices[i];
  }

  *ndevices_return = xmock_devices_count;

  return devices;
}

void XIFreeDeviceInfo(XIDeviceInfo *info)
{
  XMOCK_PRINT_FUNCTION;
  free(info);
}

Status XIQueryVersion(Display *display,
                      int *major_version_inout,
                      int *minor_version_inout)
{
  XMOCK_PRINT_FUNCTION;
  (void)display;
  *major_version_inout = 2;
  *minor_version_inout = 2;
  return Success;
}

Status XISelectEvents(Display *display,
                      Window win,
                      XIEventMask *masks,
                      int num_masks)
{
  XMOCK_PRINT_FUNCTION;
  (void)display;
  (void)win;
  (void)masks;
  (void)num_masks;
  return Success;
}

int XIGrabTouchBegin(
    Display*            display,
    int                 deviceid,
    Window              grab_window,
    int                 owner_events,
    XIEventMask         *mask,
    int                 num_modifiers,
    XIGrabModifiers     *modifiers_inout)
{
  XMOCK_PRINT_FUNCTION;
  (void)display;
  (void)deviceid;
  (void)grab_window;
  (void)owner_events;
  (void)mask;

  for (int i = 0; i < num_modifiers; ++i)
  {
    modifiers_inout[i].status = XIGrabSuccess;
  }

  return 0;
}

Status XIUngrabTouchBegin(
    Display*            display,
    int                 deviceid,
    Window              grab_window,
    int                 num_modifiers,
    XIGrabModifiers     *modifiers)
{
  XMOCK_PRINT_FUNCTION;
  (void)display;
  (void)deviceid;
  (void)grab_window;
  (void)num_modifiers;
  (void)modifiers;
  return Success;
}

Status XIAllowTouchEvents(
    Display*            display,
    int                 deviceid,
    unsigned int        touchid,
    Window              grab_window,
    int                 event_mode)
{
  XMOCK_PRINT_FUNCTION;
  (void)display;
  xmock_touch_acceptance[deviceid][touchid][grab_window] = event_mode;
  return Success;
}

Status XSyncQueryExtension(
    Display* dpy,
    int* event_base_return,
    int* error_base_return)
{
  XMOCK_PRINT_FUNCTION;
  (void)dpy;
  *event_base_return = xmock_xsync_event_base;
  *error_base_return = xmock_xsync_error_base;
  return True;
}

Status XSyncInitialize(
    Display* dpy,
    int* major_version_return,
    int* minor_version_return)
{
  XMOCK_PRINT_FUNCTION;
  (void)dpy;
  *major_version_return = 1;
  *minor_version_return = 0;
  return True;
}

XSyncSystemCounter *XSyncListSystemCounters(
    Display* dpy,
    int* n_counters_return)
{
  XMOCK_PRINT_FUNCTION;
  (void)dpy;
  *n_counters_return = 1;

  XSyncSystemCounter *sys_counter = reinterpret_cast<XSyncSystemCounter*>(malloc(sizeof(XSyncSystemCounter)));
  sys_counter->name = const_cast<char*>("SERVERTIME"); // I know it's technically dangerous, but it's simple.
  sys_counter->counter = 1;
  sys_counter->resolution.hi = 1;
  sys_counter->resolution.lo = 0;
  return sys_counter;
}

void XSyncFreeSystemCounterList(XSyncSystemCounter* list)
{
  XMOCK_PRINT_FUNCTION;
  free(list);
}

XSyncAlarm XSyncCreateAlarm(
    Display* dpy,
    unsigned long values_mask,
    XSyncAlarmAttributes* values)
{
  XMOCK_PRINT_FUNCTION;
  (void)dpy;

  XSyncAlarmNotifyEvent alarm_notify;
  alarm_notify.type = xmock_xsync_event_base + XSyncAlarmNotify;
  alarm_notify.alarm = _xmock_next_alarm;
  alarm_notify.counter_value = values->trigger.wait_value;
  xmock_add_to_event_queue((XEvent*)&alarm_notify);

  XSyncValue time = values->trigger.wait_value;
  uint64_t timeout = (uint64_t)XSyncValueHigh32(time) << 32
                   | (uint64_t)XSyncValueLow32(time);
  xmock_server_time = timeout + 1;

  return _xmock_next_alarm++;
}

Status XSyncDestroyAlarm(
    Display* dpy,
    XSyncAlarm alarm)
{
  XMOCK_PRINT_FUNCTION;
  (void)dpy;
  (void)alarm;
  return Success;
}

void XSyncIntsToValue(
    XSyncValue* pv,
    unsigned int l,
    int h)
{
  XMOCK_PRINT_FUNCTION;
  pv->hi = h;
  pv->lo = l;
}

int XSyncValueHigh32(XSyncValue v)
{
  XMOCK_PRINT_FUNCTION;
  return v.hi;
}

unsigned int XSyncValueLow32(XSyncValue v)
{
  XMOCK_PRINT_FUNCTION;
  return v.lo;
}

Atom XInternAtom(Display* display, _Xconst char* atom_name, Bool only_if_exists)
{
  XMOCK_PRINT_FUNCTION;
  /* This function doesn't end up getting called when frame is run from tests yet.
     It's here just to make the code compile. Add some minimal logic when needed. */
  abort();
  return 1;
}


