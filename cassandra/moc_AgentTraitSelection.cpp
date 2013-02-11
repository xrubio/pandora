/****************************************************************************
** Meta object code from reading C++ file 'AgentTraitSelection.hxx'
**
** Created: Tue Feb 5 12:55:39 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "AgentTraitSelection.hxx"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AgentTraitSelection.hxx' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI__AgentTraitSelection[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      31,   26,   25,   25, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GUI__AgentTraitSelection[] = {
    "GUI::AgentTraitSelection\0\0item\0"
    "typeSelected(QListWidgetItem*)\0"
};

void GUI::AgentTraitSelection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AgentTraitSelection *_t = static_cast<AgentTraitSelection *>(_o);
        switch (_id) {
        case 0: _t->typeSelected((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GUI::AgentTraitSelection::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GUI::AgentTraitSelection::staticMetaObject = {
    { &QListWidget::staticMetaObject, qt_meta_stringdata_GUI__AgentTraitSelection,
      qt_meta_data_GUI__AgentTraitSelection, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI::AgentTraitSelection::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI::AgentTraitSelection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI::AgentTraitSelection::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI__AgentTraitSelection))
        return static_cast<void*>(const_cast< AgentTraitSelection*>(this));
    return QListWidget::qt_metacast(_clname);
}

int GUI::AgentTraitSelection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
