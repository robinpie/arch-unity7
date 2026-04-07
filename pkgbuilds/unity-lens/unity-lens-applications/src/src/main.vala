/* -*- mode: vala; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 * Copyright (C) 2011-2013 Canonical Ltd
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

using GLib;
using Config;

public int unity_scope_module_get_version ()
{
  return Unity.SCOPE_API_VERSION;
}

public List<Unity.AbstractScope> unity_scope_module_load_scopes () throws Error
{
  /* Sort up locale to get translations but also sorting and
   * punctuation right */
  GLib.Intl.bindtextdomain (Config.PACKAGE, Config.LOCALEDIR);
  GLib.Intl.bind_textdomain_codeset (Config.PACKAGE, "UTF-8");
  GLib.Intl.setlocale(GLib.LocaleCategory.ALL, "");

  var scopes = new List<Unity.AbstractScope> ();
  var apps_scope = new Unity.ApplicationsLens.ApplicationsScope ();
  var scopes_scope = new Unity.ApplicationsLens.ScopesScope (apps_scope);

  scopes.append(apps_scope);
  scopes.append(apps_scope.commands_scope);
  scopes.append(scopes_scope);

  return scopes;
}
