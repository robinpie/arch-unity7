/****************************************************************************
** Meta object code from reading C++ file 'SqliteUsageTracker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../hud-14.10+17.10.20170619/service/SqliteUsageTracker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SqliteUsageTracker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_hud__service__SqliteUsageTracker_t {
    QByteArrayData data[3];
    char stringdata0[51];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_hud__service__SqliteUsageTracker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_hud__service__SqliteUsageTracker_t qt_meta_stringdata_hud__service__SqliteUsageTracker = {
    {
QT_MOC_LITERAL(0, 0, 32), // "hud::service::SqliteUsageTracker"
QT_MOC_LITERAL(1, 33, 16), // "loadFromDatabase"
QT_MOC_LITERAL(2, 50, 0) // ""

    },
    "hud::service::SqliteUsageTracker\0"
    "loadFromDatabase\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_hud__service__SqliteUsageTracker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void hud::service::SqliteUsageTracker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SqliteUsageTracker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->loadFromDatabase(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject hud::service::SqliteUsageTracker::staticMetaObject = { {
    QMetaObject::SuperData::link<UsageTracker::staticMetaObject>(),
    qt_meta_stringdata_hud__service__SqliteUsageTracker.data,
    qt_meta_data_hud__service__SqliteUsageTracker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *hud::service::SqliteUsageTracker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *hud::service::SqliteUsageTracker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_hud__service__SqliteUsageTracker.stringdata0))
        return static_cast<void*>(this);
    return UsageTracker::qt_metacast(_clname);
}

int hud::service::SqliteUsageTracker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = UsageTracker::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
