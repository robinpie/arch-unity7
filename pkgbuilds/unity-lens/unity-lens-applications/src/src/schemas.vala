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
 * Authored by Mikkel Kamstrup Erlandsen <mikkel.kamstrup@gmail.com>
 *
 */

namespace Unity.ApplicationsLens {

  public enum ResultsColumn
  {
    URI = 0,
    ICON_HINT,
    GROUP_ID,
    MIMETYPE,
    DISPLAY_NAME,
    COMMENT
  }
  
 public enum Category
  {
    APPLICATIONS,
    RECENT,
    RECENT_APPS,
    INSTALLED,
    AVAILABLE,
  }

  public enum RemoteScopesColumn
  {
    SCOPE_ID,
    NAME,
    DESCRIPTION,
    ICON_HINT,
    SCREENSHOT_URL,
    KEYWORDS
  }
  
  public enum RunnerCategory
  {
    RESULTS,
    HISTORY
  }
}
