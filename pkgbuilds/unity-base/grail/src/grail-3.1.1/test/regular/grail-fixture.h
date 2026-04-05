#ifndef GRAIL_FIXTURE_H
#define GRAIL_FIXTURE_H

#include <gtest/gtest.h>
#include <oif/grail.h>
#include <oif/frame_backend.h>

#include <functional>
#include <list>
#include <set>

class GrailGesture
{
  public:
  bool HasTouch(UFTouchId touch_id) const
  {
    return touches.find(touch_id) != touches.end();
  }
  unsigned int id;
  std::set<UFTouchId> touches;
  bool construction_finished;
  UGGestureState state;
};

class GrailTest : public ::testing::Test
{
  public:
  virtual void SetUp()
  {
    grail_handle = NULL;
    previous_frame_ = nullptr;
    CreateFakeDevice();
  }

  virtual void TearDown()
  {
    if (grail_handle)
      grail_delete(grail_handle);

    if (previous_frame_)
      frame_backend_frame_delete(previous_frame_);

    frame_backend_device_delete(device_);
  }

  protected:

  UGSubscription CreateSubscription(unsigned int num_touches,
      UGGestureTypeMask gesture_mask, UFWindowId window_id);

  void CreateFakeDevice();
  void SendDeviceAddedEvent(uint64_t time);
  void SendFrameEvent(uint64_t time, UFBackendFrame frame);

  void BeginTouch(int touch_id);
  void BeginTouchWindowCoords(int touch_id, float window_x, float window_y);
  void GiveTouchOwnership(int touch_id);
  void SetTouchWindowCoords(UFBackendFrame frame,
                            int touch_id, float window_x, float window_y);
  void SendTouchPendingEnd(int touch_id);
  void EndTouch(int touch_id);
  void UpdateTouch(int touch_id, std::function< void(UFBackendTouch) >& update_func);

  /* Fetches and processes all pending grail events, updating grail_gestures
     list accordingly. */
  void ProcessGrailEvents();
  GrailGesture *GestureWithId(unsigned int id);

  /* Helper function for debugging purposes.
     Prints all gestures in grail's queue.
   */
  void PrintPendingGestures();
  void PrintSlice(UGSlice slice);

  UGHandle grail_handle;
  UFBackendDevice device_;

  uint64_t time;
  UFWindowId fake_window_id;
  UFBackendFrame previous_frame_;

  /*
    A representation of the currently active gestures according to the grail events
    fetched by ProcessGrailEvents()
   */
  std::list<GrailGesture> grail_gestures;

  private:
  void ProcessSlice(UGSlice slice);
  void AddNewGesture(UGSlice slice);
  void UpdateGesture(UGSlice slice);
  void EndGesture(UGSlice slice);
};

#endif // GRAIL_FIXTURE
