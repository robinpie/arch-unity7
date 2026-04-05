/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2010-2011 Canonical Ltd.
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

#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/extensions/sync.h>

#include <oif/grail.h>

XSyncAlarm create_alarm(Display *display);
void destroy_alarm(Display *display, XSyncAlarm alarm);
void update_time(UGHandle handle, XSyncAlarmNotifyEvent *event);
void set_timeout(UGHandle handle, Display *display, XSyncAlarm alarm);
