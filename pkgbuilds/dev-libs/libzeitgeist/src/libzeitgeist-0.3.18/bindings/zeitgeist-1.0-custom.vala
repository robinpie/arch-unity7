/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authored by Michal Hruby <michal.mhr@gmail.com>
 */

namespace Zeitgeist
{
  public interface ResultSet: GLib.Object
  {
    [CCode (cname = "_vala_zeitgeist_result_set_next_value")]
    public unowned Event? next_value ();
    [CCode (cname = "_vala_zeitgeist_result_set_iterator")]
    public ResultSet iterator ();
  }
}

namespace Zeitgeist.Timestamp
{
  [CCode (cname = "ZEITGEIST_TIMESTAMP_SECOND", cheader_filename = "zeitgeist.h")]
  public int64 SECOND;

  [CCode (cname = "ZEITGEIST_TIMESTAMP_MINUTE", cheader_filename = "zeitgeist.h")]
  public int64 MINUTE;

  [CCode (cname = "ZEITGEIST_TIMESTAMP_HOUR", cheader_filename = "zeitgeist.h")]
  public int64 HOUR;

  [CCode (cname = "ZEITGEIST_TIMESTAMP_DAY", cheader_filename = "zeitgeist.h")]
  public int64 DAY;

  [CCode (cname = "ZEITGEIST_TIMESTAMP_WEEK", cheader_filename = "zeitgeist.h")]
  public int64 WEEK;

  [CCode (cname = "ZEITGEIST_TIMESTAMP_YEAR", cheader_filename = "zeitgeist.h")]
  public int64 YEAR;
}

// vim:et:ai:cindent:ts=2 sts=2 sw=2:
