/**
 * GTest test suite for GEIS v1 device handling.
 *
 * Copyright 2012 Canonical Ltd.
 */
#include "geis/geis.h"
#include "gtest_evemu_device.h"
#include "gtest_geis1_fixture.h"
#include "libgeis/geis_test_api.h"
#include <memory>

static const std::string TEST_DEVICE_PROP_FILE(
    TEST_ROOT_DIR "recordings/touchscreen_a/device.prop");
static const std::string TEST_DEVICE_EVENTS_FILE(
    TEST_ROOT_DIR "recordings/touchscreen_a/drag_2.record");

/**
 * Fixture for testing device handling.
 * This is a separate class because gtest uses Java reflection.
 */
class Geis1DeviceTests : public GTestGeis1Fixture
{
public:
  Geis1DeviceTests() : saw_events_(false) {}
  void GestureUpdate(GeisGestureType type, GeisGestureId id, GeisSize count,
                     GeisGestureAttr* attrs);

  void UseNewDevice();

protected:
  void CreateNewDevice();

  std::unique_ptr<Testsuite::EvemuDevice> new_device_;
  bool saw_events_;
};

namespace
{

void gesture_update_func(void* cookie, GeisGestureType type, GeisGestureId id,
                    GeisSize count, GeisGestureAttr* attrs) {
  Geis1DeviceTests* fixture = reinterpret_cast<Geis1DeviceTests*>(cookie);
  fixture->GestureUpdate(type, id, count, attrs);
}

void gesture_null_func(void* cookie, GeisGestureType type, GeisGestureId id,
                       GeisSize count, GeisGestureAttr* attrs) {
}

void device_added_func(void* cookie, GeisInputDeviceId deviceId, void* attrs) {
  Geis1DeviceTests* fixture = reinterpret_cast<Geis1DeviceTests*>(cookie);
  fixture->UseNewDevice();
}

void device_null_func(void* cookie, GeisInputDeviceId deviceId, void* attrs) {
}

} // namespace

void Geis1DeviceTests::GestureUpdate(GeisGestureType type, GeisGestureId id,
                                     GeisSize count, GeisGestureAttr* attrs)
{
  saw_events_ = true;
}

void Geis1DeviceTests::CreateNewDevice() {
  new_device_.reset(new Testsuite::EvemuDevice(TEST_DEVICE_PROP_FILE));
}

void Geis1DeviceTests::UseNewDevice() {
  new_device_->play(TEST_DEVICE_EVENTS_FILE);
}

/*
 * Test case for lp:1009270 -- Geis v1 does not report gestures for new devices
 *
 * Creates a subscription before a device is present. A device is added, and
 * then events are played. Gesture events should be detected.
 */
TEST_F(Geis1DeviceTests, addDeviceSubscription)
{

  static const char* gestures[] =
  {
    GEIS_GESTURE_TYPE_DRAG2,
    NULL
  };

  static GeisGestureFuncs callbacks = {
    &gesture_null_func,
    &gesture_null_func,
    &gesture_null_func,
    &gesture_update_func,
    &gesture_null_func,
  };

  static GeisInputFuncs inputfuncs = {
    &device_added_func,
    &device_null_func,
    &device_null_func,
  };

  ASSERT_EQ(GEIS_STATUS_SUCCESS, geis_input_devices(geis(), &inputfuncs, this));

  ASSERT_EQ(GEIS_STATUS_SUCCESS,
            geis_subscribe(geis(), GEIS_ALL_INPUT_DEVICES, gestures, &callbacks,
                           this));

  CreateNewDevice();

  while (1) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(geis_fd(), &read_fds);
    timeval tmo = { 1, 0 };
    int sstat = select(geis_fd() + 1, &read_fds, NULL, NULL, &tmo);
    EXPECT_GT(sstat, -1) << "error in select";
    if (sstat == 0)
      break;
    ASSERT_EQ(GEIS_STATUS_SUCCESS, geis_event_dispatch(geis()));
  }

  EXPECT_TRUE(saw_events_);
}
