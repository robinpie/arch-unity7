/****************************************************************************
** Meta object code from reading C++ file 'BamfWindowStack.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../hud-14.10+17.10.20170619/window-stack-bridge/BamfWindowStack.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BamfWindowStack.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BamfWindowStack_t {
    QByteArrayData data[21];
    char stringdata0[246];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BamfWindowStack_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BamfWindowStack_t qt_meta_stringdata_BamfWindowStack = {
    {
QT_MOC_LITERAL(0, 0, 15), // "BamfWindowStack"
QT_MOC_LITERAL(1, 16, 15), // "GetAppIdFromPid"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 3), // "pid"
QT_MOC_LITERAL(4, 37, 14), // "GetWindowStack"
QT_MOC_LITERAL(5, 52, 27), // "hud::common::WindowInfoList"
QT_MOC_LITERAL(6, 80, 19), // "GetWindowProperties"
QT_MOC_LITERAL(7, 100, 8), // "windowId"
QT_MOC_LITERAL(8, 109, 5), // "appId"
QT_MOC_LITERAL(9, 115, 5), // "names"
QT_MOC_LITERAL(10, 121, 19), // "GetWindowBusAddress"
QT_MOC_LITERAL(11, 141, 19), // "ActiveWindowChanged"
QT_MOC_LITERAL(12, 161, 9), // "oldWindow"
QT_MOC_LITERAL(13, 171, 9), // "newWindow"
QT_MOC_LITERAL(14, 181, 10), // "ViewClosed"
QT_MOC_LITERAL(15, 192, 4), // "path"
QT_MOC_LITERAL(16, 197, 4), // "type"
QT_MOC_LITERAL(17, 202, 10), // "ViewOpened"
QT_MOC_LITERAL(18, 213, 9), // "addWindow"
QT_MOC_LITERAL(19, 223, 9), // "WindowPtr"
QT_MOC_LITERAL(20, 233, 12) // "removeWindow"

    },
    "BamfWindowStack\0GetAppIdFromPid\0\0pid\0"
    "GetWindowStack\0hud::common::WindowInfoList\0"
    "GetWindowProperties\0windowId\0appId\0"
    "names\0GetWindowBusAddress\0ActiveWindowChanged\0"
    "oldWindow\0newWindow\0ViewClosed\0path\0"
    "type\0ViewOpened\0addWindow\0WindowPtr\0"
    "removeWindow"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BamfWindowStack[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x0a /* Public */,
       4,    0,   62,    2, 0x0a /* Public */,
       6,    3,   63,    2, 0x0a /* Public */,
      10,    1,   70,    2, 0x0a /* Public */,
      11,    2,   73,    2, 0x09 /* Protected */,
      14,    2,   78,    2, 0x09 /* Protected */,
      17,    2,   83,    2, 0x09 /* Protected */,
      18,    1,   88,    2, 0x09 /* Protected */,
      20,    1,   91,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::QString, QMetaType::UInt,    3,
    0x80000000 | 5,
    QMetaType::QStringList, QMetaType::UInt, QMetaType::QString, QMetaType::QStringList,    7,    8,    9,
    QMetaType::QStringList, QMetaType::UInt,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   12,   13,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   15,   16,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   15,   16,
    0x80000000 | 19, QMetaType::QString,   15,
    0x80000000 | 19, QMetaType::QString,   15,

       0        // eod
};

void BamfWindowStack::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BamfWindowStack *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { QString _r = _t->GetAppIdFromPid((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 1: { hud::common::WindowInfoList _r = _t->GetWindowStack();
            if (_a[0]) *reinterpret_cast< hud::common::WindowInfoList*>(_a[0]) = std::move(_r); }  break;
        case 2: { QStringList _r = _t->GetWindowProperties((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QStringList(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = std::move(_r); }  break;
        case 3: { QStringList _r = _t->GetWindowBusAddress((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = std::move(_r); }  break;
        case 4: _t->ActiveWindowChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->ViewClosed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->ViewOpened((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 7: { WindowPtr _r = _t->addWindow((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< WindowPtr*>(_a[0]) = std::move(_r); }  break;
        case 8: { WindowPtr _r = _t->removeWindow((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< WindowPtr*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject BamfWindowStack::staticMetaObject = { {
    QMetaObject::SuperData::link<AbstractWindowStack::staticMetaObject>(),
    qt_meta_stringdata_BamfWindowStack.data,
    qt_meta_data_BamfWindowStack,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *BamfWindowStack::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BamfWindowStack::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BamfWindowStack.stringdata0))
        return static_cast<void*>(this);
    return AbstractWindowStack::qt_metacast(_clname);
}

int BamfWindowStack::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractWindowStack::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
