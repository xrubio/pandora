
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

/*
QuadTree*  QuadTree::getChildNodeNW()
{
    return _childNW;
}

QuadTree*  QuadTree::getChildNodeNE()
{
    return this->childNE;

}
QuadTree*  QuadTree::getChildNodeSE()
{
    return this->childSE;

}
QuadTree*  QuadTree::getChildNodeSW()
{
    return this->childSW;
}

Engine::Point2D<int>  QuadTree::getOuterPointNW()
{
    return this->NW;
}

Engine::Point2D<int>  QuadTree::getOuterPointNE()
{
    return this->NE;
}

Engine::Point2D<int>  QuadTree::getOuterPointSE()
{
    return this->SE;
}

Engine::Point2D<int>  QuadTree::getOuterPointSW()
{
    return this->SW;
}

void QuadTree::setChildNodes(QuadTree *NW, QuadTree *NE, QuadTree *SE, QuadTree *SW)
{
    this->childNW = NW;
    this->childNE = NE;
    this->childSE = SE;
    this->childSW = SW;
}


void QuadTree::setOuterPoints(Engine::Point2D<int> NW, Engine::Point2D<int> NE, Engine::Point2D<int> SE, Engine::Point2D<int> SW)
{
    this->NW = NW;
    this->NE = NE;
    this->SE = SE;
    this->SW = SW;
}


void QuadTree::setCenter(Engine::Point2D<int>  center)
{
    this->center = center;

}
*/


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

//NETEJAR DEMRaster (NO NECESARI, es carreguen els valors de Z quan es pinta), guardar nomes 2 punts a larbre (X,Y)
void QuadTree::initializeChilds( int size)
{
    int pot2 = powf(2,ceil(log2(size)));
    int prof = pot2/2;
    /*this->childNW = initializeChild(Engine::Point3D<int>(this->center._x/2,this->center._y/2,DEMRaster.getValue(Engine::Point2D<int>(this->center._x/2,this->center._y/2))),prof/2,DEMRaster);
    this->childNE = initializeChild(Engine::Point3D<int>(this->center._x+(this->center._x/2),this->center._y/2,DEMRaster.getValue(Engine::Point2D<int>(this->center._x+(this->center._x/2),this->center._y/2))),prof/2,DEMRaster);
    this->childSE = initializeChild(Engine::Point3D<int>(this->center._x+(this->center._x/2),this->center._y+(this->center._y/2),DEMRaster.getValue(Engine::Point2D<int>(this->center._x+(this->center._x/2),this->center._y+(this->center._y/2)))),prof/2,DEMRaster);
    this->childSW = initializeChild(Engine::Point3D<int>(this->center._x/2,this->center._y+(this->center._y/2),DEMRaster.getValue(Engine::Point2D<int>(this->center._x/2,this->center._y+(this->center._y/2)))),prof/2,DEMRaster);
    */
    prof = prof / 2;

    _childNW = initializeChild(Engine::Point2D<int>(_center._x-prof,_center._y-prof),prof,size);
    _childNE = initializeChild(Engine::Point2D<int>(_center._x+prof,_center._y-prof),prof,size);
    _childSE = initializeChild(Engine::Point2D<int>(_center._x+prof,_center._y+prof),prof,size);
    _childSW = initializeChild(Engine::Point2D<int>(_center._x-prof,_center._y+prof),prof,size);

}

QuadTree* QuadTree::initializeChild(Engine::Point2D<int> center, int prof, int size)
{
    if (prof >= 1)
    {
        //Engine::Point2D<int> NW, NE, SE, SW, neighN, neighS, neighE, neighW;
        QuadTree *child;

        Engine::Point2D<int> NW(center._x-prof,center._y-prof);
        Engine::Point2D<int> NE(center._x+prof,center._y-prof);
        Engine::Point2D<int> SE(center._x+prof,center._y+prof);
        Engine::Point2D<int> SW(center._x-prof,center._y+prof);

        Engine::Point2D<int> neighN(center._x,center._y-prof);
        Engine::Point2D<int> neighS(center._x,center._y+prof);
        Engine::Point2D<int> neighE(center._x+prof,center._y);
        Engine::Point2D<int> neighW(center._x-prof,center._y);

        child = new QuadTree(center,NW,NE,SE,SW,neighN,neighS,neighE,neighW);
        prof=prof/2;

        child->_childNW = initializeChild(Engine::Point2D<int>(center._x-prof,center._y-prof),prof,size);
        child->_childNE = initializeChild(Engine::Point2D<int>(center._x+prof,center._y-prof),prof,size);
        child->_childSE = initializeChild(Engine::Point2D<int>(center._x+prof,center._y+prof),prof,size);
        child->_childSW = initializeChild(Engine::Point2D<int>(center._x-prof,center._y+prof),prof,size);

        return child;
    }
    else return 0;

}

