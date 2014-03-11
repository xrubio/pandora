
#include "QuadTree.hxx"

#include <GL/gl.h>
#include <GL/glu.h>
#include <QListWidgetItem>
#include <ctime>
#include <cstdlib>
#include <QColor>

#include <StaticRaster.hxx>
#include <RasterConfiguration.hxx>
#include <ColorSelector.hxx>

namespace GUI {

QuadTree::QuadTree( const Engine::Size<int> & size ) : _size(size), _childNW(0), _childNE(0), _childSE(0), _childSW(0)
{
	int pot2X = powf(2,ceil(log2(_size._width)));
	int pot2Y = powf(2,ceil(log2(_size._height)));
	_center = Engine::Point2D<int>(pot2X/2, pot2Y/2);

	_NW = Engine::Point2D<int>(0,0);
	_NE = Engine::Point2D<int>(pot2X-1, 0);
	_SE = Engine::Point2D<int>(pot2X-1, pot2Y-1);
	_SW = Engine::Point2D<int>(0,pot2Y-1);

	_neighN = Engine::Point2D<int>(pot2X/2,0);
	_neighS = Engine::Point2D<int>(pot2X/2,pot2Y-1);
	_neighE = Engine::Point2D<int>(pot2X-1,pot2Y/2);
	_neighW = Engine::Point2D<int>(0,pot2Y/2);
}

QuadTree::QuadTree( const Engine::Size<int> & size, const Engine::Point2D<int> & center, const Engine::Point2D<int> & prof ) : _size(size), _center(center), _childNW(0), _childNE(0), _childSE(0), _childSW(0)
{
	Engine::Point2D<int> outOfBounds;
	outOfBounds._x = std::min(_size._width-1, _center._x+prof._x);
	outOfBounds._y = std::min(_size._height-1, _center._y+prof._y);

	_NW = Engine::Point2D<int>(_center._x-prof._x,_center._y-prof._y);
	_NE = Engine::Point2D<int>(outOfBounds._x,_center._y-prof._y);
	_SE = Engine::Point2D<int>(outOfBounds._x,outOfBounds._y);
	_SW = Engine::Point2D<int>(_center._x-prof._x,outOfBounds._y);

	_neighN = Engine::Point2D<int>(_center._x,_center._y-prof._y);
	_neighS = Engine::Point2D<int>(_center._x,outOfBounds._y);
	_neighE = Engine::Point2D<int>(outOfBounds._x,_center._y);
	_neighW = Engine::Point2D<int>(_center._x-prof._x,_center._y);
}

QuadTree::~QuadTree()
{
	if(_childNW)
	{
		delete _childNW;
	}
	if(_childNE)
	{
		delete _childNE;
	}	
	if(_childSE)
	{
		delete _childSE;
	}
	if(_childSW)
	{
		delete _childSW;
	}
}

bool QuadTree::polygonInFrustum(Engine::Point3D<int> a, Engine::Point3D<int> b, Engine::Point3D<int> c )
{
   int f, p;

   for( f = 0; f < 6; f++ )
   {
         p = 0;
         if( _frustum[f][0] * a._x + _frustum[f][1] * a._y + _frustum[f][2] * a._z + _frustum[f][3] < 0 ) p++;
         if( _frustum[f][0] * b._x + _frustum[f][1] * b._y + _frustum[f][2] * b._z + _frustum[f][3] < 0 ) p++;
         if( _frustum[f][0] * c._x + _frustum[f][1] * c._y + _frustum[f][2] * c._z + _frustum[f][3] < 0 ) p++;

      if( p == 3)
         return true;
   }
   return true;
}

void QuadTree::setFrustum(float frust[6][4])
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            _frustum[i][j] = frust[i][j];
        }
    }
}

void QuadTree::initializeChilds()
{
    int pot2X = powf(2,ceil(log2(_size._width)));
    int pot2Y = powf(2,ceil(log2(_size._height)));

	Engine::Point2D<int> prof(pot2X/4, pot2Y/4);

    _childNW = initializeChild(1, Engine::Point2D<int>( _center._x-prof._x,_center._y-prof._y),prof);
    _childNE = initializeChild(1, Engine::Point2D<int>( _center._x+prof._x,_center._y-prof._y),prof);
    _childSE = initializeChild(1, Engine::Point2D<int>( _center._x+prof._x,_center._y+prof._y),prof);
    _childSW = initializeChild(1, Engine::Point2D<int>( _center._x-prof._x,_center._y+prof._y),prof);
}

