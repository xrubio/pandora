/****************************************************************************
** Meta object code from reading C++ file 'Display2D.hxx'
**
** Created: Tue Feb 5 12:55:36 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "Display2D.hxx"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Display2D.hxx' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI__Display2D[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   16,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      77,   63,   15,   15, 0x0a,
     104,   15,   15,   15, 0x0a,
     113,   15,   15,   15, 0x0a,
     123,   15,   15,   15, 0x0a,
     141,  136,   15,   15, 0x0a,
     172,  136,   15,   15, 0x0a,
     210,  204,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GUI__Display2D[] = {
    "GUI::Display2D\0\0,\0"
    "calculateStatistics(std::string,std::string)\0"
    "newViewedStep\0viewedStepChangedSlot(int)\0"
    "zoomIn()\0zoomOut()\0showAgents()\0item\0"
    "typeSelected(QListWidgetItem*)\0"
    "stateSelected(QListWidgetItem*)\0items\0"
    "rastersRearranged(std::list<std::string>)\0"
};

void GUI::Display2D::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Display2D *_t = static_cast<Display2D *>(_o);
        switch (_id) {
        case 0: _t->calculateStatistics((*reinterpret_cast< const std::string(*)>(_a[1])),(*reinterpret_cast< const std::string(*)>(_a[2]))); break;
        case 1: _t->viewedStepChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->zoomIn(); break;
        case 3: _t->zoomOut(); break;
        case 4: _t->showAgents(); break;
        case 5: _t->typeSelected((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 6: _t->stateSelected((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 7: _t->rastersRearranged((*reinterpret_cast< std::list<std::string>(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GUI::Display2D::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GUI::Display2D::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GUI__Display2D,
      qt_meta_data_GUI__Display2D, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI::Display2D::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI::Display2D::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI::Display2D::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI__Display2D))
        return static_cast<void*>(const_cast< Display2D*>(this));
    return QWidget::qt_metacast(_clname);
}

int GUI::Display2D::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void GUI::Display2D::calculateStatistics(const std::string & _t1, const std::string & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
