// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2012-2013 Canonical Ltd
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
 * Authored by: Andrea Azzarone <andrea.azzarone@canonical.com>
 *              Marco Trevisan <marco.trevisan@canonical.com>
 */

#include "GnomeFileManager.h"
#include <NuxCore/Logger.h>
#include <UnityCore/DesktopUtilities.h>
#include <UnityCore/GLibSource.h>
#include <UnityCore/GLibDBusProxy.h>
#include <UnityCore/GLibWrapper.h>
#include <gdk/gdk.h>
#include <gio/gio.h>

namespace unity
{

namespace
{
DECLARE_LOGGER(logger, "unity.filemanager.gnome");

const std::string TRASH_URI = "trash:///";
const std::string FILE_SCHEMA = "file://";

const std::string NAUTILUS_NAME = "org.gnome.Nautilus";
const std::string NAUTILUS_FILE_OPS_PATH = "/org/gnome/Nautilus/FileOperations2";

const std::string GTK_WINDOW_PATH_PROPERTY = "_GTK_WINDOW_OBJECT_PATH";
}

struct GnomeFileManager::Impl
{
  Impl(GnomeFileManager* parent)
    : parent_(parent)
    , filemanager_proxy_("org.freedesktop.FileManager1", "/org/freedesktop/FileManager1", "org.freedesktop.FileManager1", G_BUS_TYPE_SESSION, G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS)
  {
    auto callback = sigc::mem_fun(this, &Impl::OnOpenWindowsWithLocationsChanged);
    filemanager_proxy_.GetProperty("OpenWindowsWithLocations", callback);
    filemanager_proxy_.ConnectProperty("OpenWindowsWithLocations", callback);
  }

  glib::DBusProxy::Ptr NautilusOperationsProxy() const
  {
    auto flags = static_cast<GDBusProxyFlags>(G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES|G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS);
    return std::make_shared<glib::DBusProxy>(NAUTILUS_NAME, NAUTILUS_FILE_OPS_PATH,
                                             "org.gnome.Nautilus.FileOperations2",
                                             G_BUS_TYPE_SESSION, flags);
  }

  void OnOpenWindowsWithLocationsChanged(GVariant* value)
  {
    opened_location_for_window_.clear();

    if (!value)
    {
      LOG_WARN(logger) << "Locations have been invalidated, maybe there's no filemanager around...";
      parent_->locations_changed.emit();
      return;
    }

    if (!g_variant_is_of_type(value, G_VARIANT_TYPE("a{sas}")))
    {
      LOG_ERROR(logger) << "Locations value type is not matching the expected one!";
      parent_->locations_changed.emit();
      return;
    }

    GVariantIter iter;
    GVariantIter *str_iter;
    const char *loc;
    const char *window_path;
    std::map<std::string, std::string> opened_locations_for_path;

    g_variant_iter_init(&iter, value);

    while (g_variant_iter_loop(&iter, "{&sas}", &window_path, &str_iter))
    {
      while (g_variant_iter_loop(str_iter, "s", &loc))
      {
        /* We only care about the first mentioned location as per our "standard"
         * it's the active one */
        LOG_DEBUG(logger) << window_path << ": Opened location " << loc;
        opened_locations_for_path[window_path] = loc;
        break;
      }
    }

    // We must ensure that we emit the locations_changed signal only when all
    // the parent windows have been registered on the app-manager
    auto app_manager_not_synced = [this, opened_locations_for_path]
    {
      auto& app_manager = ApplicationManager::Default();
      bool synced = true;

      for (auto const& pair : opened_locations_for_path)
      {
        auto win = app_manager.GetWindowForProperty(GTK_WINDOW_PATH_PROPERTY, pair.first);
        synced = (win != nullptr);

        if (!synced)
          break;

        LOG_DEBUG(logger) << win->window_id() << ": Opened location " << pair.second;
        opened_location_for_window_[win] = pair.second;
      }

      if (synced)
        parent_->locations_changed.emit();
      else
        opened_location_for_window_.clear();

      return !synced;
    };

    if (app_manager_not_synced())
      idle_.reset(new glib::Idle(app_manager_not_synced));
  }

  GVariant *GetPlatformData(uint64_t timestamp, Window parent_xid)
  {
    GVariantBuilder builder;
    char *parent_handle;

    g_variant_builder_init(&builder, G_VARIANT_TYPE ("a{sv}"));

    parent_handle = g_strdup_printf("x11:%lx", parent_xid);

    g_variant_builder_add(&builder, "{sv}", "parent-handle",
                          g_variant_new_take_string(parent_handle));

    g_variant_builder_add(&builder, "{sv}", "timestamp",
                          g_variant_new_uint32(timestamp));

    g_variant_builder_add(&builder, "{sv}", "window-position",
                          g_variant_new_string("center"));

    return g_variant_builder_end(&builder);
  }