QuadTree* QuadTree::initializeChild(int depth, Engine::Point2D<int> center, const Engine::Point2D<int> & previousProf)
{

	if(previousProf._x<1 || previousProf._y<1)
	{
		return 0;
	}

	QuadTree * child = new QuadTree(_size, center, previousProf); //,NW,NE,SE,SW,neighN,neighS,neighE,neighW);
	Engine::Point2D<int> prof(previousProf._x/2, previousProf._y/2);

	child->_childNW = initializeChild(depth+1, Engine::Point2D<int>(center._x-prof._x,center._y-prof._y),prof);
	child->_childNE = initializeChild(depth+1, Engine::Point2D<int>(center._x+prof._x,center._y-prof._y),prof);
	child->_childSE = initializeChild(depth+1, Engine::Point2D<int>(center._x+prof._x,center._y+prof._y),prof);
	child->_childSW = initializeChild(depth+1, Engine::Point2D<int>(center._x-prof._x,center._y+prof._y),prof);
	return child;
}

void QuadTree::paintTriangle( const Engine::Point2D<int> & point1, const Engine::Point2D<int> & point2, const RasterConfiguration & rasterConfig, const Engine::StaticRaster & DEMRaster, const Engine::StaticRaster & colorRaster, bool randomColor )
{
	// size check
	if(point1._x>=_size._width || point1._y>=_size._height)
	{
		return;
	}
	if(point2._x>=_size._width || point2._y>=_size._height)
	{
		return;
	}

	// frustrum check
	Engine::Point3D<int> point3D1(point1._x*rasterConfig.getCellResolution(), point1._y*rasterConfig.getCellResolution(), DEMRaster.getValue(point1)*rasterConfig.getElevationExaggeration());
	Engine::Point3D<int> point3D2(point2._x*rasterConfig.getCellResolution(), point2._y*rasterConfig.getCellResolution(), DEMRaster.getValue(point2)*rasterConfig.getElevationExaggeration());
	Engine::Point3D<int> point3D3(_center._x*rasterConfig.getCellResolution(), _center._y*rasterConfig.getCellResolution(), DEMRaster.getValue(_center)*rasterConfig.getElevationExaggeration());
	if(!polygonInFrustum(point3D1, point3D2, point3D3))
	{
		return;
	}
//	std::cout << "\tpainting triangle between: " << point1 << " -- " << point2 << " and centre: " << _center << std::endl;
	glBegin(GL_TRIANGLES);
	
	Engine::Point2D<float> sizeF;
	sizeF._x = _size._width;
	sizeF._y = _size._height;

	setCellColor(colorRaster, rasterConfig.getColorRamp(), _center, randomColor);
	glTexCoord2f((float)(_center._x)/sizeF._x, (float)(_center._y)/sizeF._y);
	glVertex3f(_center._x*rasterConfig.getCellResolution(),-_center._y*rasterConfig.getCellResolution(),((DEMRaster.getValue(_center)*rasterConfig.getElevationExaggeration())));

	//setCellColor(colorRaster, colorSelector, point2, randomColor);
	glTexCoord2f((float)(point2._x)/sizeF._x, (float)(point2._y)/sizeF._y);
	glVertex3f(point2._x*rasterConfig.getCellResolution(),-point2._y*rasterConfig.getCellResolution(),((DEMRaster.getValue(point2)*rasterConfig.getElevationExaggeration())));

	//setCellColor(colorRaster, colorSelector, point1, randomColor);
	glTexCoord2f((float)(point1._x)/sizeF._x, (float)(point1._y)/sizeF._y);
	glVertex3f(point1._x*rasterConfig.getCellResolution(),-point1._y*rasterConfig.getCellResolution(),((DEMRaster.getValue(point1)*rasterConfig.getElevationExaggeration())));

	glEnd();
}

