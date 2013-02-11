/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.hxx'
**
** Created: Tue Feb 5 12:55:37 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MainWindow.hxx"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.hxx' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI__MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      37,   17,   16,   16, 0x05,
      76,   62,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      95,   16,   16,   16, 0x08,
     108,   16,   16,   16, 0x08,
     122,   16,   16,   16, 0x08,
     136,   16,   16,   16, 0x08,
     152,   16,   16,   16, 0x08,
     171,   16,   16,   16, 0x08,
     178,   16,   16,   16, 0x08,
     190,   16,   16,   16, 0x08,
     205,   16,   16,   16, 0x08,
     216,   16,   16,   16, 0x08,
     228,   16,   16,   16, 0x08,
     246,  235,   16,   16, 0x08,
     269,   16,   16,   16, 0x08,
     287,  282,   16,   16, 0x08,
     327,  282,   16,   16, 0x08,
     368,   16,   16,   16, 0x08,
     383,   16,   16,   16, 0x08,
     399,   16,   16,   16, 0x08,
     417,   16,   16,   16, 0x08,
     436,  434,   16,   16, 0x08,
     484,  434,   16,   16, 0x08,
     534,   16,   16,   16, 0x08,
     572,  564,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GUI__MainWindow[] = {
    "GUI::MainWindow\0\0stepChangeToStepBox\0"
    "stepChangeToStepBox(int)\0newViewedStep\0"
    "newViewedStep(int)\0newProject()\0"
    "loadProject()\0saveProject()\0saveProjectAs()\0"
    "selectSimulation()\0quit()\0firstStep()\0"
    "previousStep()\0nextStep()\0finalStep()\0"
    "play()\0viewedStep\0viewedStepChanged(int)\0"
    "updatePlay()\0item\0"
    "openAgentConfigurator(QListWidgetItem*)\0"
    "openRasterConfigurator(QListWidgetItem*)\0"
    "show3DWindow()\0show3DOptions()\0"
    "show3DWindowVTK()\0show3DWindowVL()\0,\0"
    "agentConfigured(std::string,AgentConfiguration)\0"
    "rasterConfigured(std::string,RasterConfiguration)\0"
    "configured3D(Configuration3D)\0correct\0"
    "loadSimulationFinished(bool)\0"
};

void GUI::MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->stepChangeToStepBox((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->newViewedStep((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->newProject(); break;
        case 3: _t->loadProject(); break;
        case 4: _t->saveProject(); break;
        case 5: _t->saveProjectAs(); break;
        case 6: _t->selectSimulation(); break;
        case 7: _t->quit(); break;
        case 8: _t->firstStep(); break;
        case 9: _t->previousStep(); break;
        case 10: _t->nextStep(); break;
        case 11: _t->finalStep(); break;
        case 12: _t->play(); break;
        case 13: _t->viewedStepChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->updatePlay(); break;
        case 15: _t->openAgentConfigurator((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 16: _t->openRasterConfigurator((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 17: _t->show3DWindow(); break;
        case 18: _t->show3DOptions(); break;
        case 19: _t->show3DWindowVTK(); break;
        case 20: _t->show3DWindowVL(); break;
        case 21: _t->agentConfigured((*reinterpret_cast< const std::string(*)>(_a[1])),(*reinterpret_cast< const AgentConfiguration(*)>(_a[2]))); break;
        case 22: _t->rasterConfigured((*reinterpret_cast< const std::string(*)>(_a[1])),(*reinterpret_cast< const RasterConfiguration(*)>(_a[2]))); break;
        case 23: _t->configured3D((*reinterpret_cast< const Configuration3D(*)>(_a[1]))); break;
        case 24: _t->loadSimulationFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GUI::MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GUI::MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_GUI__MainWindow,
      qt_meta_data_GUI__MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI::MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI::MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI::MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI__MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int GUI::MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    }
    return _id;
}

// SIGNAL 0
void GUI::MainWindow::stepChangeToStepBox(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GUI::MainWindow::newViewedStep(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
