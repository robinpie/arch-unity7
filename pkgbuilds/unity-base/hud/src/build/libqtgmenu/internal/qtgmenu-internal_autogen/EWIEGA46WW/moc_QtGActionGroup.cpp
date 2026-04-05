/****************************************************************************
** Meta object code from reading C++ file 'QtGActionGroup.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../hud-14.10+17.10.20170619/libqtgmenu/internal/QtGActionGroup.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtGActionGroup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_qtgmenu__QtGActionGroup_t {
    QByteArrayData data[14];
    char stringdata0[177];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_qtgmenu__QtGActionGroup_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_qtgmenu__QtGActionGroup_t qt_meta_stringdata_qtgmenu__QtGActionGroup = {
    {
QT_MOC_LITERAL(0, 0, 23), // "qtgmenu::QtGActionGroup"
QT_MOC_LITERAL(1, 24, 11), // "ActionAdded"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 11), // "action_name"
QT_MOC_LITERAL(4, 49, 13), // "ActionRemoved"
QT_MOC_LITERAL(5, 63, 13), // "ActionEnabled"
QT_MOC_LITERAL(6, 77, 7), // "enabled"
QT_MOC_LITERAL(7, 85, 19), // "ActionParameterized"
QT_MOC_LITERAL(8, 105, 13), // "parameterized"
QT_MOC_LITERAL(9, 119, 18), // "ActionStateChanged"
QT_MOC_LITERAL(10, 138, 5), // "value"
QT_MOC_LITERAL(11, 144, 13), // "TriggerAction"
QT_MOC_LITERAL(12, 158, 7), // "checked"
QT_MOC_LITERAL(13, 166, 10) // "EmitStates"

    },
    "qtgmenu::QtGActionGroup\0ActionAdded\0"
    "\0action_name\0ActionRemoved\0ActionEnabled\0"
    "enabled\0ActionParameterized\0parameterized\0"
    "ActionStateChanged\0value\0TriggerAction\0"
    "checked\0EmitStates"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qtgmenu__QtGActionGroup[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    1,   52,    2, 0x06 /* Public */,
       5,    2,   55,    2, 0x06 /* Public */,
       7,    2,   60,    2, 0x06 /* Public */,
       9,    2,   65,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    2,   70,    2, 0x08 /* Private */,
      13,    0,   75,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,    3,   10,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,   12,
    QMetaType::Void,

       0        // eod
};

void qtgmenu::QtGActionGroup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QtGActionGroup *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->ActionAdded((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->ActionRemoved((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->ActionEnabled((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->ActionParameterized((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->ActionStateChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 5: _t->TriggerAction((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: _t->EmitStates(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QtGActionGroup::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGActionGroup::ActionAdded)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (QtGActionGroup::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGActionGroup::ActionRemoved)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (QtGActionGroup::*)(QString , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGActionGroup::ActionEnabled)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (QtGActionGroup::*)(QString , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGActionGroup::ActionParameterized)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (QtGActionGroup::*)(QString , QVariant );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QtGActionGroup::ActionStateChanged)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject qtgmenu::QtGActionGroup::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_qtgmenu__QtGActionGroup.data,
    qt_meta_data_qtgmenu__QtGActionGroup,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *qtgmenu::QtGActionGroup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qtgmenu::QtGActionGroup::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_qtgmenu__QtGActionGroup.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int qtgmenu::QtGActionGroup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void qtgmenu::QtGActionGroup::ActionAdded(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qtgmenu::QtGActionGroup::ActionRemoved(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qtgmenu::QtGActionGroup::ActionEnabled(QString _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void qtgmenu::QtGActionGroup::ActionParameterized(QString _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void qtgmenu::QtGActionGroup::ActionStateChanged(QString _t1, QVariant _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
