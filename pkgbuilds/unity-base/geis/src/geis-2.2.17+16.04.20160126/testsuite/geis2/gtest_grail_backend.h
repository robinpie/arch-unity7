#ifndef GTEST_GRAIL_BACKEND_H
#define GTEST_GRAIL_BACKEND_H

#include <gtest/gtest.h>
#include <geis/geis.h>
#include <ostream>
#include <vector>

class Touch
{
 public:
  Touch(GeisTouch geis_touch);

  int id;
  float x;
  float y;
  friend std::ostream& operator<<(std::ostream& os, const Touch& touch);
};

class TouchSet : public std::vector<Touch>
{
 public:
  TouchSet(GeisTouchSet geis_touch_set);
  bool contains(float x, float y);
  friend std::ostream& operator<<(std::ostream& os, const TouchSet& touch_set);
};

class Geis2GrailBackendBase : public ::testing::Test
{
 protected:
  Geis2GrailBackendBase() : _geis(nullptr),
                            _xevent_serial_number(0) {}
  virtual ~Geis2GrailBackendBase() {}

  /* Dispatch and process Geis events in a loop until there are no more
     events. */
  void Run();


  /* Creates (and activates) a GeisSubscription with the given name
     and with filters for the given number of touches and gesture class. */
  GeisSubscription CreateFilteredSubscription(GeisString name,
                                              GeisSize num_touches,
                                              GeisString gesture_class);

  void CreateXMockTouchScreenDevice();
  void DestroyXMockDevices();

  virtual void OnEventInitComplete(GeisEvent event) {}
  virtual void OnEventClassAvailable(GeisEvent event) {}
  virtual void OnEventGestureBegin(GeisEvent event) {}
  virtual void OnEventGestureUpdate(GeisEvent event) {}
  virtual void OnEventGestureEnd(GeisEvent event) {}

  void SendTouchEvent(int event_type, int touch_id, float x, float y);
  void SendTouchOwnershipEvent(int touch_id);

  void AcceptGestureInEvent(GeisEvent event);
  void RejectGestureInEvent(GeisEvent event);
  void GetGestureTimestampInEvent(GeisInteger *timestamp, GeisEvent event);

  Geis _geis;

  /* holds the serial number to be used on the next synthetic XEvent */
  int _xevent_serial_number;

 private:

  bool DispatchAndProcessEvents();

  void AcceptRejectGestureInEvent(GeisEvent event, bool accept);
};

#endif // GTEST_GRAIL_BACKEND_H
