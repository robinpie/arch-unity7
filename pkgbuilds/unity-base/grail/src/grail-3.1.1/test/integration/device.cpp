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

#include "device.h"

#include <fcntl.h>

#include <stdexcept>

#include <gtest/gtest.h>

oif::evemu::Device::Device(const char* path) {
  static const char UINPUT_NODE[] = "/dev/uinput";

  device_ = evemu_new(NULL);
  if (!device_)
    throw std::runtime_error("Failed to create evemu record");

  FILE* fp = fopen(path, "r");
  if (fp == NULL)
    throw std::runtime_error(std::string("Failed to open device file ") + path);

  if (evemu_read(device_, fp) <= 0) {
    fclose(fp);
    throw std::runtime_error(std::string("Failed to read device file ") + path);
  }

  fclose(fp);

  fd_ = open(UINPUT_NODE, O_WRONLY);
  if (fd_ < 0) {
    evemu_delete(device_);
    throw std::runtime_error("Failed to open uinput node");
  }

  if (evemu_create(device_, fd_) < 0) {
    close(fd_);
    evemu_delete(device_);
    throw std::runtime_error("Failed to create evemu device");
  }
}

oif::evemu::Device::~Device() {
  close(fd_);
  evemu_delete(device_);
}
