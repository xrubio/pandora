#ifndef DISPLAYVTK_H
#define DISPLAYVTK_H

#include <QWidget>
#include <map>
#include <Material.hxx>

#include <Point3D.hxx>
#include "point.h"

namespace Ui {
class DisplayVTK;
}

class DisplayVTK : public QWidget
{
    Q_OBJECT
    
public:
    explicit DisplayVTK(QWidget *parent = 0);
    ~DisplayVTK();
    
protected:
    void focus();
    void initializeGL();
    double dist, anterior, posterior, radi, angleX, angleY, angleZ, anglecam, ra;
    Point VRP; //Punt on mira l'observador
    typedef  enum {NONE, ROTATE, ZOOM, PAN, MOV} InteractiveAction;
    InteractiveAction DoingInteractive;
    int   xClick, yClick;


private:
    Ui::DisplayVTK *ui;

private:
    void normalizeAngle(int &angle);
    Engine::SimulationRecord * _simulationRecord;
    int _viewedStep;
    Models3DMap _models;
    // we need this map in order to check if models are new or already loadd (from registerModels)
    FileNamesMap _paths;
    Material _landscapeMaterial;

    float _zoom;
    QPoint _position;
    QPoint _targetPosition;
    QPoint _lastPos;
    // TODO improve mouse navigation. At the moment this attribute is not used
    QPoint _lastTargetPos;
    QPoint _rotation;
    int _rotationZ;

    Engine::Point3D<float> _cellScale;

    // order of rasters
    std::list<std::string> _orderedRasters;

    const Configuration3D & _config3D;

};

#endif // DISPLAYVTK_H
