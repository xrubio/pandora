/****************************************************************************
** Meta object code from reading C++ file 'GenericStatistics.hxx'
**
** Created: Tue Feb 5 12:55:40 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "GenericStatistics.hxx"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GenericStatistics.hxx' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI__GenericStatistics[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      35,   24,   23,   23, 0x0a,
      94,   80,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GUI__GenericStatistics[] = {
    "GUI::GenericStatistics\0\0type,state\0"
    "calculateStatistics(std::string,std::string)\0"
    "newViewedStep\0viewedStepChangedSlot(int)\0"
};

void GUI::GenericStatistics::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GenericStatistics *_t = static_cast<GenericStatistics *>(_o);
        switch (_id) {
        case 0: _t->calculateStatistics((*reinterpret_cast< const std::string(*)>(_a[1])),(*reinterpret_cast< const std::string(*)>(_a[2]))); break;
        case 1: _t->viewedStepChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GUI::GenericStatistics::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GUI::GenericStatistics::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GUI__GenericStatistics,
      qt_meta_data_GUI__GenericStatistics, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI::GenericStatistics::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI::GenericStatistics::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI::GenericStatistics::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI__GenericStatistics))
        return static_cast<void*>(const_cast< GenericStatistics*>(this));
    return QWidget::qt_metacast(_clname);
}

int GUI::GenericStatistics::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
