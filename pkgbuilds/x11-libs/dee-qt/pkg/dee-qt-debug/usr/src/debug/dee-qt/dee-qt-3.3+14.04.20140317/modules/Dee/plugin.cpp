/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "deelistmodel.h"
#include "plugin.h"
#if WITHQT5
  #include <qqml.h>
#else
  #include <QtDeclarative/qdeclarative.h>
#endif

void DeePlugin::registerTypes(const char *uri)
{
    qmlRegisterType<DeeListModel>(uri, 3, 0, "DeeListModel");
}

#if !WITHQT5
  Q_EXPORT_PLUGIN2(Dee, DeePlugin);
#endif
