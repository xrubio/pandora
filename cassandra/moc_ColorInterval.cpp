/****************************************************************************
** Meta object code from reading C++ file 'ColorInterval.hxx'
**
** Created: Tue Feb 5 12:55:45 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ColorInterval.hxx"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ColorInterval.hxx' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI__ColorInterval[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x05,
      41,   19,   19,   19, 0x05,
      63,   19,   19,   19, 0x05,
      78,   19,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      93,   19,   19,   19, 0x08,
     107,   19,   19,   19, 0x08,
     115,   19,   19,   19, 0x08,
     130,  124,   19,   19, 0x0a,
     158,  152,   19,   19, 0x0a,
     176,  152,   19,   19, 0x0a,
     194,  152,   19,   19, 0x0a,
     214,  152,   19,   19, 0x0a,
     234,   19,   19,   19, 0x0a,
     254,   19,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GUI__ColorInterval[] = {
    "GUI::ColorInterval\0\0splitColorInterval()\0"
    "removeColorInterval()\0adjustMax(int)\0"
    "adjustMin(int)\0selectColor()\0split()\0"
    "remove()\0color\0colorSelected(QColor)\0"
    "value\0checkMinimum(int)\0checkMaximum(int)\0"
    "adjustMaxValue(int)\0adjustMinValue(int)\0"
    "updateMaxValue(int)\0updateMinValue(int)\0"
};

void GUI::ColorInterval::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ColorInterval *_t = static_cast<ColorInterval *>(_o);
        switch (_id) {
        case 0: _t->splitColorInterval(); break;
        case 1: _t->removeColorInterval(); break;
        case 2: _t->adjustMax((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->adjustMin((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->selectColor(); break;
        case 5: _t->split(); break;
        case 6: _t->remove(); break;
        case 7: _t->colorSelected((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 8: _t->checkMinimum((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->checkMaximum((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->adjustMaxValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->adjustMinValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->updateMaxValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->updateMinValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GUI::ColorInterval::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GUI::ColorInterval::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GUI__ColorInterval,
      qt_meta_data_GUI__ColorInterval, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI::ColorInterval::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI::ColorInterval::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI::ColorInterval::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI__ColorInterval))
        return static_cast<void*>(const_cast< ColorInterval*>(this));
    return QWidget::qt_metacast(_clname);
}

int GUI::ColorInterval::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void GUI::ColorInterval::splitColorInterval()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GUI::ColorInterval::removeColorInterval()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void GUI::ColorInterval::adjustMax(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GUI::ColorInterval::adjustMin(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
