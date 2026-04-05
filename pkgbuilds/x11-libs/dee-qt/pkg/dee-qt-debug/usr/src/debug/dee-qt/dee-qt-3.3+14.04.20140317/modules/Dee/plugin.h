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

#ifndef QTDEEPLUGIN_H
#define QTDEEPLUGIN_H

#if WITHQT5
  #include <QQmlExtensionPlugin>
#else
  #include <QtDeclarative/QDeclarativeExtensionPlugin>
#endif

class DeePlugin :
#if WITHQT5
  public QQmlExtensionPlugin
#else
  public QDeclarativeExtensionPlugin
#endif
{
    Q_OBJECT
#if WITHQT5
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
#endif
public:
    void registerTypes(const char *uri);
};


#endif // QTDEEPLUGIN_H
