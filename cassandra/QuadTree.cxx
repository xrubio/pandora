
#include "QuadTree.hxx"

#include <GL/gl.h>
#include <GL/glu.h>
#include <QListWidgetItem>
#include <ctime>
#include <cstdlib>

namespace GUI {

QuadTree::QuadTree(Engine::Point2D<int> center, Engine::Point2D<int> NW, Engine::Point2D<int> NE, Engine::Point2D<int> SE, Engine::Point2D<int> SW, Engine::Point2D<int> neighN, Engine::Point2D<int> neighS, Engine::Point2D<int> neighE,Engine::Point2D<int> neighW) : _childNW(0), _childNE(0), _childSE(0), _childSW(0)
{
    _NW = NW;
    _NE = NE;
    _SE = SE;
    _SW = SW;
    _center = center;
    _neighN = neighN;
    _neighS = neighS;
    _neighE = neighE;
    _neighW = neighW;
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

bool QuadTree::PolygonInFrustum(Engine::Point3D<int> a, Engine::Point3D<int> b, Engine::Point3D<int> c )
{
   int f, p;

   for( f = 0; f < 6; f++ )
   {
         p = 0;
         if( frustum[f][0] * a._x + frustum[f][1] * a._y + frustum[f][2] * a._z + frustum[f][3] < 0 ) p++;
         if( frustum[f][0] * b._x + frustum[f][1] * b._y + frustum[f][2] * b._z + frustum[f][3] < 0 ) p++;
         if( frustum[f][0] * c._x + frustum[f][1] * c._y + frustum[f][2] * c._z + frustum[f][3] < 0 ) p++;

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
            frustum[i][j] = frust[i][j];
        }
    }
}

void QuadTree::initializeChilds( int size)
{
    int pot2 = powf(2,ceil(log2(size)));
    int prof = pot2/2;
    prof = prof / 2;

    _childNW = initializeChild(1, Engine::Point2D<int>( _center._x-prof,_center._y-prof),prof,size);
    _childNE = initializeChild(1, Engine::Point2D<int>( _center._x+prof,_center._y-prof),prof,size);
    _childSE = initializeChild(1, Engine::Point2D<int>( _center._x+prof,_center._y+prof),prof,size);
    _childSW = initializeChild(1, Engine::Point2D<int>( _center._x-prof,_center._y+prof),prof,size);
}

QuadTree* QuadTree::initializeChild(int depth, Engine::Point2D<int> center, int prof, int size)
{

	if(prof<1)
	{
		return 0;
	}
	for(int i=0; i<depth; i++)
	{
		std::cout << "\t";
	}
	int outOfBounds_x = center._x+prof;
	int outOfBounds_y = center._y+prof;
	int pot2 = powf(2,ceil(log2(size)));
	if(outOfBounds_x >= pot2)
	{
		outOfBounds_x = pot2-1;
	}
	if(outOfBounds_y >= pot2)
	{
		outOfBounds_y = pot2-1;
	}

	QuadTree *child = 0;
	if (outOfBounds_x > size || outOfBounds_y > size || center._x-prof > size || center._y-prof > size)
	{

	}
	else
	{
		if(outOfBounds_x == size)
		{
			outOfBounds_x = size-1;
		}
		if(outOfBounds_y == size)
		{
			outOfBounds_y = size-1;
		}
	}
	Engine::Point2D<int> NW(center._x-prof,center._y-prof);
	Engine::Point2D<int> NE(outOfBounds_x,center._y-prof);
	Engine::Point2D<int> SE(outOfBounds_x,outOfBounds_y);
	Engine::Point2D<int> SW(center._x-prof,outOfBounds_y);

	Engine::Point2D<int> neighN(center._x,center._y-prof);
	Engine::Point2D<int> neighS(center._x,outOfBounds_y);
	Engine::Point2D<int> neighE(outOfBounds_x,center._y);
	Engine::Point2D<int> neighW(center._x-prof,center._y);

	child = new QuadTree(center,NW,NE,SE,SW,neighN,neighS,neighE,neighW);
	prof=prof/2;

	child->_childNW = initializeChild(depth+1, Engine::Point2D<int>(center._x-prof,center._y-prof),prof,size);
	child->_childNE = initializeChild(depth+1, Engine::Point2D<int>(center._x+prof,center._y-prof),prof,size);
	child->_childSE = initializeChild(depth+1, Engine::Point2D<int>(center._x+prof,center._y+prof),prof,size);
	child->_childSW = initializeChild(depth+1, Engine::Point2D<int>(center._x-prof,center._y+prof),prof,size);
	return child;
}

void QuadTree::paintTriangle( const Engine::Point2D<int> & point1, const Engine::Point2D<int> & point2, int size, const Engine::StaticRaster & DEMRaster, float exaggeration, const Engine::StaticRaster & colorRaster, ColorSelector & colorSelector, int offset, bool randomColor )
{
	// size check
	if(point1._x>=size || point1._y>=size)
	{
		return;
	}
	if(point2._x>=size || point2._y>=size)
	{
		return;
	}

	// frustrum check
	Engine::Point3D<int> point3D1(point1._x, point1._y, DEMRaster.getValue(point1)*exaggeration);
	Engine::Point3D<int> point3D2(point2._x, point2._y, DEMRaster.getValue(point2)*exaggeration);
	Engine::Point3D<int> point3D3(_center._x, _center._y, DEMRaster.getValue(_center)*exaggeration);
	if(!PolygonInFrustum(point3D1, point3D2, point3D3))
	{
		return;
	}
//	std::cout << "\tpainting triangle between: " << point1 << " -- " << point2 << " and centre: " << _center << std::endl;
	glBegin(GL_TRIANGLES);
	
	float sizeF = (float)size;

	setCellColor(colorRaster, colorSelector, _center, randomColor);
	glTexCoord2f((float)(_center._x)/sizeF, (float)(_center._y)/sizeF);
	glVertex3f(_center._x,-_center._y,offset+((DEMRaster.getValue(_center)*exaggeration)));

	//setCellColor(colorRaster, colorSelector, point1, randomColor);
	glTexCoord2f((float)(point1._x)/sizeF, (float)(point1._y)/sizeF);
	glVertex3f(point1._x,-point1._y,offset+((DEMRaster.getValue(point1)*exaggeration)));

	//setCellColor(colorRaster, colorSelector, point2, randomColor);
	glTexCoord2f((float)(point2._x)/sizeF, (float)(point2._y)/sizeF);
	glVertex3f(point2._x,-point2._y,offset+((DEMRaster.getValue(point2)*exaggeration)));

	glEnd();
}

void QuadTree::update(double dist, int prof, ColorSelector &colorSelector, Engine::StaticRaster & colorRaster, int size, Engine::StaticRaster & DEMRaster, int LOD, int offset, float exaggeration, float cellResolution, bool randomColor)
{
	float camara1[16];
	glGetFloatv(GL_MODELVIEW_MATRIX,camara1);

		
	Engine::Point3D<float> camara(0,0,0);
	Engine::Point3D<float> punt(0.0f,0.0f,0.0f);

	if(_center._x < size && _center._y < size )
	{
		punt._x = camara1[0] * _center._x + -camara1[4] * _center._y + camara1[8] * DEMRaster.getValue(_center)*exaggeration +camara1[12];
		punt._y = camara1[1] * _center._x + -camara1[5] * _center._y + camara1[9] * DEMRaster.getValue(_center)*exaggeration +camara1[13];
		punt._z = camara1[2] * _center._x + -camara1[6] * _center._y + camara1[10]* DEMRaster.getValue(_center)*exaggeration +camara1[14];
	}

//	std::cout << "dist: " << dist << " punt: " << punt << " prof: " << prof << " lod: " << LOD << std::endl;
	if(abs(camara.distance(punt))/prof < LOD)
	{
		if(_childNW)
		{
			_childNW->update(dist,prof/2,colorSelector,colorRaster,size,DEMRaster,LOD,offset,exaggeration, cellResolution, randomColor);
		}
				
		if(_childNE)
		{
			_childNE->update(dist,prof/2,colorSelector,colorRaster,size,DEMRaster,LOD,offset,exaggeration, cellResolution, randomColor);
		}

		if(_childSE)
		{
			_childSE->update(dist,prof/2,colorSelector,colorRaster,size,DEMRaster,LOD,offset,exaggeration, cellResolution, randomColor);
		}

		if(_childSW)
		{
			_childSW->update(dist,prof/2,colorSelector,colorRaster,size,DEMRaster,LOD,offset,exaggeration, cellResolution, randomColor);
		}
	}
	else
	{
		if(_center._x < size && _center._y < size)
		{
			paintTriangle(_neighN, _NE, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset, randomColor);
			paintTriangle(_NE, _neighE, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset, randomColor);
			paintTriangle(_neighE, _SE, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset, randomColor);
			paintTriangle(_SE, _neighS, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset, randomColor);

			paintTriangle(_neighS, _SW, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset, randomColor);
			paintTriangle(_SW, _neighW, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset, randomColor);
			paintTriangle(_neighW, _NW, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset, randomColor);
			paintTriangle(_NW, _neighN, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset, randomColor);
        }
    }

}

void QuadTree::setCellColor( const Engine::StaticRaster & raster, ColorSelector & colorSelector, const Engine::Point2D<int> & cell, bool randomColor)
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

