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

#include <QtCore/QHash>
#include <QtCore/QByteArray>

#include <dee.h>
#include <glib-object.h>

#include "deelistmodel.h"
#include "variantconversions.h"

class DeeListModelPrivate {
public:
    enum ProcessingState
    {
        START,
        ADDITIONS,
        REMOVALS
    };

    enum ChangeType
    {
        ADDITION,
        REMOVAL
    };

    DeeListModelPrivate(DeeListModel* parent);
    ~DeeListModelPrivate();

    void connectToDeeModel();
    void connectToDeeModel(DeeModel *model);
    void disconnectFromDeeModel();
    void createRoles();
    bool synchronized() const;

    void processChange(ChangeType changeType, int changePos);
    void flushChanges();

    /* GObject signal handlers for m_deeModel */
    static void onSynchronizedChanged(GObject* emitter, GParamSpec *pspec, DeeListModel* model);
    static void onRowAdded(GObject* emitter, DeeModelIter* iter, DeeListModel* model);
    static void onRowRemoved(GObject* emitter, DeeModelIter* iter, DeeListModel* model);
    static void onRowChanged(GObject* emitter, DeeModelIter* iter, DeeListModel* model);

    static void onStartChangeset(DeeListModel* model);
    static void onFinishChangeset(DeeListModel* model);

    DeeListModel* m_parent;
    DeeModel* m_deeModel;
    QString m_name;
    int m_count;
    bool m_listeningSynchronized;
    QHash<int, QByteArray> m_roleNames;
    int m_rowBeingAdded;
    int m_rowBeingRemoved;
    ProcessingState m_changesetState;
    bool m_changesetInProgress;
    int m_changesetRowStart;
    int m_changesetRowEnd;
};

DeeListModelPrivate::DeeListModelPrivate(DeeListModel* parent)
    : m_parent(parent)
    , m_deeModel(NULL)
    , m_count(0)
    , m_listeningSynchronized(false)
    , m_rowBeingAdded(-1)
    , m_rowBeingRemoved(-1)
    , m_changesetState(ProcessingState::START)
    , m_changesetInProgress(false)
    , m_changesetRowStart(-1)
    , m_changesetRowEnd(-1)
{
}

DeeListModelPrivate::~DeeListModelPrivate()
{
    disconnectFromDeeModel();
}

void
DeeListModelPrivate::disconnectFromDeeModel()
{
    if (m_deeModel != NULL) {
        /* Disconnect from all GObject properties */
        if (m_listeningSynchronized) {
            g_object_disconnect(m_deeModel, "any_signal", G_CALLBACK(onSynchronizedChanged), m_parent, NULL);
            m_listeningSynchronized = false;
        }
        g_object_disconnect(m_deeModel, "any_signal", G_CALLBACK(onRowAdded), m_parent,
                                        "any_signal", G_CALLBACK(onRowRemoved), m_parent,
                                        "any_signal", G_CALLBACK(onRowChanged), m_parent,
                                        "any_signal", G_CALLBACK(onStartChangeset), m_parent,
                                        "any_signal", G_CALLBACK(onFinishChangeset), m_parent,
                                        NULL);

        g_object_unref(m_deeModel);
        m_deeModel = NULL;
        Q_EMIT m_parent->synchronizedChanged(false);
    }
}

void
DeeListModelPrivate::connectToDeeModel()
{
    if (!m_name.isEmpty())
    {
        DeeModel* model = dee_shared_model_new(m_name.toUtf8().data());
        connectToDeeModel(model);
        g_object_unref(model);
    }
    else
    {
        disconnectFromDeeModel();
    }
}

void
DeeListModelPrivate::connectToDeeModel(DeeModel *model)
{
    disconnectFromDeeModel();

    m_deeModel = (DeeModel*)g_object_ref (model);
    g_signal_connect(m_deeModel, "row-added", G_CALLBACK(onRowAdded), m_parent);
    g_signal_connect(m_deeModel, "row-removed", G_CALLBACK(onRowRemoved), m_parent);
    g_signal_connect(m_deeModel, "row-changed", G_CALLBACK(onRowChanged), m_parent);
    g_signal_connect_swapped(m_deeModel, "changeset-started", G_CALLBACK(onStartChangeset), m_parent);
    g_signal_connect_swapped(m_deeModel, "changeset-finished", G_CALLBACK(onFinishChangeset), m_parent);
    if (synchronized())
    {
        createRoles();
        m_parent->beginResetModel();
        m_count = dee_model_get_n_rows(m_deeModel);
        m_parent->endResetModel();
        Q_EMIT m_parent->countChanged();
    }
    else
    {
        g_signal_connect(m_deeModel, "notify::synchronized", G_CALLBACK(onSynchronizedChanged), m_parent);
        m_listeningSynchronized = true;
    }
}

bool
DeeListModelPrivate::synchronized() const
{
    if (m_deeModel == NULL) {
        return false;
    }

    if (DEE_IS_SHARED_MODEL(m_deeModel)) {
        return dee_shared_model_is_synchronized(DEE_SHARED_MODEL(m_deeModel));
    }

    return true;
}

