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

#include <QtTest>
#include <QObject>
#include <QDBusInterface>

#include "deelistmodel.h"

#include <dee.h>

static bool wait_until_test_service_appears()
{
    bool have_name = false;
    bool timeout_reached = false;

    auto timeout_cb = [](gpointer data) -> gboolean
    {
        *(bool*)data = true;
        return FALSE;
    };

    auto callback = [](GDBusConnection * conn,
                    const char * name,
                    const char * name_owner,
                    gpointer user_data)
    {
        *(bool*)user_data = true;
    };

    g_bus_watch_name(G_BUS_TYPE_SESSION,
                    "com.deeqt.test.control",
                    G_BUS_NAME_WATCHER_FLAGS_NONE,
                    callback,
                    NULL,
                    &have_name,
                    NULL);
    g_timeout_add(10000, timeout_cb, &timeout_reached);

    while (!have_name && !timeout_reached)
        g_main_context_iteration(g_main_context_get_thread_default(), TRUE);

    return (have_name && !timeout_reached);
}

static void tell_service_to_exit()
{
    QDBusInterface iface("com.deeqt.test.control", "/control");
    iface.call("quit");
}

class DeeListModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        g_type_init();

        QVERIFY(wait_until_test_service_appears());
    }

    void synchronizedTest()
    {
        DeeModel* model = dee_shared_model_new("com.deeqt.test.does_not_exist");

        DeeListModel model_qt;
        QCOMPARE(model_qt.count(), 0);

        model_qt.setModel(model);
        QCOMPARE(model_qt.synchronized(), (bool)dee_shared_model_is_synchronized(DEE_SHARED_MODEL(model)));
    }

    void setExistingModelTest()
    {
        DeeListModel model_qt;
        QCOMPARE(model_qt.count(), 0);

        model_qt.setName("com.deeqt.test.model");
        QCOMPARE(model_qt.synchronized(), false);

        while(!model_qt.synchronized())
            qApp->processEvents();

        QCOMPARE(model_qt.synchronized(), true);
        QCOMPARE(model_qt.roleNames().count(), 1);
        QCOMPARE(model_qt.roleNames()[0], QByteArray("column_0"));

        model_qt.setName("");
        QCOMPARE(model_qt.synchronized(), false);
        QCOMPARE(model_qt.rowCount(), 0);
    }
    
    void cleanupTestCase()
    {
        tell_service_to_exit();
    }

};

QTEST_MAIN(DeeListModelTest)

#include "deelistmodeltest.moc"
