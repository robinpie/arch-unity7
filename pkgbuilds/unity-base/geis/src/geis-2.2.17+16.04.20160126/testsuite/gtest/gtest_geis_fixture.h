/**
 * @file gtest_geis_fixture.h
 * @brief A GTest fixture for testing the full stack through GEIS.
 */
/*
 *  Copyright 2012 Canonical Ltd.
 *
 *  This program is free software: you can redistribute it and/or modify it 
 *  under the terms of the GNU General Public License version 3, as published 
 *  by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranties of 
 *  MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along 
 *  with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef GTEST_GEIS_FIXTURE_H_
#define GTEST_GEIS_FIXTURE_H_

#include <functional>
#include "geis/geis.h"
#include <gtest/gtest.h>
#include <map>
#include <xorg/gtest/xorg-gtest.h>


/**
 * Fixture for testing expected attributes.
 */
class GTestGeisFixture
: public xorg::testing::Test
{
public:
  /**
   * Test cases should provide an event handler function with this type
   * signature to perform the main test processing.
   */
  typedef std::function<void(Geis, GeisEvent)> GeisEventHandler;

  /**
   * A test case can extend the event-driven model with a bound callback.
   */
  typedef std::function<void()> EventCallback;

  GTestGeisFixture();

  void
  SetUp();

  void
  TearDown();

  /**
   * Customization point for test cases that need to synchronize their own
   * entities.
   *
   * @param[in] fd        A file descriptor to synchronize.
   * @param[in] callback  A function object to invoke when read activity is
   *                      detected on @p fd.
   */
  void
  add_event_callback(int fd, EventCallback callback);

  /**
   * Indicates if the GEIS instance has been successfully initialized.
   *
   * Test cases can use this function in their event handler(s) to avoid
   * performing actions until the stack has been fully initialized.
   */
  bool
  geis_is_initialized() const;

  /**
   * Sets the main loop timeout.
   *
   * @param[in] seconds  The timeout (in seconds) to wait until exiting the main
   *                     event loop.
   *
   * The default timeout is 1 second.
   */
  void
  geis_dispatch_loop_timeout(long seconds);

  /**
   * Customization point for common event handling for all tests run with a
   * fixture.
   *
   * @param[in] event  The current geis event to be handled.
   *
   * The default pre-event handler does nothing.
   */
  virtual void
  pre_event_handler(GeisEvent event);

  /**
   * Sets the geis dispatch loop event handler.
   *
   * @param[in] handler  An event handler function.
   */
  void
  set_geis_event_handler(GeisEventHandler handler);

  /**
   * Customization point for common event handling for all tests run with a
   * fixture.
   *
   * @param[in] event  The current geis event to be handled.
   *
   * The default post-event handler calls geis_dispatch_stop(true) on a
   * GEIS_EVENT_GESTURE_END event.  Derived test fixtures that override
   * this function should invoke the base class function at the end of their
   * processing (prefered) or else call geis_dispatch_stop(true) themselves
   * if appropriate.
   */
  virtual void
  post_event_handler(GeisEvent event);

  /**
   * Runs the geis dispatch loop.
   *
   * The geis dipatch loop waits for geis events (and only geis events) and
   * dispatches them as the are repotred, until geis_dispatch_stop(true) has
   * been called.
   */
  void
  geis_dispatch_loop();

  /**
   * Asks the geis dispatch loop to stop and return.
   *
   * @param[in] do_stop Tells the loop to stop (true) or continue (false);
   */
  void
  geis_dispatch_stop(bool do_stop);

private:
  /**
   * Sets up the GEIS instance.
   *
   * This function provides a customization point for tests that need to create
   * the captive geis instance with different configuration options.
   */
  virtual void
  setup_geis();

  /**
   * Gets the event-watch file descriptor of the captive geis instance.
   */
  int
  geis_fd();

  /**
   * The event handler for the captive geis instance.
   */
  void
  geis_event_callback();

protected:
  typedef std::map<int, EventCallback> EventCallbacks;

  Geis             geis_;
  bool             geis_is_initialized_;
  long             geis_dispatch_loop_timeout_;
  GeisEventHandler geis_event_handler_;
  bool             geis_event_loop_stop_;
  EventCallbacks   event_callbacks_;
};


#endif /* GTEST_GEIS_FIXTURE_H_ */

