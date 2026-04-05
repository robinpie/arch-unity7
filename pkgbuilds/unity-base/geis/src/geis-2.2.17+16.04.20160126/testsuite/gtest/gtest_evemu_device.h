/**
 * @file testsuite/geis2/gtest_evemu_device.h
 * @brief OO wrapper for a evemu device
 */
/*
 * Copyright 2012 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef TESTSUITE_GEIS2_GTEST_EVEMU_DEVICE_H_
#define TESTSUITE_GEIS2_GTEST_EVEMU_DEVICE_H_

extern "C" {
#include <evemu.h>
}
#include <string>


namespace Testsuite
{

class EvemuDevice
{
public:
  /**
   * Creates a wrapped evemu pseudodevice.
   * @param[in] propFilename  Names the evemy device properties file.
   *
   * @throws std::runtime_error on any failure.
   */
  EvemuDevice(const std::string& propFileName);
  ~EvemuDevice();

  /**
   * Gets the name of the device.
   */
  std::string
  name() const;

  /**
   * Plays a named evemu events fil on the wrapped device.
   * @param[in] eventsFilename  Names the events file.
   *
   * @throws std::runtime_error on any failure.
   */
  void
  play(const std::string& eventsFileName);

private:
  evemu_device* evemu_device_;
  int           evemu_fd_;
};

} // namespace Testsuite

#endif // TESTSUITE_GEIS2_GTEST_EVEMU_DEVICE_H_