void QuadTree::update( const Engine::Point2D<int> & prof, RasterConfiguration & rasterConfig, Engine::StaticRaster & colorRaster, Engine::StaticRaster & DEMRaster, bool randomColor)
{
	float camara1[16];
	glGetFloatv(GL_MODELVIEW_MATRIX,camara1);

		
	Engine::Point3D<float> camara(0,0,0);
	Engine::Point3D<float> punt(0.0f,0.0f,0.0f);

	if(_center._x < _size._width && _center._y < _size._height )
	{
		punt._x = _size._width*rasterConfig.getOffset()._x + camara1[0] * _center._x + -camara1[4] * _center._y + camara1[8] * DEMRaster.getValue(_center)*rasterConfig.getElevationExaggeration()+camara1[12];
		punt._y = _size._height*rasterConfig.getOffset()._y + camara1[1] * _center._x + -camara1[5] * _center._y + camara1[9] * DEMRaster.getValue(_center)*rasterConfig.getElevationExaggeration()+camara1[13];
		// size.z??
		punt._z = _size._width*rasterConfig.getOffset()._z + camara1[2] * _center._x + -camara1[6] * _center._y + camara1[10]* DEMRaster.getValue(_center)*rasterConfig.getElevationExaggeration()+camara1[14];
	}

	if(abs(camara.distance(punt))/prof._x < rasterConfig.getLOD() || abs(camara.distance(punt))/prof._y< rasterConfig.getLOD() )
	{
		if(_childNW)
		{
			_childNW->update(prof/2,rasterConfig, colorRaster,DEMRaster,randomColor);
		}
				
		if(_childNE)
		{
			_childNE->update(prof/2,rasterConfig, colorRaster,DEMRaster,randomColor);
		}

		if(_childSE)
		{
			_childSE->update(prof/2,rasterConfig, colorRaster,DEMRaster,randomColor);
		}

		if(_childSW)
		{
			_childSW->update(prof/2,rasterConfig, colorRaster,DEMRaster,randomColor);
		}
	}
	else
	{
		if(_center._x < _size._width && _center._y < _size._width)
		{
			Engine::Point3D<float> offset = rasterConfig.getOffset();
			offset._x *= _size._width;
			offset._y *= _size._height;

			glPushMatrix();
			glTranslatef(offset._x, offset._y, offset._z);
			paintTriangle(_neighN, _NE, rasterConfig, DEMRaster, colorRaster, randomColor);
			paintTriangle(_NE, _neighE, rasterConfig, DEMRaster, colorRaster, randomColor);
			paintTriangle(_neighE, _SE, rasterConfig, DEMRaster, colorRaster, randomColor);			
			paintTriangle(_SE, _neighS, rasterConfig, DEMRaster, colorRaster, randomColor);

			paintTriangle(_neighS, _SW, rasterConfig, DEMRaster, colorRaster, randomColor);
			paintTriangle(_SW, _neighW, rasterConfig, DEMRaster, colorRaster, randomColor);
			paintTriangle(_neighW, _NW, rasterConfig, DEMRaster, colorRaster, randomColor);
			paintTriangle(_NW, _neighN, rasterConfig, DEMRaster, colorRaster, randomColor);
			glPopMatrix();
        }
    }

}

void QuadTree::setCellColor( const Engine::StaticRaster & raster, const ColorSelector & colorSelector, const Engine::Point2D<int> & cell, bool randomColor)
{
	QColor color;
	if(raster.hasColorTable())
	{
		Engine::ColorEntry entry = raster.getColorEntry(raster.getValue(cell));
		color = QColor(entry._r, entry._g, entry._b, entry._alpha);
	}
	else
	{
		color = colorSelector.getColor(raster.getValue(cell));
	}

	if(randomColor)
	{
		color = QColor(rand()%255, rand()%255, rand()%255);
	}
    
	float ambientColor[3];
	float diffuseColor[3];
	float specularColor[3];

//	std::cout << "painting cell: " << cell << " with color: " << color.name().toStdString() << std::endl;

	ambientColor[0] = color.redF();
	ambientColor[1] = color.greenF();
	ambientColor[2] = color.blueF();

	diffuseColor[0] = color.redF();
	diffuseColor[1] = color.greenF();
	diffuseColor[2] = color.blueF();

	specularColor[0] = specularColor[1] = specularColor[2] = 1.0f;
	glMaterialfv( GL_FRONT, GL_AMBIENT, ambientColor);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuseColor);
	glMaterialfv( GL_FRONT, GL_SPECULAR, specularColor);
}

} // namespace GUI

