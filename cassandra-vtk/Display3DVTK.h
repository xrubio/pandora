#ifndef DISPLAY3DVTK_H
#define DISPLAY3DVTK_H

#include <QVTKWidget.h>
#include <Material.hxx>
#include <Point3D.hxx>
#include <map>

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

class Display3DVTK : public QVTKWidget
{

    Q_OBJECT

    typedef std::map<std::string, Model3D * > Models3DMap;
    typedef std::map<std::string, std::string > FileNamesMap;
    typedef std::map<std::string, AgentConfiguration *> AgentsConfigurationMap;
    typedef std::map<std::string, RasterConfiguration *> RastersConfigurationMap;

public:
    Display3DVTK(QVTKWidget *parent);
    virtual ~Display3DVTK();
    void setSimulationRecord( Engine::SimulationRecord * simulationRecord );
    void paintLandscape();


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

#endif // DISPLAY3DVTK_H
