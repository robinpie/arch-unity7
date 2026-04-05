// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2012-2022 Canonical Ltd
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
 * Authored by: Marco Trevisan <marco.trevisan@canonical.com>
 */

#include "NemoFileManager.h"
#include <NuxCore/Logger.h>
#include <UnityCore/DesktopUtilities.h>
#include <UnityCore/GLibDBusProxy.h>
#include <UnityCore/GLibWrapper.h>
#include <gdk/gdk.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>

namespace unity
{

namespace
{
DECLARE_LOGGER(logger, "unity.filemanager.nemo");

const std::string TRASH_URI = "trash:///";
const std::string FILE_SCHEMA = "file://";

const std::string NEMO_DESKTOP_ID = "nemo.desktop";
const std::string NEMO_NAME = "org.Nemo";
const std::string NEMO_FILE_OPS_PATH = "/org/Nemo";
}

struct NemoFileManager::Impl
{
  Impl(NemoFileManager* parent)
    : parent_(parent)
  {
  }

  glib::DBusProxy::Ptr NemoOperationsProxy() const
  {
    auto flags = static_cast<GDBusProxyFlags>(G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES|G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS);
    return std::make_shared<glib::DBusProxy>(NEMO_NAME, NEMO_FILE_OPS_PATH,
                                             "org.Nemo.FileOperations",
                                             G_BUS_TYPE_SESSION, flags);
  }

  NemoFileManager* parent_;
};


FileManager::Ptr NemoFileManager::Get()
{
  static FileManager::Ptr instance(new NemoFileManager());
  return instance;
}

NemoFileManager::NemoFileManager()
  : impl_(new Impl(this))
{}

NemoFileManager::~NemoFileManager()
{}

void Activate(uint64_t timestamp)
{
  glib::Cancellable cancellable;
  glib::Object<GAppInfo> app_info(G_APP_INFO (g_desktop_app_info_new(NEMO_DESKTOP_ID.c_str())));

  if (app_info)
  {
    GdkDisplay* display = gdk_display_get_default();
    glib::Object<GdkAppLaunchContext> context(gdk_display_get_app_launch_context(display));

    if (timestamp > 0)
      gdk_app_launch_context_set_timestamp(context, timestamp);

    auto const& gcontext = glib::object_cast<GAppLaunchContext>(context);
    auto proxy = std::make_shared<glib::DBusProxy>(NEMO_NAME, NEMO_FILE_OPS_PATH,
                                                  "org.freedesktop.Application");

    glib::String context_string(g_app_launch_context_get_startup_notify_id(gcontext, app_info, nullptr));

    if (context_string && g_utf8_validate(context_string, -1, nullptr))
    {
      GVariantBuilder builder;
      g_variant_builder_init(&builder, G_VARIANT_TYPE("a{sv}"));
      g_variant_builder_add(&builder, "{sv}", "desktop-startup-id", g_variant_new("s", context_string.Value()));
      GVariant *param = g_variant_new("(@a{sv})", g_variant_builder_end(&builder));

      // Passing the proxy to the lambda we ensure that it will be destroyed when needed
      proxy->CallBegin("Activate", param, [proxy] (GVariant*, glib::Error const&) {});
    }
  }
}

void NemoFileManager::Open(std::string const& uri, uint64_t timestamp)
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

void NemoFileManager::OpenTrash(uint64_t timestamp)
{
  Open(TRASH_URI, timestamp);
}

bool NemoFileManager::TrashFile(std::string const& uri)
{
  glib::Cancellable cancellable;
  glib::Object<GFile> file(g_file_new_for_uri(uri.c_str()));
  glib::Error error;

  if (g_file_trash(file, cancellable, &error))
    return true;

  LOG_ERROR(logger) << "Impossible to trash file '" << uri << "': " << error;
  return false;
}

void NemoFileManager::EmptyTrash(uint64_t timestamp, Window parent_xid)
{
  auto const& proxy = impl_->NemoOperationsProxy();

  // Passing the proxy to the lambda we ensure that it will be destroyed when needed
  Activate(timestamp);
  proxy->CallBegin("EmptyTrash", nullptr, [proxy] (GVariant*, glib::Error const&) {});
}

void NemoFileManager::CopyFiles(std::set<std::string> const& uris, std::string const& dest, uint64_t timestamp, Window parent_xid)
{
  if (uris.empty() || dest.empty())
    return;

  bool found_valid = false;
  GVariantBuilder b;
  g_variant_builder_init(&b, G_VARIANT_TYPE("(ass)"));
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
  glib::Variant parameters(g_variant_builder_end(&b));

  if (found_valid)
  {
    // Passing the proxy to the lambda we ensure that it will be destroyed when needed
    auto const& proxy = impl_->NemoOperationsProxy();
    proxy->CallBegin("CopyURIs", parameters, [proxy] (GVariant*, glib::Error const&) {});
    Activate(timestamp);
  }
}

WindowList NemoFileManager::WindowsForLocation(std::string const& location) const
{
  std::vector<ApplicationWindowPtr> windows;
  return windows;
}

std::string NemoFileManager::LocationForWindow(ApplicationWindowPtr const& win) const
{
  return std::string();
}

}  // namespace unity