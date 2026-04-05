/**
 * @file testsuite/geis2/gtest_evemu_device.cpp
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
#include "gtest_evemu_device.h"

#include <cstdio>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>


namespace
{

static const char UINPUT_NODE[] = "/dev/uinput";

} // anonymous namespace


Testsuite::EvemuDevice::
EvemuDevice(const std::string& propFileName)
{
  evemu_device_ = evemu_new(NULL);
  if (!evemu_device_)
    throw std::runtime_error("Failed to create evemu device");

  FILE* prop_file = std::fopen(propFileName.c_str(), "r");
  if (!prop_file)
    throw std::runtime_error("Can not open evemu prop file " + propFileName);

  if (evemu_read(evemu_device_, prop_file) <= 0)
  {
    std::fclose(prop_file);
    throw std::runtime_error("can not read evemu device file");
  }

  std::fclose(prop_file);

  evemu_fd_ = open(UINPUT_NODE, O_WRONLY);
  if (evemu_fd_ < 0) {
    evemu_delete(evemu_device_);
    throw std::runtime_error("Failed to open uinput node");
  }

  if (evemu_create(evemu_device_, evemu_fd_) < 0) {
    close(evemu_fd_);
    evemu_delete(evemu_device_);
    throw std::runtime_error("Failed to create evemu device");
  }
}


Testsuite::EvemuDevice::
~EvemuDevice()
{
  close(evemu_fd_);
}
  

std::string Testsuite::EvemuDevice::
name() const
{
  const char* device_name = evemu_get_name(evemu_device_);
  if (device_name)
    return device_name;
  return "";
}


void Testsuite::EvemuDevice::
play(const std::string& eventsFileName)
{
  FILE* events_file = std::fopen(eventsFileName.c_str(), "r");
  if (!events_file)
    throw std::runtime_error("can not open evemu events file " + eventsFileName);

  if (evemu_play(events_file, evemu_fd_) != 0)
  {
    std::fclose(events_file);
    throw std::runtime_error("can not play evemu recording");
  }

  std::fclose(events_file);
}

