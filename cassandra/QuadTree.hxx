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
    Quadtree(Engine::Point2D<int> center, Engine::Point2D<int> NW, Engine::Point2D<int> NE, Engine::Point2D<int> SE, Engine::Point2D<int> SW,Engine::Point2D<int> neighN, Engine::Point2D<int> neighS, Engine::Point2D<int> neighE,Engine::Point2D<int> neighW);

    //atributs
    Quadtree *childNW, *childNE, *childSE, *childSW; //child nodes
    Engine::Point2D<int> neighN, neighS, neighE, neighW; //neighbours
    Engine::Point2D<int> NW, NE, SE, SW; //outer points [x,y,z]
    Engine::Point2D<int> center; //center point [x,y,z]

    //Getters i setters
    Quadtree *getChildNodeNW();
    Quadtree *getChildNodeNE();
    Quadtree *getChildNodeSE();
    Quadtree *getChildNodeSW();

    Engine::Point2D<int> getOuterPointNW();
    Engine::Point2D<int> getOuterPointNE();
    Engine::Point2D<int> getOuterPointSE();
    Engine::Point2D<int> getOuterPointSW();

    Engine::Point2D<int> getCenter();

    float frustum[6][4];

    void setChildNodes(Quadtree *NW, Quadtree *NE, Quadtree *SE, Quadtree *SW);
    void setOuterPoints(Engine::Point2D<int> NW, Engine::Point2D<int> NE, Engine::Point2D<int> SE, Engine::Point2D<int> SW);
    void setCenter(Engine::Point2D<int> center);

    void initializeChilds(Engine::StaticRaster & DEMRaster);
    Quadtree* initializeChild(Engine::Point2D<int> center, int prof, Engine::StaticRaster &DEMRaster);

    void setCellColor( const QColor & color );
    int update(double anglecam, int prof, ColorSelector &colorSelector, Engine::StaticRaster &colorRaster, int size, const Engine::Point3D<float> scale, Engine::StaticRaster & DEMRaster, int LOD, int offset); //recorrer l'arbre i segons la distancia activar o desactivar nodes (childs) i afegirlos a points/polys
    bool PolygonInFrustum(Engine::Point3D<int> a, Engine::Point3D<int> b, Engine::Point3D<int> c );
    void setFrustum(float frust[6][4]);

};
}

#endif // QUADTREE_H
