/****************************************************************************
** Meta object code from reading C++ file 'ApplicationListImpl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../hud-14.10+17.10.20170619/service/ApplicationListImpl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ApplicationListImpl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_hud__service__ApplicationListImpl_t {
    QByteArrayData data[10];
    char stringdata0[143];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_hud__service__ApplicationListImpl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_hud__service__ApplicationListImpl_t qt_meta_stringdata_hud__service__ApplicationListImpl = {
    {
QT_MOC_LITERAL(0, 0, 33), // "hud::service::ApplicationList..."
QT_MOC_LITERAL(1, 34, 20), // "FocusedWindowChanged"
QT_MOC_LITERAL(2, 55, 0), // ""
QT_MOC_LITERAL(3, 56, 8), // "windowId"
QT_MOC_LITERAL(4, 65, 13), // "applicationId"
QT_MOC_LITERAL(5, 79, 5), // "stage"
QT_MOC_LITERAL(6, 85, 13), // "WindowCreated"
QT_MOC_LITERAL(7, 99, 15), // "WindowDestroyed"
QT_MOC_LITERAL(8, 115, 19), // "serviceUnregistered"
QT_MOC_LITERAL(9, 135, 7) // "service"

    },
    "hud::service::ApplicationListImpl\0"
    "FocusedWindowChanged\0\0windowId\0"
    "applicationId\0stage\0WindowCreated\0"
    "WindowDestroyed\0serviceUnregistered\0"
    "service"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_hud__service__ApplicationListImpl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    3,   34,    2, 0x0a /* Public */,
       6,    2,   41,    2, 0x0a /* Public */,
       7,    2,   46,    2, 0x0a /* Public */,
       8,    1,   51,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::QString, QMetaType::UInt,    3,    4,    5,
    QMetaType::Void, QMetaType::UInt, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::UInt, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::QString,    9,

       0        // eod
};

void hud::service::ApplicationListImpl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ApplicationListImpl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->FocusedWindowChanged((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        case 1: _t->WindowCreated((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->WindowDestroyed((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->serviceUnregistered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject hud::service::ApplicationListImpl::staticMetaObject = { {
    QMetaObject::SuperData::link<ApplicationList::staticMetaObject>(),
    qt_meta_stringdata_hud__service__ApplicationListImpl.data,
    qt_meta_data_hud__service__ApplicationListImpl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *hud::service::ApplicationListImpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *hud::service::ApplicationListImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_hud__service__ApplicationListImpl.stringdata0))
        return static_cast<void*>(this);
    return ApplicationList::qt_metacast(_clname);
}

int hud::service::ApplicationListImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ApplicationList::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
