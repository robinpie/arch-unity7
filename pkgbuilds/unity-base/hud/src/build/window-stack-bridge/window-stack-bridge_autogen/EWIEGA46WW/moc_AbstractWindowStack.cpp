/****************************************************************************
** Meta object code from reading C++ file 'AbstractWindowStack.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../hud-14.10+17.10.20170619/window-stack-bridge/AbstractWindowStack.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AbstractWindowStack.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AbstractWindowStack_t {
    QByteArrayData data[15];
    char stringdata0[202];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AbstractWindowStack_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AbstractWindowStack_t qt_meta_stringdata_AbstractWindowStack = {
    {
QT_MOC_LITERAL(0, 0, 19), // "AbstractWindowStack"
QT_MOC_LITERAL(1, 20, 20), // "FocusedWindowChanged"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 8), // "windowId"
QT_MOC_LITERAL(4, 51, 5), // "appId"
QT_MOC_LITERAL(5, 57, 5), // "stage"
QT_MOC_LITERAL(6, 63, 13), // "WindowCreated"
QT_MOC_LITERAL(7, 77, 15), // "WindowDestroyed"
QT_MOC_LITERAL(8, 93, 15), // "GetAppIdFromPid"
QT_MOC_LITERAL(9, 109, 3), // "pid"
QT_MOC_LITERAL(10, 113, 14), // "GetWindowStack"
QT_MOC_LITERAL(11, 128, 27), // "hud::common::WindowInfoList"
QT_MOC_LITERAL(12, 156, 19), // "GetWindowProperties"
QT_MOC_LITERAL(13, 176, 5), // "names"
QT_MOC_LITERAL(14, 182, 19) // "GetWindowBusAddress"

    },
    "AbstractWindowStack\0FocusedWindowChanged\0"
    "\0windowId\0appId\0stage\0WindowCreated\0"
    "WindowDestroyed\0GetAppIdFromPid\0pid\0"
    "GetWindowStack\0hud::common::WindowInfoList\0"
    "GetWindowProperties\0names\0GetWindowBusAddress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AbstractWindowStack[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   49,    2, 0x06 /* Public */,
       6,    2,   56,    2, 0x06 /* Public */,
       7,    2,   61,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   66,    2, 0x0a /* Public */,
      10,    0,   69,    2, 0x0a /* Public */,
      12,    3,   70,    2, 0x0a /* Public */,
      14,    1,   77,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::QString, QMetaType::UInt,    3,    4,    5,
    QMetaType::Void, QMetaType::UInt, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::UInt, QMetaType::QString,    3,    4,

 // slots: parameters
    QMetaType::QString, QMetaType::UInt,    9,
    0x80000000 | 11,
    QMetaType::QStringList, QMetaType::UInt, QMetaType::QString, QMetaType::QStringList,    3,    4,   13,
    QMetaType::QStringList, QMetaType::UInt,    3,

       0        // eod
};

void AbstractWindowStack::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AbstractWindowStack *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->FocusedWindowChanged((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        case 1: _t->WindowCreated((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->WindowDestroyed((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: { QString _r = _t->GetAppIdFromPid((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 4: { hud::common::WindowInfoList _r = _t->GetWindowStack();
            if (_a[0]) *reinterpret_cast< hud::common::WindowInfoList*>(_a[0]) = std::move(_r); }  break;
        case 5: { QStringList _r = _t->GetWindowProperties((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QStringList(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = std::move(_r); }  break;
        case 6: { QStringList _r = _t->GetWindowBusAddress((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AbstractWindowStack::*)(uint , const QString & , uint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractWindowStack::FocusedWindowChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AbstractWindowStack::*)(uint , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractWindowStack::WindowCreated)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AbstractWindowStack::*)(uint , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractWindowStack::WindowDestroyed)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AbstractWindowStack::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_AbstractWindowStack.data,
    qt_meta_data_AbstractWindowStack,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AbstractWindowStack::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AbstractWindowStack::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AbstractWindowStack.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QDBusContext"))
        return static_cast< QDBusContext*>(this);
    return QObject::qt_metacast(_clname);
}

int AbstractWindowStack::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void AbstractWindowStack::FocusedWindowChanged(uint _t1, const QString & _t2, uint _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AbstractWindowStack::WindowCreated(uint _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AbstractWindowStack::WindowDestroyed(uint _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
