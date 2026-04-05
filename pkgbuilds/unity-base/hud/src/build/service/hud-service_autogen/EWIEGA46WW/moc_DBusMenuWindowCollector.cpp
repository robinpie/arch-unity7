/****************************************************************************
** Meta object code from reading C++ file 'DBusMenuWindowCollector.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../hud-14.10+17.10.20170619/service/DBusMenuWindowCollector.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DBusMenuWindowCollector.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_hud__service__DBusMenuWindowCollector_t {
    QByteArrayData data[7];
    char stringdata0[104];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_hud__service__DBusMenuWindowCollector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_hud__service__DBusMenuWindowCollector_t qt_meta_stringdata_hud__service__DBusMenuWindowCollector = {
    {
QT_MOC_LITERAL(0, 0, 37), // "hud::service::DBusMenuWindowC..."
QT_MOC_LITERAL(1, 38, 16), // "WindowRegistered"
QT_MOC_LITERAL(2, 55, 0), // ""
QT_MOC_LITERAL(3, 56, 8), // "windowId"
QT_MOC_LITERAL(4, 65, 7), // "service"
QT_MOC_LITERAL(5, 73, 15), // "QDBusObjectPath"
QT_MOC_LITERAL(6, 89, 14) // "menuObjectPath"

    },
    "hud::service::DBusMenuWindowCollector\0"
    "WindowRegistered\0\0windowId\0service\0"
    "QDBusObjectPath\0menuObjectPath"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_hud__service__DBusMenuWindowCollector[] = {

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
       1,    3,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::QString, 0x80000000 | 5,    3,    4,    6,

       0        // eod
};

void hud::service::DBusMenuWindowCollector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DBusMenuWindowCollector *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->WindowRegistered((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QDBusObjectPath(*)>(_a[3]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject hud::service::DBusMenuWindowCollector::staticMetaObject = { {
    QMetaObject::SuperData::link<Collector::staticMetaObject>(),
    qt_meta_stringdata_hud__service__DBusMenuWindowCollector.data,
    qt_meta_data_hud__service__DBusMenuWindowCollector,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *hud::service::DBusMenuWindowCollector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *hud::service::DBusMenuWindowCollector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_hud__service__DBusMenuWindowCollector.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "std::enable_shared_from_this<DBusMenuWindowCollector>"))
        return static_cast< std::enable_shared_from_this<DBusMenuWindowCollector>*>(this);
    return Collector::qt_metacast(_clname);
}

int hud::service::DBusMenuWindowCollector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Collector::qt_metacall(_c, _id, _a);
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
