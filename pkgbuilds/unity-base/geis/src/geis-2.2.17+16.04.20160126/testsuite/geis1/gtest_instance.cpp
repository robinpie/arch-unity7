/**
 * Gtest test suite for GEIS v1 instances.
 *
 * Copyright 2012 Canonical Ltd.
 */
#include "geis_config.h"

#define _XOPEN_SOURCE 600
# include <stdlib.h>
#undef _XOPEN_SOURCE
#include "geis/geis.h"
#include "gtest_evemu_device.h"
#include <gtest/gtest.h>
#include <xorg/gtest/xorg-gtest.h>


namespace
{

const std::string TEST_DEVICE_PROP_FILE(TEST_ROOT_DIR "recordings/apple_magic_trackpad/device.prop");


/*
 * A special fixture that does not have any GEIS instances yet.
 */
class Geis1InstanceTests
: public xorg::testing::Test
{
public:
  Geis1InstanceTests()
  : evemu_device_(TEST_DEVICE_PROP_FILE), device_count_(0)
  { }

  void device_seen()
  { ++device_count_; }

  int device_seen_count() const
  { return device_count_; }

private:
  Testsuite::EvemuDevice evemu_device_;
  int                    device_count_;
};


/*
 * Regression test for lp:973539.
 *
 * The problem would only occur if an appropriate X server was
 * unavailable or did not support the required XI2.2 functionality.
 *
 */
TEST_F(Geis1InstanceTests, noX11Server)
{
  char* old_display = getenv("DISPLAY");
  ASSERT_TRUE(old_display != NULL);
  unsetenv("DISPLAY");

  GeisXcbWinInfo xcb_win_info = { NULL, NULL, 0 };
  GeisWinInfo win_info = { GEIS_XCB_FULL_WINDOW, &xcb_win_info };
  GeisInstance geis;
  ASSERT_NE(GEIS_STATUS_SUCCESS, geis_init(&win_info, &geis));

  setenv("DISPLAY", old_display, ~0);
}

static void
input_device_added(void              *context,
                   GeisInputDeviceId  device_id GEIS_UNUSED,
                   void              *attrs GEIS_UNUSED)
{
  Geis1InstanceTests* fixture = static_cast<Geis1InstanceTests*>(context);
  fixture->device_seen();
}


static void
null_device_function(void              *context GEIS_UNUSED,
                     GeisInputDeviceId  device_id GEIS_UNUSED,
                     void              *attrs GEIS_UNUSED)
{
}

static GeisInputFuncs input_funcs = {
  input_device_added,
  null_device_function,
  null_device_function
};

TEST_F(Geis1InstanceTests, reportDevices)
{
  GeisXcbWinInfo xcb_win_info = { NULL, NULL, 0 };
  GeisWinInfo win_info = { GEIS_XCB_FULL_WINDOW, &xcb_win_info };
  GeisInstance geis;
  ASSERT_EQ(GEIS_STATUS_SUCCESS,
            geis_init(&win_info, &geis));
  ASSERT_EQ(GEIS_STATUS_SUCCESS,
            geis_input_devices(geis, &input_funcs, this));
  EXPECT_GT(device_seen_count(), 0) << "no devices seen";
}

} // anonymous namespace