void
DeeListModelPrivate::flushChanges()
{
    bool countChanged = false;
    // The problem we're facing here is that the signals emitted by DeeListModel are not completely
    // in sync with the backend DeeModel - Qt will likely call our data() method right after calling
    // end{Insert|Remove}Rows() and the backend model might still have extra rows, because we're
    // inside onRowRemoved/onRowAdded callback
    // See the data() method for more details
    if (m_changesetState == ProcessingState::ADDITIONS) {
        /* Force emission of QAbstractItemModel::rowsInserted by calling
           beginInsertRows and endInsertRows. Necessary because according to the
           documentation:
           "It can only be emitted by the QAbstractItemModel implementation, and
            cannot be explicitly emitted in subclass code."
        */
        m_parent->beginInsertRows(QModelIndex(), m_changesetRowStart, m_changesetRowEnd);
        m_count += m_changesetRowEnd - m_changesetRowStart + 1;
        countChanged = true;
        m_parent->endInsertRows();
    } else if (m_changesetState == ProcessingState::REMOVALS) {
        /* Force emission of QAbstractItemModel::rowsRemoved by calling
           beginRemoveRows and endRemoveRows. Necessary because according to the
           documentation:
           "It can only be emitted by the QAbstractItemModel implementation, and
            cannot be explicitly emitted in subclass code."
        */
        m_parent->beginRemoveRows(QModelIndex(), m_changesetRowStart, m_changesetRowEnd);
        m_count -= m_changesetRowEnd - m_changesetRowStart + 1;
        countChanged = true;
        m_parent->endRemoveRows();
    }

    if (countChanged) Q_EMIT m_parent->countChanged();

    m_changesetState = ProcessingState::START;
    m_changesetRowStart = -1;
    m_changesetRowEnd = -1;
}

void
DeeListModelPrivate::processChange(ChangeType changeType, int changePos)
{
    /* flush if changeType doesn't match current processing state */
    if (m_changesetState != ProcessingState::START &&
            ((changeType == ChangeType::ADDITION && m_changesetState != ProcessingState::ADDITIONS) ||
            (changeType == ChangeType::REMOVAL && m_changesetState != ProcessingState::REMOVALS))) {
        flushChanges();
    }

    /* flush also if current changeType isn't consecutive:
     * - consecutive additions are if changePos == m_changesetRowEnd + 1
     * - consecutive removals are if changePos == m_changesetRowStart
     */
    if ((m_changesetState == ProcessingState::ADDITIONS && changePos != m_changesetRowEnd + 1) ||
            (m_changesetState == ProcessingState::REMOVALS && changePos != m_changesetRowStart)) {
        flushChanges();
    }

    switch (m_changesetState) {
        case ProcessingState::START:
            switch (changeType) {
                case ChangeType::ADDITION:
                    m_changesetState = ProcessingState::ADDITIONS;
                    m_changesetRowStart = changePos;
                    m_changesetRowEnd = changePos;
                    break;
                case ChangeType::REMOVAL:
                    m_changesetState = ProcessingState::REMOVALS;
                    m_changesetRowStart = changePos;
                    m_changesetRowEnd = changePos;
                    break;
                default: break;
            }
            break;
        case ProcessingState::ADDITIONS:
            m_changesetRowEnd = changePos;
            break;
        case ProcessingState::REMOVALS:
            m_changesetRowEnd++;
            break;
    }
}

void
DeeListModelPrivate::createRoles()
{
    if (m_deeModel == NULL) {
        return;
    }

    QHash<int, QByteArray> roles;
    QString column;
    guint n_columns = dee_model_get_n_columns(m_deeModel);

    for (unsigned int index=0; index<n_columns; index++)
    {
        column = QString("column_%1").arg(index);
        roles[index] = column.toLocal8Bit();
    }

#if WITHQT5==0
    // In Qt4, roleNames() is non-virtual and non-const, so we still need to
    // call the old setRoleNames() which doesn't exist any more in Qt5.
    m_parent->setRoleNames(roles);
#endif
    m_roleNames = roles;
    Q_EMIT m_parent->roleNamesChanged(roles);
}

void
DeeListModelPrivate::onSynchronizedChanged(GObject* emitter __attribute__ ((unused)),
                                           GParamSpec *pspec,
                                           DeeListModel *model)
{
    model->d->createRoles();
    model->beginResetModel();
    model->d->m_count = dee_model_get_n_rows(model->d->m_deeModel);
    model->synchronizedChanged(model->synchronized());
    model->endResetModel();
    Q_EMIT model->countChanged();
}

void
DeeListModelPrivate::onRowAdded(GObject *emitter __attribute__ ((unused)),
                                DeeModelIter* iter,
                                DeeListModel* model)
{
    if(!model->synchronized()) {
        return;
    }

    gint position = dee_model_get_position(model->d->m_deeModel, iter);

    /* if we're inside transaction, we'll consider this row hidden */
    model->d->m_rowBeingAdded = position;
    model->d->processChange(ChangeType::ADDITION, position);
    if (!model->d->m_changesetInProgress) {
        model->d->flushChanges();
    }
    model->d->m_rowBeingAdded = -1;
}

