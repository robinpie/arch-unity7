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

#include "deelistmodel.h"

#include <dee.h>

class DeeListModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
    }

    void synchronizationTest()
    {
        DeeModel* model = dee_sequence_model_new();
        dee_model_set_schema(model, "s", "i", NULL);

        DeeListModel model_qt;
        QCOMPARE(model_qt.rowCount(), 0);
        QCOMPARE(model_qt.count(), 0);

        model_qt.setModel(model);
        QCOMPARE(model_qt.synchronized(), true);
        QVERIFY(model_qt.name().isNull());

        model_qt.setModel(NULL);
        QCOMPARE(model_qt.synchronized(), false);
        QCOMPARE(model_qt.rowCount(), 0);
        QCOMPARE(model_qt.count(), 0);
    }

    void emitBasicSignalsTest()
    {
        DeeModel* model = dee_sequence_model_new();
        dee_model_set_schema(model, "s", "i", NULL);

        DeeListModel model_qt;
        model_qt.setModel(model);

        int num_insertions = 0;

        connect(&model_qt, &QAbstractItemModel::rowsInserted, [&num_insertions] (const QModelIndex &parent, int start, int end) {
            num_insertions++;
        });

        dee_model_append(model, "foo", 5);

        QCOMPARE(num_insertions, 1);
        QCOMPARE(model_qt.count(), 1);
        QCOMPARE(model_qt.rowCount(), 1);
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("foo"));
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 5);

        int num_removals = 0;
        connect(&model_qt, &QAbstractItemModel::rowsRemoved, [&num_removals] (const QModelIndex &parent, int start, int end) {
            num_removals++;
        });

        dee_model_remove(model, dee_model_get_first_iter(model));

        QCOMPARE(num_removals, 1);
        QCOMPARE(model_qt.count(), 0);
        QCOMPARE(model_qt.rowCount(), 0);
        QCOMPARE(num_insertions, 1);
    }

    void emitMultipleSignalsTest()
    {
        DeeModel* model = dee_sequence_model_new();
        dee_model_set_schema(model, "s", "i", NULL);

        DeeListModel model_qt;
        model_qt.setModel(model);

        int num_insertions = 0;

        connect(&model_qt, &QAbstractItemModel::rowsInserted, [&num_insertions] (const QModelIndex &parent, int start, int end) {
            num_insertions++;
        });

        dee_model_append(model, "foo", 5);
        QCOMPARE(num_insertions, 1);
        QCOMPARE(model_qt.count(), 1);
        QCOMPARE(model_qt.rowCount(), 1);
        dee_model_append(model, "bar", 6);
        QCOMPARE(num_insertions, 2);
        QCOMPARE(model_qt.count(), 2);
        QCOMPARE(model_qt.rowCount(), 2);
        dee_model_append(model, "baz", 7);
        QCOMPARE(num_insertions, 3);
        QCOMPARE(model_qt.count(), 3);
        QCOMPARE(model_qt.rowCount(), 3);
        dee_model_append(model, "qoo", 8);

        QCOMPARE(num_insertions, 4);
        QCOMPARE(model_qt.count(), 4);
        QCOMPARE(model_qt.rowCount(), 4);

        QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("foo"));
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 5);
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("bar"));
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 6);
        QCOMPARE(model_qt.data(model_qt.index(2, 0), 0).toString(), QString("baz"));
        QCOMPARE(model_qt.data(model_qt.index(2, 0), 1).toInt(), 7);
        QCOMPARE(model_qt.data(model_qt.index(3, 0), 0).toString(), QString("qoo"));
        QCOMPARE(model_qt.data(model_qt.index(3, 0), 1).toInt(), 8);

        int num_removals = 0;
        connect(&model_qt, &QAbstractItemModel::rowsRemoved, [&num_removals] (const QModelIndex &parent, int start, int end) {
            num_removals++;
        });

        num_insertions = 0;

        dee_model_remove(model, dee_model_get_first_iter(model));
        QCOMPARE(num_removals, 1);
        QCOMPARE(model_qt.count(), 3);
        QCOMPARE(model_qt.rowCount(), 3);
        dee_model_remove(model, dee_model_get_first_iter(model));
        QCOMPARE(num_removals, 2);
        QCOMPARE(model_qt.count(), 2);
        QCOMPARE(model_qt.rowCount(), 2);
        dee_model_remove(model, dee_model_get_first_iter(model));
        QCOMPARE(num_removals, 3);
        QCOMPARE(model_qt.count(), 1);
        QCOMPARE(model_qt.rowCount(), 1);
        dee_model_remove(model, dee_model_get_first_iter(model));
        QCOMPARE(num_removals, 4);
        QCOMPARE(model_qt.count(), 0);
        QCOMPARE(model_qt.rowCount(), 0);
        dee_model_clear(model);
        QCOMPARE(num_removals, 4);
        QCOMPARE(model_qt.count(), 0);
        QCOMPARE(model_qt.rowCount(), 0);

        QCOMPARE(num_insertions, 0);
    }

    void emitConsecutiveInsertsTest()
    {
        DeeModel* model = dee_sequence_model_new();
        dee_model_set_schema(model, "s", "i", NULL);

        DeeListModel model_qt;
        model_qt.setModel(model);

        int num_insertions = 0;

        connect(&model_qt, &QAbstractItemModel::rowsInserted, [&num_insertions] (const QModelIndex &parent, int start, int end) {
            num_insertions++;
        });

        dee_model_begin_changeset(model);

        dee_model_append(model, "foo", 5);
        dee_model_append(model, "bar", 6);
        dee_model_append(model, "baz", 7);
        dee_model_append(model, "qoo", 8);

        QCOMPARE(num_insertions, 0);

        dee_model_end_changeset(model);

        QCOMPARE(num_insertions, 1);
        QCOMPARE(model_qt.count(), 4);
        QCOMPARE(model_qt.rowCount(), 4);

        QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("foo"));
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 5);
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("bar"));
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 6);
        QCOMPARE(model_qt.data(model_qt.index(2, 0), 0).toString(), QString("baz"));
        QCOMPARE(model_qt.data(model_qt.index(2, 0), 1).toInt(), 7);
        QCOMPARE(model_qt.data(model_qt.index(3, 0), 0).toString(), QString("qoo"));
        QCOMPARE(model_qt.data(model_qt.index(3, 0), 1).toInt(), 8);

        QVariantMap row2 = model_qt.get(2);
        QCOMPARE(row2["column_0"].toString(), QString("baz"));
        QCOMPARE(row2["column_1"].toInt(), 7);
    }

    void emitConsecutiveRemovalsTest()
    {
        DeeModel* model = dee_sequence_model_new();
        dee_model_set_schema(model, "s", "i", NULL);

        DeeListModel model_qt;
        model_qt.setModel(model);

        dee_model_append(model, "foo", 5);
        dee_model_append(model, "bar", 6);
        dee_model_append(model, "baz", 7);
        dee_model_append(model, "qoo", 8);

        QCOMPARE(model_qt.count(), 4);
        QCOMPARE(model_qt.rowCount(), 4);

        int num_removals = 0;

        connect(&model_qt, &QAbstractItemModel::rowsRemoved, [&num_removals] (const QModelIndex &parent, int start, int end) {
            num_removals++;
        });

        dee_model_begin_changeset(model);

        dee_model_clear(model);

        dee_model_end_changeset(model);

        QCOMPARE(num_removals, 1);
        QCOMPARE(model_qt.count(), 0);
        QCOMPARE(model_qt.rowCount(), 0);
    }
    
    void emitConsecutiveInsertsWithChangeTest()
    {
        DeeModel* model = dee_sequence_model_new();
        dee_model_set_schema(model, "s", "i", NULL);

        DeeListModel model_qt;
        model_qt.setModel(model);

        int num_insertions = 0;

        connect(&model_qt, &QAbstractItemModel::rowsInserted, [&num_insertions] (const QModelIndex &parent, int start, int end) {
            num_insertions++;
        });

        dee_model_begin_changeset(model);

        dee_model_append(model, "koo", -5);
        dee_model_append(model, "bar", 6);
        dee_model_append(model, "baz", 7);
        dee_model_set(model, dee_model_get_first_iter(model), "foo", 5);
        dee_model_append(model, "qoo", 8);

        QCOMPARE(num_insertions, 1);
        QCOMPARE(model_qt.count(), 3);
        QCOMPARE(model_qt.rowCount(), 3);

        QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("foo"));
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 5);
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("bar"));
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 6);
        QCOMPARE(model_qt.data(model_qt.index(2, 0), 0).toString(), QString("baz"));
        QCOMPARE(model_qt.data(model_qt.index(2, 0), 1).toInt(), 7);

        dee_model_end_changeset(model);

        QCOMPARE(num_insertions, 2);
        QCOMPARE(model_qt.count(), 4);
        QCOMPARE(model_qt.rowCount(), 4);

        QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("foo"));
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 5);
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("bar"));
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 6);
        QCOMPARE(model_qt.data(model_qt.index(2, 0), 0).toString(), QString("baz"));
        QCOMPARE(model_qt.data(model_qt.index(2, 0), 1).toInt(), 7);
        QCOMPARE(model_qt.data(model_qt.index(3, 0), 0).toString(), QString("qoo"));
        QCOMPARE(model_qt.data(model_qt.index(3, 0), 1).toInt(), 8);
    }

    void emitSignalsMixedTest()
    {
        DeeModel* model = dee_sequence_model_new();
        dee_model_set_schema(model, "s", "i", NULL);

        DeeListModel model_qt;
        model_qt.setModel(model);

        int num_insertions = 0;
        int num_removals = 0;

        connect(&model_qt, &QAbstractItemModel::rowsInserted, [&num_insertions] (const QModelIndex &parent, int start, int end) {
            num_insertions++;
        });

        connect(&model_qt, &QAbstractItemModel::rowsRemoved, [&num_removals] (const QModelIndex &parent, int start, int end) {
            num_removals++;
        });

        dee_model_begin_changeset(model);

        dee_model_append(model, "foo", 5);
        dee_model_append(model, "bar", 6);
        dee_model_append(model, "baz", 7);
        dee_model_append(model, "qoo", 8);

        dee_model_remove(model, dee_model_get_first_iter(model));
        dee_model_remove(model, dee_model_get_first_iter(model));

        dee_model_end_changeset(model);

        QCOMPARE(num_insertions, 1);
        QCOMPARE(num_removals, 1);
        QCOMPARE(model_qt.count(), 2);
        QCOMPARE(model_qt.rowCount(), 2);
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("baz"));
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 7);
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("qoo"));
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 8);

        // continue with more changes that aren't consecutive
        num_insertions = 0;
        num_removals = 0;

        dee_model_begin_changeset(model);
        
        dee_model_append(model, "foo", 5);
        dee_model_remove(model, dee_model_get_iter_at_row(model, 1));
        dee_model_remove(model, dee_model_get_iter_at_row(model, 0));
        dee_model_prepend(model, "qoo", 8);
        
        dee_model_end_changeset(model);

        QCOMPARE(num_insertions, 2);
        QCOMPARE(num_removals, 2);
        QCOMPARE(model_qt.count(), 2);
        QCOMPARE(model_qt.rowCount(), 2);
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("qoo"));
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 8);
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("foo"));
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 5);

        // and once again
        num_insertions = 0;
        num_removals = 0;

        dee_model_begin_changeset(model);

        dee_model_remove(model, dee_model_get_first_iter(model));   // ++ rem
        dee_model_append(model, "foo", 5);                          // ++ ins
        dee_model_clear(model);                                     // ++ rem
        dee_model_append(model, "baz", 7);
        dee_model_append(model, "qoo", 8);
        dee_model_prepend(model, "bar", 6);
        dee_model_prepend(model, "foo", 5);

        dee_model_end_changeset(model);

        QCOMPARE(num_insertions, 4);
        QCOMPARE(num_removals, 2);
        QCOMPARE(model_qt.count(), 4);
        QCOMPARE(model_qt.rowCount(), 4);
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("foo"));
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 5);
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("bar"));
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 6);
        QCOMPARE(model_qt.data(model_qt.index(2, 0), 0).toString(), QString("baz"));
        QCOMPARE(model_qt.data(model_qt.index(2, 0), 1).toInt(), 7);
        QCOMPARE(model_qt.data(model_qt.index(3, 0), 0).toString(), QString("qoo"));
        QCOMPARE(model_qt.data(model_qt.index(3, 0), 1).toInt(), 8);
    }

    void readDuringTransactionTest()
    {
        DeeModel* model = dee_sequence_model_new();
        dee_model_set_schema(model, "s", "i", NULL);

        DeeListModel model_qt;
        model_qt.setModel(model);

        int num_insertions = 0;
        int num_removals = 0;
        int state = 0;

        connect(&model_qt, &QAbstractItemModel::rowsInserted, [&] (const QModelIndex &parent, int start, int end) {
            num_insertions++;
            QVERIFY(state >= 0 && state <= 2);
            if (state == 0) {
                QCOMPARE(start, 0);
                QCOMPARE(end, 1);
                QCOMPARE(model_qt.rowCount(), 2);
                QCOMPARE(model_qt.count(), 2);
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("foo"));
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 5);
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("baz"));
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 7);
            } else if (state == 1) {
                QCOMPARE(start, 0);
                QCOMPARE(end, 0);
                QCOMPARE(model_qt.rowCount(), 3);
                QCOMPARE(model_qt.count(), 3);
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("bar"));
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 6);
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("foo"));
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 5);
                QCOMPARE(model_qt.data(model_qt.index(2, 0), 0).toString(), QString("baz"));
                QCOMPARE(model_qt.data(model_qt.index(2, 0), 1).toInt(), 7);
            } else if (state == 2) {
                QCOMPARE(start, 1);
                QCOMPARE(end, 1);
                QCOMPARE(model_qt.rowCount(), 2);
                QCOMPARE(model_qt.count(), 2);
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("baz"));
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 7);
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("qoo"));
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 8);
            }
        });

        connect(&model_qt, &QAbstractItemModel::rowsRemoved, [&] (const QModelIndex &parent, int start, int end) {
            num_removals++;
            QVERIFY(state >= 2 && state <= 2);
            if (state == 2) {
                QCOMPARE(start, 0);
                QCOMPARE(end, 1);
                QCOMPARE(model_qt.rowCount(), 1);
                QCOMPARE(model_qt.count(), 1);
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("baz"));
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 7);
            }
        });

        dee_model_begin_changeset(model);

        dee_model_append(model, "foo", 5);
        dee_model_append(model, "baz", 7);
        dee_model_prepend(model, "bar", 6);

        state = 1;

        dee_model_remove(model, dee_model_get_first_iter(model));
        dee_model_remove(model, dee_model_get_first_iter(model));

        state = 2;

        dee_model_append(model, "qoo", 8);

        dee_model_end_changeset(model);

        QCOMPARE(num_insertions, 3);
        QCOMPARE(num_removals, 1);
        QCOMPARE(model_qt.count(), 2);
        QCOMPARE(model_qt.rowCount(), 2);
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("baz"));
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 7);
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("qoo"));
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 8);
    }

    void readOutsideOfTransactionTest()
    {
        DeeModel* model = dee_sequence_model_new();
        dee_model_set_schema(model, "s", "i", NULL);

        DeeListModel model_qt;
        model_qt.setModel(model);

        int num_insertions = 0;
        int num_removals = 0;
        int state = 0;

        connect(&model_qt, &QAbstractItemModel::rowsInserted, [&] (const QModelIndex &parent, int start, int end) {
            num_insertions++;
            QVERIFY((state >= 0 && state <= 2) || state == 5);
            if (state == 0) {
                QCOMPARE(start, 0);
                QCOMPARE(end, 0);
                QCOMPARE(model_qt.rowCount(), 1);
                QCOMPARE(model_qt.count(), 1);
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("foo"));
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 5);
            } else if (state == 1) {
                QCOMPARE(start, 1);
                QCOMPARE(end, 1);
                QCOMPARE(model_qt.rowCount(), 2);
                QCOMPARE(model_qt.count(), 2);
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("foo"));
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 5);
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("baz"));
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 7);
            } else if (state == 2) {
                QCOMPARE(start, 0);
                QCOMPARE(end, 0);
                QCOMPARE(model_qt.rowCount(), 3);
                QCOMPARE(model_qt.count(), 3);
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("bar"));
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 6);
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("foo"));
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 5);
                QCOMPARE(model_qt.data(model_qt.index(2, 0), 0).toString(), QString("baz"));
                QCOMPARE(model_qt.data(model_qt.index(2, 0), 1).toInt(), 7);
            } else if (state == 5) {
                QCOMPARE(start, 1);
                QCOMPARE(end, 1);
                QCOMPARE(model_qt.rowCount(), 2);
                QCOMPARE(model_qt.count(), 2);
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("baz"));
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 7);
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("qoo"));
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 8);
            }
        });

        connect(&model_qt, &QAbstractItemModel::rowsRemoved, [&] (const QModelIndex &parent, int start, int end) {
            num_removals++;
            QVERIFY(state >= 3 && state <= 4);
            if (state == 3) {
                QCOMPARE(start, 0);
                QCOMPARE(end, 0);
                QCOMPARE(model_qt.rowCount(), 2);
                QCOMPARE(model_qt.count(), 2);
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("foo"));
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 5);
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("baz"));
                QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 7);
            } else if (state == 4) {
                QCOMPARE(start, 0);
                QCOMPARE(end, 0);
                QCOMPARE(model_qt.rowCount(), 1);
                QCOMPARE(model_qt.count(), 1);
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("baz"));
                QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 7);
            }
        });

        dee_model_append(model, "foo", 5);

        state = 1;

        dee_model_append(model, "baz", 7);

        state = 2;

        dee_model_prepend(model, "bar", 6);

        state = 3;

        dee_model_remove(model, dee_model_get_first_iter(model));

        state = 4;

        dee_model_remove(model, dee_model_get_first_iter(model));

        state = 5;

        dee_model_append(model, "qoo", 8);

        QCOMPARE(num_insertions, 4);
        QCOMPARE(num_removals, 2);
        QCOMPARE(model_qt.count(), 2);
        QCOMPARE(model_qt.rowCount(), 2);
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 0).toString(), QString("baz"));
        QCOMPARE(model_qt.data(model_qt.index(0, 0), 1).toInt(), 7);
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 0).toString(), QString("qoo"));
        QCOMPARE(model_qt.data(model_qt.index(1, 0), 1).toInt(), 8);
    }
};

QTEST_MAIN(DeeListModelTest)

#include "signaltest.moc"
