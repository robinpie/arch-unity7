/****************************************************************************
** Meta object code from reading C++ file 'QtGMenuImporter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../hud-14.10+17.10.20170619/libqtgmenu/QtGMenuImporter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtGMenuImporter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_qtgmenu__QtGMenuImporter_t {
    QByteArrayData data[10];
    char stringdata0[128];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_qtgmenu__QtGMenuImporter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_qtgmenu__QtGMenuImporter_t qt_meta_stringdata_qtgmenu__QtGMenuImporter = {
    {
QT_MOC_LITERAL(0, 0, 24), // "qtgmenu::QtGMenuImporter"
QT_MOC_LITERAL(1, 25, 16), // "MenuItemsChanged"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 11), // "ActionAdded"
QT_MOC_LITERAL(4, 55, 11), // "action_name"
QT_MOC_LITERAL(5, 67, 13), // "ActionRemoved"
QT_MOC_LITERAL(6, 81, 13), // "ActionEnabled"
QT_MOC_LITERAL(7, 95, 7), // "enabled"
QT_MOC_LITERAL(8, 103, 18), // "ActionStateChanged"
QT_MOC_LITERAL(9, 122, 5) // "value"

    },
    "qtgmenu::QtGMenuImporter\0MenuItemsChanged\0"
    "\0ActionAdded\0action_name\0ActionRemoved\0"
    "ActionEnabled\0enabled\0ActionStateChanged\0"
    "value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qtgmenu__QtGMenuImporter[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    1,   40,    2, 0x06 /* Public */,
       5,    1,   43,    2, 0x06 /* Public */,
       6,    2,   46,    2, 0x06 /* Public */,
       8,    2,   51,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    4,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,    4,    9,

       0        // eod
};

void qtgmenu::QtGMenuImporter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QtGMenuImporter *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->MenuItemsChanged(); break;
        case 1: _t->ActionAdded((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->ActionRemoved((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->ActionEnabled((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->ActionStateChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QtGMenuImporter::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGMenuImporter::MenuItemsChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (QtGMenuImporter::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGMenuImporter::ActionAdded)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (QtGMenuImporter::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGMenuImporter::ActionRemoved)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (QtGMenuImporter::*)(QString , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGMenuImporter::ActionEnabled)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (QtGMenuImporter::*)(QString , QVariant );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGMenuImporter::ActionStateChanged)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject qtgmenu::QtGMenuImporter::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_qtgmenu__QtGMenuImporter.data,
    qt_meta_data_qtgmenu__QtGMenuImporter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *qtgmenu::QtGMenuImporter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qtgmenu::QtGMenuImporter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_qtgmenu__QtGMenuImporter.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int qtgmenu::QtGMenuImporter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void qtgmenu::QtGMenuImporter::MenuItemsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void qtgmenu::QtGMenuImporter::ActionAdded(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qtgmenu::QtGMenuImporter::ActionRemoved(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void qtgmenu::QtGMenuImporter::ActionEnabled(QString _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void qtgmenu::QtGMenuImporter::ActionStateChanged(QString _t1, QVariant _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
