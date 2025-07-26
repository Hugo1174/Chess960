/****************************************************************************
** Meta object code from reading C++ file 'gamewindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../gamewindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gamewindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_gamewindow_t {
    uint offsetsAndSizes[30];
    char stringdata0[11];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[4];
    char stringdata5[4];
    char stringdata6[17];
    char stringdata7[20];
    char stringdata8[18];
    char stringdata9[18];
    char stringdata10[15];
    char stringdata11[15];
    char stringdata12[5];
    char stringdata13[5];
    char stringdata14[23];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_gamewindow_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_gamewindow_t qt_meta_stringdata_gamewindow = {
    {
        QT_MOC_LITERAL(0, 10),  // "gamewindow"
        QT_MOC_LITERAL(11, 13),  // "menuRequested"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 15),  // "handleCellClick"
        QT_MOC_LITERAL(42, 3),  // "row"
        QT_MOC_LITERAL(46, 3),  // "col"
        QT_MOC_LITERAL(50, 16),  // "onNewGameClicked"
        QT_MOC_LITERAL(67, 19),  // "onBackToMenuClicked"
        QT_MOC_LITERAL(87, 17),  // "onPrevMoveClicked"
        QT_MOC_LITERAL(105, 17),  // "onNextMoveClicked"
        QT_MOC_LITERAL(123, 14),  // "onBoardChanged"
        QT_MOC_LITERAL(138, 14),  // "onMoveReceived"
        QT_MOC_LITERAL(153, 4),  // "Move"
        QT_MOC_LITERAL(158, 4),  // "move"
        QT_MOC_LITERAL(163, 22)   // "onOpponentDisconnected"
    },
    "gamewindow",
    "menuRequested",
    "",
    "handleCellClick",
    "row",
    "col",
    "onNewGameClicked",
    "onBackToMenuClicked",
    "onPrevMoveClicked",
    "onNextMoveClicked",
    "onBoardChanged",
    "onMoveReceived",
    "Move",
    "move",
    "onOpponentDisconnected"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_gamewindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   68,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    2,   69,    2, 0x08,    2 /* Private */,
       6,    0,   74,    2, 0x08,    5 /* Private */,
       7,    0,   75,    2, 0x08,    6 /* Private */,
       8,    0,   76,    2, 0x08,    7 /* Private */,
       9,    0,   77,    2, 0x08,    8 /* Private */,
      10,    0,   78,    2, 0x08,    9 /* Private */,
      11,    1,   79,    2, 0x08,   10 /* Private */,
      14,    0,   82,    2, 0x08,   12 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject gamewindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_gamewindow.offsetsAndSizes,
    qt_meta_data_gamewindow,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_gamewindow_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<gamewindow, std::true_type>,
        // method 'menuRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleCellClick'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onNewGameClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onBackToMenuClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPrevMoveClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onNextMoveClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onBoardChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMoveReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Move &, std::false_type>,
        // method 'onOpponentDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void gamewindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<gamewindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->menuRequested(); break;
        case 1: _t->handleCellClick((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 2: _t->onNewGameClicked(); break;
        case 3: _t->onBackToMenuClicked(); break;
        case 4: _t->onPrevMoveClicked(); break;
        case 5: _t->onNextMoveClicked(); break;
        case 6: _t->onBoardChanged(); break;
        case 7: _t->onMoveReceived((*reinterpret_cast< std::add_pointer_t<Move>>(_a[1]))); break;
        case 8: _t->onOpponentDisconnected(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (gamewindow::*)();
            if (_t _q_method = &gamewindow::menuRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *gamewindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *gamewindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_gamewindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int gamewindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void gamewindow::menuRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
