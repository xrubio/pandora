/****************************************************************************
** Meta object code from reading C++ file 'RasterConfigurator.hxx'
**
** Created: Tue Feb 5 12:55:44 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "RasterConfigurator.hxx"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RasterConfigurator.hxx' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI__RasterConfigurator[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   25,   24,   24, 0x05,

 // slots: signature, parameters, type, tag, flags
      85,   77,   24,   24, 0x08,
     110,   24,   24,   24, 0x08,
     119,   24,   24,   24, 0x08,
     140,   24,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GUI__RasterConfigurator[] = {
    "GUI::RasterConfigurator\0\0,\0"
    "rasterConfigured(std::string,RasterConfiguration)\0"
    "checked\0transparentToggled(bool)\0"
    "accept()\0splitColorInterval()\0"
    "removeColorInterval()\0"
};

void GUI::RasterConfigurator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RasterConfigurator *_t = static_cast<RasterConfigurator *>(_o);
        switch (_id) {
        case 0: _t->rasterConfigured((*reinterpret_cast< const std::string(*)>(_a[1])),(*reinterpret_cast< const RasterConfiguration(*)>(_a[2]))); break;
        case 1: _t->transparentToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->accept(); break;
        case 3: _t->splitColorInterval(); break;
        case 4: _t->removeColorInterval(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GUI::RasterConfigurator::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GUI::RasterConfigurator::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GUI__RasterConfigurator,
      qt_meta_data_GUI__RasterConfigurator, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI::RasterConfigurator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI::RasterConfigurator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI::RasterConfigurator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI__RasterConfigurator))
        return static_cast<void*>(const_cast< RasterConfigurator*>(this));
    return QDialog::qt_metacast(_clname);
}

int GUI::RasterConfigurator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void GUI::RasterConfigurator::rasterConfigured(const std::string & _t1, const RasterConfiguration & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
