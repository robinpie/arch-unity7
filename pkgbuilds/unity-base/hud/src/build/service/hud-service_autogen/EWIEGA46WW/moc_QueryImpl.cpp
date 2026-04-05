/****************************************************************************
** Meta object code from reading C++ file 'QueryImpl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../hud-14.10+17.10.20170619/service/QueryImpl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QueryImpl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_hud__service__QueryImpl_t {
    QByteArrayData data[26];
    char stringdata0[290];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_hud__service__QueryImpl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_hud__service__QueryImpl_t qt_meta_stringdata_hud__service__QueryImpl = {
    {
QT_MOC_LITERAL(0, 0, 23), // "hud::service::QueryImpl"
QT_MOC_LITERAL(1, 24, 10), // "CloseQuery"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 14), // "ExecuteCommand"
QT_MOC_LITERAL(4, 51, 12), // "QDBusVariant"
QT_MOC_LITERAL(5, 64, 4), // "item"
QT_MOC_LITERAL(6, 69, 9), // "timestamp"
QT_MOC_LITERAL(7, 79, 20), // "ExecuteParameterized"
QT_MOC_LITERAL(8, 100, 8), // "QString&"
QT_MOC_LITERAL(9, 109, 6), // "prefix"
QT_MOC_LITERAL(10, 116, 10), // "baseAction"
QT_MOC_LITERAL(11, 127, 16), // "QDBusObjectPath&"
QT_MOC_LITERAL(12, 144, 10), // "actionPath"
QT_MOC_LITERAL(13, 155, 9), // "modelPath"
QT_MOC_LITERAL(14, 165, 4), // "int&"
QT_MOC_LITERAL(15, 170, 12), // "modelSection"
QT_MOC_LITERAL(16, 183, 14), // "ExecuteToolbar"
QT_MOC_LITERAL(17, 198, 9), // "UpdateApp"
QT_MOC_LITERAL(18, 208, 3), // "app"
QT_MOC_LITERAL(19, 212, 11), // "UpdateQuery"
QT_MOC_LITERAL(20, 224, 5), // "query"
QT_MOC_LITERAL(21, 230, 10), // "VoiceQuery"
QT_MOC_LITERAL(22, 241, 19), // "serviceUnregistered"
QT_MOC_LITERAL(23, 261, 7), // "service"
QT_MOC_LITERAL(24, 269, 7), // "refresh"
QT_MOC_LITERAL(25, 277, 12) // "ToolbarItems"

    },
    "hud::service::QueryImpl\0CloseQuery\0\0"
    "ExecuteCommand\0QDBusVariant\0item\0"
    "timestamp\0ExecuteParameterized\0QString&\0"
    "prefix\0baseAction\0QDBusObjectPath&\0"
    "actionPath\0modelPath\0int&\0modelSection\0"
    "ExecuteToolbar\0UpdateApp\0app\0UpdateQuery\0"
    "query\0VoiceQuery\0serviceUnregistered\0"
    "service\0refresh\0ToolbarItems"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_hud__service__QueryImpl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       1,   98, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x0a /* Public */,
       3,    2,   60,    2, 0x0a /* Public */,
       7,    7,   65,    2, 0x0a /* Public */,
      16,    2,   80,    2, 0x0a /* Public */,
      17,    1,   85,    2, 0x0a /* Public */,
      19,    1,   88,    2, 0x0a /* Public */,
      21,    1,   91,    2, 0x0a /* Public */,
      22,    1,   94,    2, 0x09 /* Protected */,
      24,    0,   97,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, QMetaType::UInt,    5,    6,
    QMetaType::QString, 0x80000000 | 4, QMetaType::UInt, 0x80000000 | 8, 0x80000000 | 8, 0x80000000 | 11, 0x80000000 | 11, 0x80000000 | 14,    5,    6,    9,   10,   12,   13,   15,
    QMetaType::Void, QMetaType::QString, QMetaType::UInt,    5,    6,
    QMetaType::Int, QMetaType::QString,   18,
    QMetaType::Int, QMetaType::QString,   20,
    QMetaType::Int, 0x80000000 | 8,   20,
    QMetaType::Void, QMetaType::QString,   23,
    QMetaType::Void,

 // properties: name, type, flags
      25, QMetaType::QStringList, 0x00095001,

       0        // eod
};

void hud::service::QueryImpl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QueryImpl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->CloseQuery(); break;
        case 1: _t->ExecuteCommand((*reinterpret_cast< const QDBusVariant(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 2: { QString _r = _t->ExecuteParameterized((*reinterpret_cast< const QDBusVariant(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QDBusObjectPath(*)>(_a[5])),(*reinterpret_cast< QDBusObjectPath(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->ExecuteToolbar((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 4: { int _r = _t->UpdateApp((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 5: { int _r = _t->UpdateQuery((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 6: { int _r = _t->VoiceQuery((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 7: _t->serviceUnregistered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->refresh(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QDBusVariant >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QDBusVariant >(); break;
            }
            break;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<QueryImpl *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QStringList*>(_v) = _t->toolbarItems(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject hud::service::QueryImpl::staticMetaObject = { {
    QMetaObject::SuperData::link<Query::staticMetaObject>(),
    qt_meta_stringdata_hud__service__QueryImpl.data,
    qt_meta_data_hud__service__QueryImpl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *hud::service::QueryImpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *hud::service::QueryImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_hud__service__QueryImpl.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QDBusContext"))
        return static_cast< QDBusContext*>(this);
    return Query::qt_metacast(_clname);
}

int hud::service::QueryImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Query::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
