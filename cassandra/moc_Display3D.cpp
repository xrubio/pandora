/****************************************************************************
** Meta object code from reading C++ file 'Display3D.hxx'
**
** Created: Tue Feb 5 12:55:43 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "Display3D.hxx"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Display3D.hxx' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI__Display3D[] = {

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
      30,   16,   15,   15, 0x0a,
      63,   57,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GUI__Display3D[] = {
    "GUI::Display3D\0\0newViewedStep\0"
    "viewedStepChangedSlot(int)\0items\0"
    "rastersRearranged(std::list<std::string>)\0"
};

void GUI::Display3D::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Display3D *_t = static_cast<Display3D *>(_o);
        switch (_id) {
        case 0: _t->viewedStepChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->rastersRearranged((*reinterpret_cast< std::list<std::string>(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GUI::Display3D::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GUI::Display3D::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_GUI__Display3D,
      qt_meta_data_GUI__Display3D, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI::Display3D::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI::Display3D::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI::Display3D::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI__Display3D))
        return static_cast<void*>(const_cast< Display3D*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int GUI::Display3D::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
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
