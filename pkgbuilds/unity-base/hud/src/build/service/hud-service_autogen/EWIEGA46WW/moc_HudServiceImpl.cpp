/****************************************************************************
** Meta object code from reading C++ file 'HudServiceImpl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../hud-14.10+17.10.20170619/service/HudServiceImpl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HudServiceImpl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_hud__service__HudServiceImpl_t {
    QByteArrayData data[28];
    char stringdata0[370];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_hud__service__HudServiceImpl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_hud__service__HudServiceImpl_t qt_meta_stringdata_hud__service__HudServiceImpl = {
    {
QT_MOC_LITERAL(0, 0, 28), // "hud::service::HudServiceImpl"
QT_MOC_LITERAL(1, 29, 19), // "RegisterApplication"
QT_MOC_LITERAL(2, 49, 15), // "QDBusObjectPath"
QT_MOC_LITERAL(3, 65, 0), // ""
QT_MOC_LITERAL(4, 66, 2), // "id"
QT_MOC_LITERAL(5, 69, 11), // "CreateQuery"
QT_MOC_LITERAL(6, 81, 5), // "query"
QT_MOC_LITERAL(7, 87, 8), // "QString&"
QT_MOC_LITERAL(8, 96, 11), // "resultsName"
QT_MOC_LITERAL(9, 108, 12), // "appstackName"
QT_MOC_LITERAL(10, 121, 4), // "int&"
QT_MOC_LITERAL(11, 126, 13), // "modelRevision"
QT_MOC_LITERAL(12, 140, 10), // "StartQuery"
QT_MOC_LITERAL(13, 151, 7), // "entries"
QT_MOC_LITERAL(14, 159, 31), // "QList<hud::common::Suggestion>&"
QT_MOC_LITERAL(15, 191, 11), // "suggestions"
QT_MOC_LITERAL(16, 203, 13), // "QDBusVariant&"
QT_MOC_LITERAL(17, 217, 8), // "querykey"
QT_MOC_LITERAL(18, 226, 12), // "ExecuteQuery"
QT_MOC_LITERAL(19, 239, 12), // "QDBusVariant"
QT_MOC_LITERAL(20, 252, 3), // "key"
QT_MOC_LITERAL(21, 256, 9), // "timestamp"
QT_MOC_LITERAL(22, 266, 10), // "CloseQuery"
QT_MOC_LITERAL(23, 277, 13), // "legacyTimeout"
QT_MOC_LITERAL(24, 291, 12), // "Applications"
QT_MOC_LITERAL(25, 304, 30), // "QList<hud::common::NameObject>"
QT_MOC_LITERAL(26, 335, 11), // "OpenQueries"
QT_MOC_LITERAL(27, 347, 22) // "QList<QDBusObjectPath>"

    },
    "hud::service::HudServiceImpl\0"
    "RegisterApplication\0QDBusObjectPath\0"
    "\0id\0CreateQuery\0query\0QString&\0"
    "resultsName\0appstackName\0int&\0"
    "modelRevision\0StartQuery\0entries\0"
    "QList<hud::common::Suggestion>&\0"
    "suggestions\0QDBusVariant&\0querykey\0"
    "ExecuteQuery\0QDBusVariant\0key\0timestamp\0"
    "CloseQuery\0legacyTimeout\0Applications\0"
    "QList<hud::common::NameObject>\0"
    "OpenQueries\0QList<QDBusObjectPath>"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_hud__service__HudServiceImpl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       2,   74, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    3, 0x0a /* Public */,
       5,    4,   47,    3, 0x0a /* Public */,
      12,    4,   56,    3, 0x0a /* Public */,
      18,    2,   65,    3, 0x0a /* Public */,
      22,    1,   70,    3, 0x0a /* Public */,
      23,    0,   73,    3, 0x09 /* Protected */,

 // slots: parameters
    0x80000000 | 2, QMetaType::QString,    4,
    0x80000000 | 2, QMetaType::QString, 0x80000000 | 7, 0x80000000 | 7, 0x80000000 | 10,    6,    8,    9,   11,
    QMetaType::QString, QMetaType::QString, QMetaType::Int, 0x80000000 | 14, 0x80000000 | 16,    6,   13,   15,   17,
    QMetaType::Void, 0x80000000 | 19, QMetaType::UInt,   20,   21,
    QMetaType::Void, 0x80000000 | 19,   17,
    QMetaType::Void,

 // properties: name, type, flags
      24, 0x80000000 | 25, 0x00095009,
      26, 0x80000000 | 27, 0x00095009,

       0        // eod
};

void hud::service::HudServiceImpl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HudServiceImpl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { QDBusObjectPath _r = _t->RegisterApplication((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusObjectPath*>(_a[0]) = std::move(_r); }  break;
        case 1: { QDBusObjectPath _r = _t->CreateQuery((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QDBusObjectPath*>(_a[0]) = std::move(_r); }  break;
        case 2: { QString _r = _t->StartQuery((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QList<hud::common::Suggestion>(*)>(_a[3])),(*reinterpret_cast< QDBusVariant(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->ExecuteQuery((*reinterpret_cast< const QDBusVariant(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 4: _t->CloseQuery((*reinterpret_cast< const QDBusVariant(*)>(_a[1]))); break;
        case 5: _t->legacyTimeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QDBusVariant >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QDBusVariant >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QDBusObjectPath> >(); break;
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<hud::common::NameObject> >(); break;
        }
    }

#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<HudServiceImpl *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QList<hud::common::NameObject>*>(_v) = _t->applications(); break;
        case 1: *reinterpret_cast< QList<QDBusObjectPath>*>(_v) = _t->openQueries(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject hud::service::HudServiceImpl::staticMetaObject = { {
    QMetaObject::SuperData::link<HudService::staticMetaObject>(),
    qt_meta_stringdata_hud__service__HudServiceImpl.data,
    qt_meta_data_hud__service__HudServiceImpl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *hud::service::HudServiceImpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *hud::service::HudServiceImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_hud__service__HudServiceImpl.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QDBusContext"))
        return static_cast< QDBusContext*>(this);
    return HudService::qt_metacast(_clname);
}

int hud::service::HudServiceImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = HudService::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
