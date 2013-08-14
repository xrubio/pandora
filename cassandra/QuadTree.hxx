#ifndef __QuadTree_hxx__
#define __QuadTree_hxx__

#include <iostream>
#include <Point3D.hxx>
#include <SimulationRecord.hxx>
#include <QColor>
#include <ColorSelector.hxx>

namespace GUI
{

class QuadTree
{  
	// child nodes
    QuadTree * _childNW;
	QuadTree * _childNE;
	QuadTree * _childSE;
	QuadTree * _childSW;

	//neighbours
    Engine::Point2D<int> _neighN;
    Engine::Point2D<int> _neighS;
    Engine::Point2D<int> _neighE;
    Engine::Point2D<int> _neighW;

	// outer points [x,y,z]
	Engine::Point2D<int> _NW;
	Engine::Point2D<int> _NE;
	Engine::Point2D<int> _SE;
	Engine::Point2D<int> _SW;

	// center points [x,y,z]
	Engine::Point2D<int> _center;

	void paintTriangle( const Engine::Point2D<int> & point1, const Engine::Point2D<int> & point2, int size, const Engine::StaticRaster & DEMRaster, float exaggeration, const Engine::StaticRaster & colorRaster, ColorSelector & colorSelector, int offset, bool randomColor);
public:
    QuadTree(Engine::Point2D<int> center, Engine::Point2D<int> NW, Engine::Point2D<int> NE, Engine::Point2D<int> SE, Engine::Point2D<int> SW,Engine::Point2D<int> neighN, Engine::Point2D<int> neighS, Engine::Point2D<int> neighE,Engine::Point2D<int> neighW);
	virtual ~QuadTree();

    float frustum[6][4];

    void initializeChilds(int size);
    QuadTree* initializeChild(int depth, Engine::Point2D<int> center, int prof, int size);

    void setCellColor(const Engine::StaticRaster & raster, ColorSelector & colorSelector, const Engine::Point2D<int> & cell, bool randomColor );
	void update(double anglecam, int prof, ColorSelector &colorSelector, Engine::StaticRaster &colorRaster, int size, Engine::StaticRaster & DEMRaster, int LOD, int offset, float exaggeration, float cellResolution, bool randomColor); //recorrer l'arbre i segons la distancia activar o desactivar nodes (childs) i afegirlos a points/polys
    bool PolygonInFrustum(Engine::Point3D<int> a, Engine::Point3D<int> b, Engine::Point3D<int> c );
    void setFrustum(float frust[6][4]);
	
};

} // namespace GUI

#endif // __QuadTree_hxx__

