#ifndef DISPLAY3DVL_H
#define DISPLAY3DVL_H

#include <QGLWidget>
#include <Material.hxx>
#include <Point3D.hxx>
#include <map>

#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/Time.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/TrackballManipulator.hpp>
#include <vlGraphics/UIEventListener.hpp>
#include <vlGraphics/OpenGLContext.hpp>
#include <vlGraphics/Applet.hpp>

namespace Engine
{
    class SimulationRecord;
}

namespace GUI
{
class Model3D;
class AgentConfiguration;
class RasterConfiguration;
class Configuration3D;

class Display3DVL :  public QGLWidget
{

    Q_OBJECT

    typedef std::map<std::string, Model3D * > Models3DMap;
    typedef std::map<std::string, std::string > FileNamesMap;
    typedef std::map<std::string, AgentConfiguration *> AgentsConfigurationMap;
    typedef std::map<std::string, RasterConfiguration *> RastersConfigurationMap;

public:
    Display3DVL(QWidget *parent);
    virtual ~Display3DVL();
    void paintlandscape();
    void setSimulationRecord( Engine::SimulationRecord * simulationRecord );

    virtual void initializeGL();
    virtual void updateGL();



private:
    Engine::SimulationRecord * _simulationRecord;
    int _viewedStep;
    Models3DMap _models;
    // we need this map in order to check if models are new or already loadd (from registerModels)
    FileNamesMap _paths;
    Material _landscapeMaterial;
    Engine::Point3D<float> _cellScale;
    // order of rasters
    std::list<std::string> _orderedRasters;
    const Configuration3D & _config3D;
};

}
#endif // DISPLAY3DVL_H
