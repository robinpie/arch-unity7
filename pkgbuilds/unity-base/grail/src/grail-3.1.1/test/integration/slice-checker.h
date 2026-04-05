/*****************************************************************************
 *
 * grail - Gesture Recognition And Instantiation Library
 *
 * Copyright (C) 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of version 3 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#ifndef _GRAIL_TEST_SLICE_CHECKER_H_
#define _GRAIL_TEST_SLICE_CHECKER_H_

#include <oif/grail.h>
#include <memory>
#include <vector>

namespace oif {
namespace grail {
namespace testing {

/*
 Holds the expected values for some of the properties
 of a UGSlice.
 */
struct ExpectedSlice {
  UGGestureState state;
  UGGestureTypeMask recognized;
  unsigned int num_touches;
  bool construction_finished;
  UGSubscription subscription;
};

/*
  Abstract base class for states of the slice checker.
 */
class SliceCheckerState {
 public:
  virtual ~SliceCheckerState() {};
  /* Average number of slices that should arrive
     with the values described in the concrete state class.
     This is a convenience function that will set min_count
     and max_count appropriately. */
  void SetAverageCount(unsigned int average_count);

  /* Minimum and maximum number of slices that should arrive
     with the values described in the concrete state class */
  unsigned int min_count;
  unsigned int max_count;

 protected:
  enum StateType {
    SlicesType, /* ExpectSlices class */
    ParallelSlicesType, /* ParalellRepeatedSlices class */
  };

  /*
    Returns the type of the state.
    With that information you're able to cast a SliceCheckerState pointer
    to a pointer of the corresponding concrete state class.
   */
  virtual StateType type() = 0;

  friend class SliceChecker;
};

/*
  Tells the slice checker to expect a stream of slices with
  the same characteristics.
 */
class ExpectSlices : public SliceCheckerState {
 public:
  ExpectSlices() : actual_count(0) {}
  virtual ~ExpectSlices() {};

  /* Property values of the expected slices. */
  ExpectedSlice expected_slice;

 protected:
  virtual StateType type() {return SlicesType;}

 private:
  /* Filled by the slice checker.
     Number of times a slice was successfully matched against
     expected_slice. */
  unsigned int actual_count;

  friend class SliceChecker;
};

/*
  Tells the slice checker to check for a single slice.

  Essentially a convenience class wrapping ExpectedSlices
 */
class ExpectSlice : public ExpectSlices {
 public:
  ExpectSlice() {min_count = 1; max_count = 1;}
};

/*
  Tells the slice checker to check for multiple streams of repeated slices
  that come interleaved.

 */
class ExpectParallelSlices : public SliceCheckerState {
 public:
  virtual ~ExpectParallelSlices() {};
  virtual StateType type() {return ParallelSlicesType;}


  /* Property values of the expected slices from each
     stream.

     min_count and max_count are meant for each expected slice. */
  std::vector<ExpectedSlice> expected_slices;

 private:
  /* Filled by the slice checker.
     Number of times a slice was successfully matched against
     expected_slices[index]. */
  std::vector<unsigned int> actual_count;

  friend class SliceChecker;
};

/*
  This class checks if a stream of slices given to it via CheckSlice() have
  the characteristics described in the slice checker states provided.

  Usage:

  1- Describe the stream of slices you expect.
      * Do that by providing a sequence of slice checker states via
        AppendState()
  2- Provide the stream of slices that will be checked against
     those expectations.
      * Do that by calling CheckSlice(), in order.
  3- Verify that all expected slices have been received.
      * Call CheckAllExpectedSlicesReceived()
 */
class SliceChecker {
 public:

  SliceChecker();
  /*
    Appends a state.
   */
  void AppendState(std::unique_ptr<SliceCheckerState> state);

  /*
    Checks the given slice.
   */
  void CheckSlice(UGSlice slice);

  /*
    Checks if all the expected slices have been received.

    Will cause a gtest assertion failure if not.
   */
  void CheckAllExpectedSlicesReceived();

 private:

  void CheckSlice(UGSlice slice, ExpectSlices *state);
  void CheckSlice(UGSlice slice, ExpectParallelSlices *state);
  void CheckAllExpectedSlicesReceived(ExpectSlices *state);
  void CheckAllExpectedSlicesReceived(ExpectParallelSlices *state);

  /*
    Returns true if the given slice matches all values described by
    expected_slice and false otherwise.
   */
  static bool SliceMatches(UGSlice slice, const ExpectedSlice &expected_slice);

  /*
   When CheckSlice(UGSlice slice) is called, how
   that slice will be checked depends on the current state of
   the slice checker. That current state is defined by
   slice_checker_states_[curr_state_]

   slice_checker_states_ holds a vector of states and curr_state_
   is the index of the current state.

   This is a linear state machine, where from "state 0" you can only transition
   to "state 1", from "state 1" you only to "state 2"  and so on until the last
   state is reached.
   */
  unsigned int curr_state_;
  std::vector<std::unique_ptr<SliceCheckerState>> slice_checker_states_;
};

} // namespace testing
} // namespace grail
} // namespace oif

#endif // _GRAIL_TEST_SLICE_CHECKER_H_
