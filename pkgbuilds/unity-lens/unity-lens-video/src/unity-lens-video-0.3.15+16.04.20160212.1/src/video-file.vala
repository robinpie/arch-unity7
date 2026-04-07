/*
 * Copyright (C) 2012 Canonical Ltd
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
 * Authored by Pawel Stolowski <pawel.stolowski@canonical.com>
 *
 */

namespace Unity.VideoLens
{
  public struct VideoFile
  {
    string title;
    string lc_title;
    string comment;
    string uri;
    string icon;
    int category;
  }

  public struct RemoteVideoFile
  {
    string title;
    string comment;
    string uri;
    string icon;
    string details_uri;
    string price;
    int category;
  }

public struct RemoteVideoDetails
  {
    string title;
    string description;
    string uri;
    string image;
    string source;
    string release_date;
    int duration;
    string[] directors;
    string starring;
    string[] genres;
    string uploaded_by;
    string date_uploaded;
    string price;
  }
}