void
DeeListModelPrivate::onRowRemoved(GObject *emitter __attribute__ ((unused)),
                                  DeeModelIter* iter,
                                  DeeListModel* model)
{
    if(!model->synchronized()) {
        return;
    }

    /* Note that at this point the row is still present and valid in the DeeModel.
       Therefore the value returned by dee_model_get_n_rows() might not be
       what one would expect.
       See Dee's dee_sequence_model_remove() method.
    */
    gint position = dee_model_get_position(model->d->m_deeModel, iter);

    model->d->m_rowBeingRemoved = position;
    model->d->processChange(ChangeType::REMOVAL, position);
    if (!model->d->m_changesetInProgress) {
        model->d->flushChanges();
    }
    model->d->m_rowBeingRemoved = -1;
}

void
DeeListModelPrivate::onRowChanged(GObject *emitter __attribute__ ((unused)),
                                  DeeModelIter* iter,
                                  DeeListModel* model)
{
    if(!model->synchronized()) {
        return;
    }

    /* If we're inside a transaction we need to flush the currently queued
     * changes and emit the dataChanged signal after that */
    if (model->d->m_changesetInProgress) {
        model->d->flushChanges();
    }

    gint position = dee_model_get_position(model->d->m_deeModel, iter);
    QModelIndex index = model->index(position);
    Q_EMIT model->dataChanged(index, index);
}

void
DeeListModelPrivate::onStartChangeset(DeeListModel* model)
{
    model->d->m_changesetInProgress = true;
}

void
DeeListModelPrivate::onFinishChangeset(DeeListModel* model)
{
    model->d->flushChanges();
    model->d->m_changesetInProgress = false;
}



DeeListModel::DeeListModel(QObject *parent) :
    QAbstractListModel(parent), d(new DeeListModelPrivate(this))
{
    g_type_init();
}

DeeListModel::~DeeListModel()
{
    delete d;
}

QString
DeeListModel::name() const
{
    return d->m_name;
}

void
DeeListModel::setName(const QString& name)
{
    if (name != d->m_name) {
        d->m_name = name;
        Q_EMIT nameChanged(d->m_name);
        d->connectToDeeModel();
    }
}

void
DeeListModel::setModel(DeeModel *model)
{
    if (model != NULL) {
        d->connectToDeeModel(model);
    } else {
        d->disconnectFromDeeModel();
    }
}


bool
DeeListModel::synchronized() const
{
    return d->synchronized();
}

int
DeeListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (d->m_deeModel == NULL || !synchronized()) {
        return 0;
    }

    return d->m_count;
}

QHash<int, QByteArray>
DeeListModel::roleNames() const
{
    return d->m_roleNames;
}

QVariant
DeeListModel::data(const QModelIndex &index, int role) const
{
    if (d->m_deeModel == NULL || !synchronized()) {
        return QVariant();
    }

    if (!index.isValid())
        return QVariant();

    /* Assume role is always a positive integer */
    if ((unsigned int)role >= dee_model_get_n_columns(d->m_deeModel)) {
        return QVariant();
    }

    GVariant* gvariant;
    DeeModelIter* iter;

    int row = index.row();
    // we're inside the row-{added|removed} callback and the backend model still has the removed row
    // (in case of row-removed) or already has the added row (in case of row-added), there are two cases:
    // 1) inside a transaction, we need to hide about-to-be-added row (because we just flushed changes unrelated
    //    to this addition)
    // 2) outside of transaction, we need to hide about-to-be-removed row (because we're flushing right after
    //    receiving the signal)
    if (d->m_changesetInProgress) {
        if (d->m_rowBeingAdded >= 0 && row >= d->m_rowBeingAdded) {
            // there's an extra row in the backend, skip it by incrementing
            row++;
        }
    } else {
        if (d->m_rowBeingRemoved >= 0 && row >= d->m_rowBeingRemoved) {
            // we need to skip the about-to-be-removed row
            row++;
        }
    }

    iter = dee_model_get_iter_at_row(d->m_deeModel, row);
    gvariant = dee_model_get_value(d->m_deeModel, iter, role);
    QVariant qvariant = QVariantFromGVariant(gvariant);
    g_variant_unref(gvariant);

    return qvariant;
}

QVariantMap
DeeListModel::get(int row)
{
    if (d->m_deeModel == NULL || !synchronized()) {
        return QVariantMap();
    }

    QVariantMap result;
    QHashIterator<int, QByteArray> i(roleNames());
    while (i.hasNext()) {
        i.next();
        QModelIndex modelIndex = index(row);
        QVariant data = modelIndex.data(i.key());
        result[i.value()] = data;
     }
     return result;
}

int
DeeListModel::count()
{
    return rowCount();
}

QVariant DeeListModel::VariantForData(GVariant* data)
{
  return QVariantFromGVariant(data);
}

GVariant* DeeListModel::DataFromVariant(const QVariant& variant)
{
  return GVariantFromQVariant(variant);
}

