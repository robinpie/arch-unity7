/****************************************************************************
** Meta object code from reading C++ file 'QtGMenuModel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../hud-14.10+17.10.20170619/libqtgmenu/internal/QtGMenuModel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtGMenuModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_qtgmenu__QtGMenuModel_t {
    QByteArrayData data[16];
    char stringdata0[184];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_qtgmenu__QtGMenuModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_qtgmenu__QtGMenuModel_t qt_meta_stringdata_qtgmenu__QtGMenuModel = {
    {
QT_MOC_LITERAL(0, 0, 21), // "qtgmenu::QtGMenuModel"
QT_MOC_LITERAL(1, 22, 16), // "MenuItemsChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 13), // "QtGMenuModel*"
QT_MOC_LITERAL(4, 54, 5), // "model"
QT_MOC_LITERAL(5, 60, 5), // "index"
QT_MOC_LITERAL(6, 66, 7), // "removed"
QT_MOC_LITERAL(7, 74, 5), // "added"
QT_MOC_LITERAL(8, 80, 15), // "ActionTriggered"
QT_MOC_LITERAL(9, 96, 11), // "action_name"
QT_MOC_LITERAL(10, 108, 7), // "checked"
QT_MOC_LITERAL(11, 116, 11), // "MenuInvalid"
QT_MOC_LITERAL(12, 128, 13), // "ActionEnabled"
QT_MOC_LITERAL(13, 142, 7), // "enabled"
QT_MOC_LITERAL(14, 150, 19), // "ActionParameterized"
QT_MOC_LITERAL(15, 170, 13) // "parameterized"

    },
    "qtgmenu::QtGMenuModel\0MenuItemsChanged\0"
    "\0QtGMenuModel*\0model\0index\0removed\0"
    "added\0ActionTriggered\0action_name\0"
    "checked\0MenuInvalid\0ActionEnabled\0"
    "enabled\0ActionParameterized\0parameterized"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qtgmenu__QtGMenuModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   44,    2, 0x06 /* Public */,
       8,    2,   53,    2, 0x06 /* Public */,
      11,    0,   58,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    2,   59,    2, 0x0a /* Public */,
      14,    2,   64,    2, 0x0a /* Public */,
       8,    1,   69,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int, QMetaType::Int,    4,    5,    6,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    9,   10,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    9,   13,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    9,   15,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void qtgmenu::QtGMenuModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QtGMenuModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->MenuItemsChanged((*reinterpret_cast< QtGMenuModel*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 1: _t->ActionTriggered((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->MenuInvalid(); break;
        case 3: _t->ActionEnabled((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->ActionParameterized((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->ActionTriggered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QtGMenuModel* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QtGMenuModel::*)(QtGMenuModel * , int , int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGMenuModel::MenuItemsChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (QtGMenuModel::*)(QString , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGMenuModel::ActionTriggered)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (QtGMenuModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGMenuModel::MenuInvalid)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject qtgmenu::QtGMenuModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_qtgmenu__QtGMenuModel.data,
    qt_meta_data_qtgmenu__QtGMenuModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *qtgmenu::QtGMenuModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qtgmenu::QtGMenuModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_qtgmenu__QtGMenuModel.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int qtgmenu::QtGMenuModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void qtgmenu::QtGMenuModel::MenuItemsChanged(QtGMenuModel * _t1, int _t2, int _t3, int _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qtgmenu::QtGMenuModel::ActionTriggered(QString _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qtgmenu::QtGMenuModel::MenuInvalid()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
