/*
 * Copyright (C) 2010 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 *
 */

using Zeitgeist;
using GLib;

public class Logger
{
  public static int main (string[] args)
    {
      Zeitgeist.Log zg = new Zeitgeist.Log();
      Event ev = new Event.full (Zeitgeist.ZG_ACCESS_EVENT,
                                 Zeitgeist.ZG_USER_ACTIVITY,
                                 "app://gedit.desktop",
                                 new Subject.full("file:///tmp/foo.txt",
                                                  Zeitgeist.NFO_TEXT_DOCUMENT,
                                                  Zeitgeist.NFO_FILE_DATA_OBJECT,
                                                  "text/plain",
                                                  "file:///tmp",
                                                  "foo.txt",
                                                  "UUID=a9a17ad2-af3a-49af-ae50-5053053535cf"));
      zg.insert_events_no_reply(ev);

      MainLoop mainloop = new MainLoop(null, false);
      mainloop.run();

      return 0;
    }
}