void QuadTree::paintTriangle( const Engine::Point2D<int> & point1, const Engine::Point2D<int> & point2, int size, const Engine::StaticRaster & DEMRaster, float exaggeration, const Engine::StaticRaster & colorRaster, ColorSelector & colorSelector, int offset )
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

	setCellColor(colorRaster, colorSelector, point1);
	glTexCoord2f((float)(point1._x)/sizeF, (float)(point1._y)/sizeF);
	glVertex3f(point1._x,-point1._y,offset+((DEMRaster.getValue(point1)*exaggeration)));

	setCellColor(colorRaster, colorSelector, _center);
	glTexCoord2f((float)(_center._x)/sizeF, (float)(_center._y)/sizeF);
	glVertex3f(_center._x,-_center._y,offset+((DEMRaster.getValue(_center)*exaggeration)));

	setCellColor(colorRaster, colorSelector, point2);
	glTexCoord2f((float)(point2._x)/sizeF, (float)(point2._y)/sizeF);
	glVertex3f(point2._x,-point2._y,offset+((DEMRaster.getValue(point2)*exaggeration)));

	glEnd();
}

int QuadTree::update(double dist, int prof, ColorSelector &colorSelector, Engine::StaticRaster & colorRaster, int size, Engine::StaticRaster & DEMRaster, int LOD, int offset, float exaggeration, float cellResolution)
{
//	float totalSize = size * cellResolution;
	//si la distancia(camara-punt(anglecam))/profunditat_arbre < Constant -> habilita punt
	//cout << "Relacio " << (abs(dist-this->center._z))/prof << endl;
	//MIRAR POSICIO CAMARA, NO DIST!!!

	float camara1[16];
	glGetFloatv(GL_MODELVIEW_MATRIX,camara1);
	//cout << camara1[12] << " " << camara1[13] << " " <<camara1[14] << endl;

	/*Engine::Point3D<float> camara(camara1[0]*camara1[12]+camara1[4]*camara1[13]+camara1[8]*camara1[14],
	camara1[1]*camara1[12]+camara1[5]*camara1[13]+camara1[9]*camara1[14],
	camara1[2]*camara1[12]+camara1[6]*camara1[13]+camara1[10]*camara1[14]);*/
		
	Engine::Point3D<float> camara(0,0,0);
	Engine::Point3D<float> punt(0.0f,0.0f,0.0f);

	if(_center._x < size && _center._y < size )
	{
		punt._x = camara1[0] * _center._x + -camara1[4] * _center._y + camara1[8] * DEMRaster.getValue(_center)*exaggeration +camara1[12];
		punt._y = camara1[1] * _center._x + -camara1[5] * _center._y + camara1[9] * DEMRaster.getValue(_center)*exaggeration +camara1[13];
		punt._z = camara1[2] * _center._x + -camara1[6] * _center._y + camara1[10]* DEMRaster.getValue(_center)*exaggeration +camara1[14];
	}
	/*else {
	px = camara1[0] * center._x + -camara1[4] * center._y + camara1[8]  +camara1[12];
	py = camara1[1] * center._x + -camara1[5] * center._y + camara1[9]  +camara1[13];
	pz = camara1[2] * center._x + -camara1[6] * center._y + camara1[10] +camara1[14];
	}*/

	if(abs(camara.distance(punt))/prof < LOD && _childNW)
	{
		int profNW = prof;
		int aux;
		if(_childNW != 0)
		{
			aux = _childNW->update(dist,prof/2,colorSelector,colorRaster,size,DEMRaster,LOD,offset,exaggeration, cellResolution);
			if (aux < prof)
			{
				profNW = aux;
			}
		}
				
		int profNE = prof;
		if(_childNE != 0)
		{
			aux = _childNE->update(dist,prof/2,colorSelector,colorRaster,size,DEMRaster,LOD,offset,exaggeration, cellResolution);
			if(aux < prof)
			{
				profNE = aux;
			}
		}

		int profSE = prof;
		if(_childSE != 0)
		{
			aux = _childSE->update(dist,prof/2,colorSelector,colorRaster,size,DEMRaster,LOD,offset,exaggeration, cellResolution);
			if(aux < prof)
			{
				profSE = aux;
			}
		}

		int profSW = prof;
		if(_childSW != 0)
		{
			aux = _childSW->update(dist,prof/2,colorSelector,colorRaster,size,DEMRaster,LOD,offset,exaggeration, cellResolution);
			if(aux < prof)
			{
				profSW = aux;
			}
		}
	}
	else
	{
		if(_center._x < size && _center._y < size)
		{
			paintTriangle(_neighN, _NE, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset);
			paintTriangle(_NE, _neighE, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset);
			paintTriangle(_neighE, _SE, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset);
			paintTriangle(_SE, _neighS, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset);

			paintTriangle(_neighS, _SW, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset);
			paintTriangle(_SW, _neighW, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset);
			paintTriangle(_neighW, _NW, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset);
			paintTriangle(_NW, _neighN, size, DEMRaster, exaggeration, colorRaster, colorSelector, offset);
            return prof;
        }
    }

}

void QuadTree::setCellColor( const Engine::StaticRaster & raster, ColorSelector & colorSelector, const Engine::Point2D<int> & cell )
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

