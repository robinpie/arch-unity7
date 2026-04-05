/****************************************************************************
** Meta object code from reading C++ file 'ApplicationImpl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../hud-14.10+17.10.20170619/service/ApplicationImpl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ApplicationImpl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_hud__service__ApplicationImpl_t {
    QByteArrayData data[16];
    char stringdata0[257];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_hud__service__ApplicationImpl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_hud__service__ApplicationImpl_t qt_meta_stringdata_hud__service__ApplicationImpl = {
    {
QT_MOC_LITERAL(0, 0, 29), // "hud::service::ApplicationImpl"
QT_MOC_LITERAL(1, 30, 10), // "AddSources"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 26), // "QList<hud::common::Action>"
QT_MOC_LITERAL(4, 69, 7), // "actions"
QT_MOC_LITERAL(5, 77, 31), // "QList<hud::common::Description>"
QT_MOC_LITERAL(6, 109, 12), // "descriptions"
QT_MOC_LITERAL(7, 122, 16), // "SetWindowContext"
QT_MOC_LITERAL(8, 139, 6), // "window"
QT_MOC_LITERAL(9, 146, 7), // "context"
QT_MOC_LITERAL(10, 154, 12), // "ActionGroups"
QT_MOC_LITERAL(11, 167, 31), // "QList<hud::common::ActionGroup>"
QT_MOC_LITERAL(12, 199, 11), // "DesktopPath"
QT_MOC_LITERAL(13, 211, 4), // "Icon"
QT_MOC_LITERAL(14, 216, 10), // "MenuModels"
QT_MOC_LITERAL(15, 227, 29) // "QList<hud::common::MenuModel>"

    },
    "hud::service::ApplicationImpl\0AddSources\0"
    "\0QList<hud::common::Action>\0actions\0"
    "QList<hud::common::Description>\0"
    "descriptions\0SetWindowContext\0window\0"
    "context\0ActionGroups\0"
    "QList<hud::common::ActionGroup>\0"
    "DesktopPath\0Icon\0MenuModels\0"
    "QList<hud::common::MenuModel>"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_hud__service__ApplicationImpl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       4,   34, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x0a /* Public */,
       7,    2,   29,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, QMetaType::UInt, QMetaType::QString,    8,    9,

 // properties: name, type, flags
      10, 0x80000000 | 11, 0x00095009,
      12, QMetaType::QString, 0x00095001,
      13, QMetaType::QString, 0x00095001,
      14, 0x80000000 | 15, 0x00095009,

       0        // eod
};

void hud::service::ApplicationImpl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ApplicationImpl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->AddSources((*reinterpret_cast< const QList<hud::common::Action>(*)>(_a[1])),(*reinterpret_cast< const QList<hud::common::Description>(*)>(_a[2]))); break;
        case 1: _t->SetWindowContext((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<hud::common::Action> >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<hud::common::Description> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<hud::common::ActionGroup> >(); break;
        case 3:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<hud::common::MenuModel> >(); break;
        }
    }

#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ApplicationImpl *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QList<hud::common::ActionGroup>*>(_v) = _t->actionGroups(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->desktopPath(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->icon(); break;
        case 3: *reinterpret_cast< QList<hud::common::MenuModel>*>(_v) = _t->menuModels(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject hud::service::ApplicationImpl::staticMetaObject = { {
    QMetaObject::SuperData::link<Application::staticMetaObject>(),
    qt_meta_stringdata_hud__service__ApplicationImpl.data,
    qt_meta_data_hud__service__ApplicationImpl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *hud::service::ApplicationImpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *hud::service::ApplicationImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_hud__service__ApplicationImpl.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QDBusContext"))
        return static_cast< QDBusContext*>(this);
    return Application::qt_metacast(_clname);
}

int hud::service::ApplicationImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Application::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
