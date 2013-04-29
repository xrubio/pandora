#ifndef QUADTREE_H
#define QUADTREE_H

#include <iostream>
#include <Point3D.hxx>
#include <SimulationRecord.hxx>
#include <QColor>
#include <ColorSelector.hxx>

using namespace std;


namespace GUI {
class Model3D;
class AgentConfiguration;
class RasterConfiguration;
class Configuration3D;

class Quadtree
{
public:
    Quadtree(Engine::Point3D<int> center, Engine::Point3D<int> NW, Engine::Point3D<int> NE, Engine::Point3D<int> SE, Engine::Point3D<int> SW,Engine::Point3D<int> neighN, Engine::Point3D<int> neighS, Engine::Point3D<int> neighE,Engine::Point3D<int> neighW);

    //atributs
    Quadtree *childNW, *childNE, *childSE, *childSW; //child nodes
    Engine::Point3D<int> neighN, neighS, neighE, neighW; //neighbours
    Engine::Point3D<int> NW, NE, SE, SW; //outer points [x,y,z]
    Engine::Point3D<int> center; //center point [x,y,z]

    //Getters i setters
    Quadtree *getChildNodeNW();
    Quadtree *getChildNodeNE();
    Quadtree *getChildNodeSE();
    Quadtree *getChildNodeSW();

    Engine::Point3D<int> getOuterPointNW();
    Engine::Point3D<int> getOuterPointNE();
    Engine::Point3D<int> getOuterPointSE();
    Engine::Point3D<int> getOuterPointSW();

    Engine::Point3D<int> getCenter();

    float frustum[6][4];

    void setChildNodes(Quadtree *NW, Quadtree *NE, Quadtree *SE, Quadtree *SW);
    void setOuterPoints(Engine::Point3D<int> NW, Engine::Point3D<int> NE, Engine::Point3D<int> SE, Engine::Point3D<int> SW);
    void setCenter(Engine::Point3D<int> center);

    void initializeChilds(Engine::StaticRaster & DEMRaster);
    Quadtree* initializeChild(Engine::Point3D<int> center, int prof, Engine::StaticRaster &DEMRaster);

    void setCellColor( const QColor & color );
    int update(double anglecam, int prof, ColorSelector &colorSelector, Engine::StaticRaster &colorRaster, int size, const Engine::Point3D<float> scale, Engine::StaticRaster & DEMRaster, int LOD); //recorrer l'arbre i segons la distancia activar o desactivar nodes (childs) i afegirlos a points/polys
    bool PolygonInFrustum(Engine::Point3D<int> a, Engine::Point3D<int> b, Engine::Point3D<int> c );
    void setFrustum(float frust[6][4]);

};
}

#endif // QUADTREE_H
