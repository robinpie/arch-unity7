/*
 * Copyright (C) 2012 Canonical, Ltd.
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

#include <QCoreApplication>
#include <QDBusConnection>
#include <QObject>

#include <dee.h>

#include <QDebug>
class TestControl : public QObject
{
    Q_OBJECT
public:
    TestControl()
    {
        g_type_init();
        DeeModel* model = dee_shared_model_new("com.deeqt.test.model");
        dee_model_set_schema(model, "b", NULL);

        // Doc says we need to be synchronized before doing anything
        while(!dee_shared_model_is_synchronized(DEE_SHARED_MODEL(model)))
            qApp->processEvents();

        QDBusConnection::sessionBus().registerService("com.deeqt.test.control");
        QDBusConnection::sessionBus().registerObject("/control", this, QDBusConnection::ExportAllSlots);
    }

public Q_SLOTS:
    void quit()
    {
        qApp->quit();
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TestControl tc;

    return a.exec();
}

#include "test-helper.moc"