  GnomeFileManager* parent_;
  glib::DBusProxy filemanager_proxy_;
  glib::Source::UniquePtr idle_;
  std::map<ApplicationWindowPtr, std::string> opened_location_for_window_;
};


FileManager::Ptr GnomeFileManager::Get()
{
  static FileManager::Ptr instance(new GnomeFileManager());
  return instance;
}

GnomeFileManager::GnomeFileManager()
  : impl_(new Impl(this))
{}

GnomeFileManager::~GnomeFileManager()
{}

void GnomeFileManager::Open(std::string const& uri, uint64_t timestamp)
{
  if (uri.empty())
  {
    LOG_ERROR(logger) << "Impossible to open an empty location";
    return;
  }

  glib::Error error;
  GdkDisplay* display = gdk_display_get_default();
  glib::Object<GdkAppLaunchContext> context(gdk_display_get_app_launch_context(display));

  if (timestamp > 0)
    gdk_app_launch_context_set_timestamp(context, timestamp);

  auto const& gcontext = glib::object_cast<GAppLaunchContext>(context);
  g_app_info_launch_default_for_uri(uri.c_str(), gcontext, &error);

  if (error)
  {
    LOG_ERROR(logger) << "Impossible to open the location: " << error.Message();
  }
}

void GnomeFileManager::OpenTrash(uint64_t timestamp)
{
  Open(TRASH_URI, timestamp);
}

bool GnomeFileManager::TrashFile(std::string const& uri)
{
  glib::Cancellable cancellable;
  glib::Object<GFile> file(g_file_new_for_uri(uri.c_str()));
  glib::Error error;

  if (g_file_trash(file, cancellable, &error))
    return true;

  LOG_ERROR(logger) << "Impossible to trash file '" << uri << "': " << error;
  return false;
}

void GnomeFileManager::EmptyTrash(uint64_t timestamp, Window parent_xid)
{
  auto const& proxy = impl_->NautilusOperationsProxy();
  const bool ask_confirmation = true;

  GVariantBuilder b;
  g_variant_builder_init(&b, G_VARIANT_TYPE("(ba{sv})"));
  g_variant_builder_add(&b, "b", ask_confirmation);
  g_variant_builder_add_value(&b, impl_->GetPlatformData(timestamp, parent_xid));
  glib::Variant parameters(g_variant_builder_end(&b));

  // Passing the proxy to the lambda we ensure that it will be destroyed when needed
  proxy->CallBegin("EmptyTrash", parameters, [proxy] (GVariant*, glib::Error const&) {});
}

void GnomeFileManager::CopyFiles(std::set<std::string> const& uris, std::string const& dest, uint64_t timestamp, Window parent_xid)
{
  if (uris.empty() || dest.empty())
    return;

  bool found_valid = false;
  GVariantBuilder b;
  g_variant_builder_init(&b, G_VARIANT_TYPE("(assa{sv})"));
  g_variant_builder_open(&b, G_VARIANT_TYPE("as"));

  for (auto const& uri : uris)
  {
    if (uri.find(FILE_SCHEMA) == 0)
    {
      found_valid = true;
      g_variant_builder_add(&b, "s", uri.c_str());
    }
  }

  g_variant_builder_close(&b);
  g_variant_builder_add(&b, "s", dest.c_str());
  g_variant_builder_add_value(&b, impl_->GetPlatformData(timestamp, parent_xid));
  glib::Variant parameters(g_variant_builder_end(&b));

  if (found_valid)
  {
    // Passing the proxy to the lambda we ensure that it will be destroyed when needed
    auto const& proxy = impl_->NautilusOperationsProxy();
    proxy->CallBegin("CopyURIs", parameters, [proxy] (GVariant*, glib::Error const&) {});
  }
}

WindowList GnomeFileManager::WindowsForLocation(std::string const& location) const
{
  std::vector<ApplicationWindowPtr> windows;
  glib::Object<GFile> location_file(g_file_new_for_uri(location.c_str()));

  for (auto const& pair : impl_->opened_location_for_window_)
  {
    auto const& loc = pair.second;
    bool matches = (loc == location);

    if (!matches)
    {
      glib::Object<GFile> loc_file(g_file_new_for_uri(loc.c_str()));
      glib::String relative(g_file_get_relative_path(location_file, loc_file));
      matches = static_cast<bool>(relative);
    }

    if (matches)
    {
      auto const& win = pair.first;

      if (win && std::find(windows.rbegin(), windows.rend(), win) == windows.rend())
        windows.push_back(win);
    }
  }

  return windows;
}

std::string GnomeFileManager::LocationForWindow(ApplicationWindowPtr const& win) const
{
  if (win)
  {
    auto it = impl_->opened_location_for_window_.find(win);

    if (it != end(impl_->opened_location_for_window_))
      return it->second;
  }

  return std::string();
}

}  // namespace unity
