/****************************************************************************
** Meta object code from reading C++ file 'AgentConfigurator.hxx'
**
** Created: Tue Feb 5 12:55:43 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "AgentConfigurator.hxx"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AgentConfigurator.hxx' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI__AgentConfigurator[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      26,   24,   23,   23, 0x05,

 // slots: signature, parameters, type, tag, flags
      74,   23,   23,   23, 0x08,
      88,   23,   23,   23, 0x08,
     101,   23,   23,   23, 0x08,
     131,  117,   23,   23, 0x08,
     153,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GUI__AgentConfigurator[] = {
    "GUI::AgentConfigurator\0\0,\0"
    "agentConfigured(std::string,AgentConfiguration)\0"
    "selectColor()\0selectIcon()\0selectModel3D()\0"
    "selectedColor\0colorSelected(QColor)\0"
    "accept()\0"
};

void GUI::AgentConfigurator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AgentConfigurator *_t = static_cast<AgentConfigurator *>(_o);
        switch (_id) {
        case 0: _t->agentConfigured((*reinterpret_cast< const std::string(*)>(_a[1])),(*reinterpret_cast< const AgentConfiguration(*)>(_a[2]))); break;
        case 1: _t->selectColor(); break;
        case 2: _t->selectIcon(); break;
        case 3: _t->selectModel3D(); break;
        case 4: _t->colorSelected((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 5: _t->accept(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GUI::AgentConfigurator::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GUI::AgentConfigurator::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GUI__AgentConfigurator,
      qt_meta_data_GUI__AgentConfigurator, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI::AgentConfigurator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI::AgentConfigurator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI::AgentConfigurator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI__AgentConfigurator))
        return static_cast<void*>(const_cast< AgentConfigurator*>(this));
    return QDialog::qt_metacast(_clname);
}

int GUI::AgentConfigurator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void GUI::AgentConfigurator::agentConfigured(const std::string & _t1, const AgentConfiguration & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
