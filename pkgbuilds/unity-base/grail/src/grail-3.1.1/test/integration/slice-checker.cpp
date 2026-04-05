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

#include "slice-checker.h"
#include <gtest/gtest.h>

/* Margin of fluctuation on number of expected repetitions
   Between 0.0 (0% error margin) to 1.0 (100% error margin)
   exclusive. */
#define EXPECTED_COUNT_MARGIN 0.3

using namespace oif::grail::testing;

void SliceCheckerState::SetAverageCount(unsigned int average_count)
{
  min_count = (unsigned int)(
      average_count*(1.0 - EXPECTED_COUNT_MARGIN));

  max_count = (unsigned int)(
      average_count*(1.0 + EXPECTED_COUNT_MARGIN));
}

SliceChecker::SliceChecker() : curr_state_(0) {
}

void SliceChecker::AppendState(std::unique_ptr<SliceCheckerState> state)
{
  slice_checker_states_.push_back(std::move(state));
}

void SliceChecker::CheckSlice(UGSlice slice) {
  ASSERT_LT(curr_state_, slice_checker_states_.size()) << "Received too many slices.";

  /* Map type() to class. */
  switch (slice_checker_states_[curr_state_]->type()) {
    case SliceCheckerState::SlicesType:
      {
        ExpectSlices *state = dynamic_cast<ExpectSlices*>(
            slice_checker_states_[curr_state_].get());
        ASSERT_NE(nullptr, state);
        CheckSlice(slice, state);
      }
      break;
    case SliceCheckerState::ParallelSlicesType:
      {
        ExpectParallelSlices *state = dynamic_cast<ExpectParallelSlices *>(
            slice_checker_states_[curr_state_].get());
        ASSERT_NE(nullptr, state);
        CheckSlice(slice, state);
      }
      break;
    default:
      ASSERT_TRUE(false);
      break;
  };
}

void SliceChecker::CheckSlice(UGSlice slice, ExpectSlices *state) {
  if (SliceMatches(slice, state->expected_slice)) {
    ++state->actual_count;
  } else {
    /* If there's no match, transition to the next state and try to get a
       match there. */
    CheckAllExpectedSlicesReceived(state);
    ++curr_state_;
    CheckSlice(slice);
  }
}

void SliceChecker::CheckSlice(UGSlice slice, ExpectParallelSlices *state) {
  if (state->actual_count.size() == 0) {
    /* init the vector */
    state->actual_count.resize(state->expected_slices.size(), 0);
  }

  bool found_match = false;
  std::size_t i = 0;
  do {
    found_match = SliceMatches(slice, state->expected_slices[i]);
    if (!found_match)
      ++i;
  } while (!found_match && i < state->expected_slices.size());

  if (found_match) {
    ++state->actual_count[i];
  } else {
    CheckAllExpectedSlicesReceived(state);
    ++curr_state_;
    CheckSlice(slice);
  }
}

void SliceChecker::CheckAllExpectedSlicesReceived()
{
  ASSERT_EQ(slice_checker_states_.size() - 1, curr_state_)
    << "the last slice checker state must be reached.";

  /* Map type() to class. */
  switch (slice_checker_states_[curr_state_]->type()) {
    case SliceCheckerState::SlicesType:
      {
        ExpectSlices *state = dynamic_cast<ExpectSlices *>(
            slice_checker_states_[curr_state_].get());
        ASSERT_NE(nullptr, state);
        CheckAllExpectedSlicesReceived(state);
      }
      break;
    case SliceCheckerState::ParallelSlicesType:
      {
        ExpectParallelSlices *state = dynamic_cast<ExpectParallelSlices *>(
            slice_checker_states_[curr_state_].get());
        ASSERT_NE(nullptr, state);
        CheckAllExpectedSlicesReceived(state);
      }
      break;
    default:
      ASSERT_TRUE(false);
      break;
  };
}

void SliceChecker::CheckAllExpectedSlicesReceived(
    ExpectSlices *state)
{
  ASSERT_GE(state->actual_count, state->min_count) << "for state " << curr_state_;
  ASSERT_LE(state->actual_count, state->max_count) << "for state " << curr_state_;
}

void SliceChecker::CheckAllExpectedSlicesReceived(
    ExpectParallelSlices *state)
{
  unsigned int min_actual_count = std::numeric_limits<unsigned int>::max();
  unsigned int max_actual_count = 0;

  for (auto actual_count : state->actual_count) {
    ASSERT_GE(actual_count, state->min_count) << "for state " << curr_state_;
    ASSERT_LE(actual_count, state->max_count) << "for state " << curr_state_;

    if (actual_count < min_actual_count)
      min_actual_count = actual_count;

    if (actual_count > max_actual_count)
      max_actual_count = actual_count;
  }

  unsigned int average_count = (state->max_count + state->min_count) / 2;
  ASSERT_LE(max_actual_count - min_actual_count, average_count*0.1)
  << "Parallel slices must come in roughly same numbers.";
}

bool SliceChecker::SliceMatches(UGSlice slice,
    const ExpectedSlice &expected_slice)
{
  if (grail_slice_get_state(slice) != expected_slice.state)
    return false;

  if (grail_slice_get_subscription(slice) != expected_slice.subscription)
    return false;

  if (grail_slice_get_recognized(slice) != expected_slice.recognized)
    return false;

  if (grail_slice_get_num_touches(slice) != expected_slice.num_touches)
    return false;

  if (grail_slice_get_construction_finished(slice) !=
      expected_slice.construction_finished)
    return false;

  return true;
}
