/****************************************************************************
** Meta object code from reading C++ file 'Configurator3D.hxx'
**
** Created: Tue Feb 5 12:55:46 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "Configurator3D.hxx"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Configurator3D.hxx' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI__Configurator3D[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      51,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GUI__Configurator3D[] = {
    "GUI::Configurator3D\0\0configured3D(Configuration3D)\0"
    "accept()\0"
};

void GUI::Configurator3D::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Configurator3D *_t = static_cast<Configurator3D *>(_o);
        switch (_id) {
        case 0: _t->configured3D((*reinterpret_cast< const Configuration3D(*)>(_a[1]))); break;
        case 1: _t->accept(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GUI::Configurator3D::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GUI::Configurator3D::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GUI__Configurator3D,
      qt_meta_data_GUI__Configurator3D, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI::Configurator3D::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI::Configurator3D::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI::Configurator3D::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI__Configurator3D))
        return static_cast<void*>(const_cast< Configurator3D*>(this));
    return QDialog::qt_metacast(_clname);
}

int GUI::Configurator3D::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void GUI::Configurator3D::configured3D(const Configuration3D & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
