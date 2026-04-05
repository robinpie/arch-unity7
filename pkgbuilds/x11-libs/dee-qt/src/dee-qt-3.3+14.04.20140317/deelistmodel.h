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

#ifndef DEELISTMODEL_H
#define DEELISTMODEL_H

#include <QtCore/QAbstractListModel>

class DeeListModelPrivate;
typedef struct _DeeModel DeeModel;
typedef struct _GVariant GVariant;
class __attribute__ ((visibility ("default"))) DeeListModel : public QAbstractListModel
{
    friend class DeeListModelPrivate;

    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool synchronized READ synchronized NOTIFY synchronizedChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    DeeListModel(QObject *parent = 0);
    ~DeeListModel();

    Q_INVOKABLE QVariantMap get(int row);
    Q_INVOKABLE int count();

    /* Implementation of virtual methods from QAbstractListModel */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QHash<int, QByteArray> roleNames() const;

    /* getters */
    QString name() const;
    bool synchronized() const;

    /* setters */
    void setName(const QString& name);
    void setModel(DeeModel* model);

    /* expose variant conversions as static methods, we don't want them
     * in the global namespace */
    static QVariant VariantForData(GVariant*);
    static GVariant* DataFromVariant(const QVariant&);

Q_SIGNALS:
    void nameChanged(const QString&);
    void synchronizedChanged(bool);
    void roleNamesChanged(const QHash<int,QByteArray> &);
    void countChanged();

private:
    Q_DISABLE_COPY(DeeListModel)
    DeeListModelPrivate* const d;
};


#endif // DEELISTMODEL_H
