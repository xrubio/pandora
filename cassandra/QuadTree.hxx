#ifndef __QuadTree_hxx__
#define __QuadTree_hxx__

#include <Point2D.hxx>
#include <Point3D.hxx>
#include <Size.hxx>

namespace Engine
{
	class StaticRaster;
}

namespace GUI
{

class RasterConfiguration;
class ColorSelector;

class QuadTree
{  
	Engine::Size <int> _size;	
	
	// center points [x,y,z]
	Engine::Point2D<int> _center;

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


	void paintTriangle( const Engine::Point2D<int> & point1, const Engine::Point2D<int> & point2, const RasterConfiguration & rasterConfig, const Engine::StaticRaster & DEMRaster, const Engine::StaticRaster & colorRaster, bool randomColor);

public:
	// initial QuadTree 
	QuadTree( const Engine::Size<int> & size);
	// child QuadTree
	QuadTree( const Engine::Size<int> & size, const Engine::Point2D<int> & center, const Engine::Point2D<int> & prof);
	virtual ~QuadTree();

    float _frustum[6][4];

    void initializeChilds();
    QuadTree* initializeChild(int depth, Engine::Point2D<int> center, const Engine::Point2D<int> & previousProf);

    void setCellColor(const Engine::StaticRaster & raster, const ColorSelector & colorSelector, const Engine::Point2D<int> & cell, bool randomColor );
	//recorrer l'arbre i segons la distancia activar o desactivar nodes (childs) i afegirlos a points/polys
	void update( const Engine::Point2D<int> & prof, RasterConfiguration & rasterConfig, Engine::StaticRaster &colorRaster, Engine::StaticRaster & DEMRaster, bool randomColor);
    bool polygonInFrustum(Engine::Point3D<int> a, Engine::Point3D<int> b, Engine::Point3D<int> c );
    void setFrustum(float frust[6][4]);
	
};

} // namespace GUI

#endif // __QuadTree_hxx__

