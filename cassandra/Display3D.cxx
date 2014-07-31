/*
 * Copyright (c) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es
 *
 * This file is part of Cassandra.
 * Cassandra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Cassandra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <Display3D.hxx>

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QListWidgetItem>

#include <Exception.hxx>
#include <SimulationRecord.hxx>
#include <Model3D.hxx>
#include <Point2D.hxx>
#include <Size.hxx>
#include <QtGui>
#include <AgentConfiguration.hxx>
#include <RasterConfiguration.hxx>
#include <ColorSelector.hxx>
#include <ProjectConfiguration.hxx>
#include <algorithm>
#include <glut.h>
#include <QDebug>
#include <QTime>
#include <AgentRecord.hxx>
#include <QuadTree.hxx>

namespace GUI
{

//Inicialitzem els paràmetres al constructor per defecte i carreguem l'arxiu on hi ha tota la informació referent al ràster
//que posteriorment haurem de representar (x,y,z).
Display3D::Display3D(QWidget *parent ) : QGLWidget(parent), _simulationRecord(0), _viewedStep(0), _zoom(1.0f), _position(0,0), _lastPos(0,0), _rotation(0,0), _rotationZ(0), _cellScale(1.0f, 1.0f, 1.0f), _agentFocus(0), _randomColor(false)
{
}


Display3D::~Display3D()
{
	for(QuadTreeMap::iterator it=_quadTrees.begin(); it!=_quadTrees.end(); it++)
	{
		delete it->second;
	}
	_quadTrees.clear();
}

//Fixem el minim tamany que podrà tenir el quadre on mostrem el ràster.
QSize Display3D::minimumSizeHint() const
{
	return QSize(200, 200);
}

//Fixem el tamany que tindrà per defecte el quadre on mostrem ràster.
QSize Display3D::sizeHint() const
{
	return QSize(700, 600);
}

//Métode per inicialitzar les propietats de color, brillantor, llum, etc. de la figura en 3D.
void Display3D::initializeGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //netejar vista
	// squared landscapes by now
	Engine::Size<int> maxRasterSize = _simulationRecord->getSize();

	// update quadtrees and check for max raster size
	float maxResolution = 1.0f;

	for(QuadTreeMap::iterator it=_quadTrees.begin(); it!=_quadTrees.end(); it++)
	{
		delete it->second;
	}
	_quadTrees.clear();

	for(std::list<std::string>::const_iterator it =_orderedRasters.begin(); it!=_orderedRasters.end(); it++)
	{
		RasterConfiguration * rasterConfig = ProjectConfiguration::instance()->getRasterConfig(*it);
		if(rasterConfig->getCellResolution()>maxResolution)
		{
			maxResolution = rasterConfig->getCellResolution();
		}
		QuadTree * newQuadTree = new QuadTree(_simulationRecord->getSize()*rasterConfig->getCellResolution());
		newQuadTree->initializeChilds();
		_quadTrees.insert(make_pair(*it, newQuadTree));
	}
	maxRasterSize._width *= maxResolution;
	maxRasterSize._height *= maxResolution;

	float puntMig = sqrt(maxRasterSize._width*maxRasterSize._width+maxRasterSize._height*maxRasterSize._height);
	_radius = (puntMig)/2.f; //mida escenari/2
	_vrp._x = maxRasterSize._width/2.0f;
	_vrp._y = -maxRasterSize._height/2.0f;
	_vrp._z = 0;

	std::cout << "Radius = " << _radius<< std::endl;
	dist = 3*_radius;
	anglecam = ((asin(_radius/dist)*180)/3.1415)*2;
	std::cout << "Angle cam = " << anglecam << std::endl;
	ra = (float)width()/(float)height();
	_angle._x = _angle._y = _angle._z = 0;
	anterior = _radius;
	posterior = _radius*3 + anterior;
	focus();

	GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat materialShininess[] = {100.0f};

	float value = dist/1.05f;
	GLfloat lightPosition[] = {0.0f, 0.0f, value, 0.0f};
	GLfloat whiteLight[] = {1.0f, 1.0f, 1.0f, 0.0f};
	GLfloat modelAmbient[] = {0.5f, 0.5f, 0.5f, 0.0f};

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel(GL_FLAT);

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//carreguem la matriu identitat per situar el llum a l'observador
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glPopMatrix();

	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, modelAmbient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		
	_landscapeMaterial.registerTexture();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Display3D::extractFrustum()
{
	float proj[16];
	float modl[16];
	float clip[16];

	/* Get the current PROJECTION matrix from OpenGL */
	glGetFloatv( GL_PROJECTION_MATRIX, proj );

	/* Get the current MODELVIEW matrix from OpenGL */
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	/* Combine the two matrices (multiply projection by modelview) */
	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	/* Extract the numbers for the RIGHT plane */
	_frustum[0][0] = clip[ 3] - clip[ 0];
	_frustum[0][1] = clip[ 7] - clip[ 4];
	_frustum[0][2] = clip[11] - clip[ 8];
	_frustum[0][3] = clip[15] - clip[12];

	/* Extract the numbers for the LEFT plane */
	_frustum[1][0] = clip[ 3] + clip[ 0];
	_frustum[1][1] = clip[ 7] + clip[ 4];
	_frustum[1][2] = clip[11] + clip[ 8];
	_frustum[1][3] = clip[15] + clip[12];
	   
	/* Extract the BOTTOM plane */
	_frustum[2][0] = clip[ 3] + clip[ 1];
	_frustum[2][1] = clip[ 7] + clip[ 5];
	_frustum[2][2] = clip[11] + clip[ 9];
	_frustum[2][3] = clip[15] + clip[13];

	/* Extract the TOP plane */
	_frustum[3][0] = clip[ 3] - clip[ 1];
	_frustum[3][1] = clip[ 7] - clip[ 5];
	_frustum[3][2] = clip[11] - clip[ 9];
	_frustum[3][3] = clip[15] - clip[13];

	/* Extract the FAR plane */
	_frustum[4][0] = clip[ 3] - clip[ 2];
	_frustum[4][1] = clip[ 7] - clip[ 6];
	_frustum[4][2] = clip[11] - clip[10];
	_frustum[4][3] = clip[15] - clip[14];

	/* Extract the NEAR plane */
	_frustum[5][0] = clip[ 3] + clip[ 2];
	_frustum[5][1] = clip[ 7] + clip[ 6];
	_frustum[5][2] = clip[11] + clip[10];
	_frustum[5][3] = clip[15] + clip[14];

}

void Display3D::focus()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(anglecam, ra, anterior, posterior);

	// if not agent
	if(!_agentFocus)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
			
		glTranslatef(0,0,-dist);
		glRotatef(-_angle._x,1,0,0);
		glRotatef(-_angle._y,0,1,0);
		glRotatef(-_angle._z,0,0,1);

		glTranslatef(-_vrp._x,-_vrp._y,-_vrp._z);
		return;
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0,0,-dist);
	glRotatef(-_angle._x,1,0,0);
	glRotatef(-_angle._y,0,1,0);
	glRotatef(-_angle._z,0,0,1);
	//cout << dist << " " << angleX << " " << angleY << " " << angleZ << endl;
	
	Engine::Point2D<int> position(_agentFocus->getInt(_viewedStep/_simulationRecord->getFinalResolution(), "x"), _agentFocus->getInt(_viewedStep/_simulationRecord->getFinalResolution(), "y"));
	glTranslatef(-position._x, position._y, -_vrp._z);
}

void Display3D::updateRasterConfig()
{
	initializeGL();
	update();
}

void Display3D::paintLandscape()
{
	Engine::Point2D<int> index;

    std::list<std::string>::const_iterator it =_orderedRasters.end();
    while(it!=_orderedRasters.begin())
    {
        it--;		
        Engine::StaticRaster & colorRaster = _simulationRecord->getRasterTmp(*(it), _viewedStep);

        glPushMatrix();
        _landscapeMaterial.activate();

        glEnable(GL_CULL_FACE);

		RasterConfiguration * rasterConfig = ProjectConfiguration::instance()->getRasterConfig(*(it));
		QuadTreeMap::iterator qIt = _quadTrees.find(*it);
		QuadTree * quadTree = qIt->second;
		int pot2X = powf(2,ceil(log2(rasterConfig->getCellResolution()*_simulationRecord->getSize()._width)));
		int pot2Y = powf(2,ceil(log2(rasterConfig->getCellResolution()*_simulationRecord->getSize()._height)));
		if(rasterConfig->hasElevationRaster())
		{
        	Engine::StaticRaster & elevationRaster(_simulationRecord->getRasterTmp(rasterConfig->getElevationRaster(), _viewedStep));
        	quadTree->update(Engine::Point2D<int>(pot2X, pot2Y), *rasterConfig, colorRaster, elevationRaster, _randomColor);
		}
		else
		{
        	quadTree->update(Engine::Point2D<int>(pot2X, pot2Y), *rasterConfig, colorRaster,_plane, _randomColor);
		}

        _landscapeMaterial.deactivate();
        glPopMatrix();
    }
}

void Display3D::setCellColor( const QColor & color )
{
	float ambientColor[3];
	float diffuseColor[3];
	float specularColor[3];

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

void Display3D::paintAgents()
{
	// TODO paint agents in selected rasters
	Engine::StaticRaster * raster = 0;
	for(Engine::SimulationRecord::AgentTypesMap::const_iterator itType = _simulationRecord->beginTypes(); itType!=_simulationRecord->endTypes(); itType++)
	{
		AgentConfiguration * agentConfig = ProjectConfiguration::instance()->getAgentConfig(itType->first);
		for(Engine::SimulationRecord::AgentRecordsMap::const_iterator it= _simulationRecord->beginAgents(itType); it!=_simulationRecord->endAgents(itType); it++)
		{
			Engine::AgentRecord * agent = it->second;
			bool exists = agent->getInt(_viewedStep/_simulationRecord->getFinalResolution(), "exists");
			if(!exists)
			{
				continue;
			}
			Engine::Point3D<int> position(agent->getInt(_viewedStep/_simulationRecord->getFinalResolution(), "x"), agent->getInt(_viewedStep/_simulationRecord->getFinalResolution(), "y"), 0);
            position._z = 1;
			if(raster)
			{
				position._z += raster->getValue(Engine::Point2D<int>(position._x,position._y));
			}
					
			glPushMatrix();
			glTranslatef(position._x, -position._y, position._z);
			if(!agentConfig->getFileName3D().empty())
			{
				agentConfig->getModel().paint();
			}
			glPopMatrix();
		}
	}
}

//Métode on dibuixem la figura del ràster dins el Widget creat amb QT.
void Display3D::paintGL()
{
    _time.restart();
    _time.start();

	if(!_simulationRecord)
	{
		return;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glViewport (0, 0, width(), height());
	focus();
	
	glPushMatrix();
	
	extractFrustum();
	glPopMatrix();

	for(QuadTreeMap::iterator it=_quadTrees.begin(); it!=_quadTrees.end(); it++)
	{
		QuadTree * quadTree = it->second;
		quadTree->setFrustum(_frustum);
	}
	paintLandscape();
	paintAgents();
	
	glPopMatrix();

	std::stringstream fps;
	fps << "s: " << _time.elapsed()/1000.0;
	setWindowTitle(fps.str().c_str());
}

//Mètode per re-estructurar el Widget en cas de que es modifiqui el tamany manualment.
void Display3D::resizeGL(int w, int h)
{
	std::cout << "resize GL: " << w << "/" << h << std::endl;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)w/(GLfloat)h, 1.0f, 100000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Display3D::wheelEvent( QWheelEvent * event )
{
    dist -=event->delta()/(10);
    if(dist < 45) dist = 45;
    anterior = dist/10;
    posterior = dist*4 + anterior;

    //_zoom -= event->delta()/120;
    updateGL();
}

void Display3D::mouseReleaseEvent( QMouseEvent *)
{
	_interactiveAction = eNone;
}

void Display3D::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_R)
    {
        float center = sqrt(_simulationRecord->getSize()._width*_simulationRecord->getSize()._width+_simulationRecord->getSize()._height*_simulationRecord->getSize()._height);

        _radius = (center)/2.f; //mida escenari/2
        _vrp._x = _simulationRecord->getSize()._width/2.0f;
        _vrp._y = -_simulationRecord->getSize()._height/2.0f;
        _vrp._z = 0;

		std::cout << "Radius = " << _radius<< std::endl;
        dist = 3*_radius;

        //cout << "Angle cam = " << ((atan(height()/(2*((3*dist) - dist))))*180)/3.1415 << endl;
        anglecam = ((asin(_radius/dist)*180)/3.1415)*2;

        //anglecam = (((atan(radi/dist)*180)/3.1415)*2);
		std::cout << "Angle cam = " << anglecam << std::endl;
        ra = (float)width()/(float)height();
        _angle._x = 0;
        _angle._y = 0;
        _angle._z = 0;
        anterior = _radius;
        posterior = _radius*3 + anterior;
        focus();
    }
    if(event->key()==Qt::Key_C)
    {
		_randomColor = !_randomColor;
	}
        
	update();
}


//Mètode que ens retorna la posició del cursor en un moment determinat.
void Display3D::mousePressEvent(QMouseEvent *event)
{
	// TODO maybe globalPos?
	_clickedPos = event->pos();
	if(event->button()==Qt::LeftButton)
	{
		if(event->modifiers() & Qt::ControlModifier)
		{
			_interactiveAction = eRotate;
		}
		else
		{
			_interactiveAction = eMove;
		}
		return;
	}
	if(event->button()==Qt::RightButton)
	{
		_interactiveAction = ePan;
	}
}

void Display3D::move( const QPoint & eventPos )
{
	_angle._x += _clickedPos.y() - eventPos.y();
	if(_angle._x > 90)
	{
		_angle._x = 90;
	}
	else if(_angle._x < -90)
	{
		_angle._x = -90;
	}

   _angle._y += _clickedPos.x() - eventPos.x();
	if(_angle._y > 90)
	{
		_angle._y = 90;
	}
	else if(_angle._y < -90)
	{
		_angle._y = -90;
	}
}

void Display3D::rotate( const QPoint & eventPos )
{
	_angle._z += _clickedPos.y() - eventPos.y();
}

void Display3D::pan( const QPoint & eventPos )
{
	Engine::Point3D<float> incX;
	Engine::Point3D<float> incY;

	float mat[4][4];
	glGetFloatv(GL_MODELVIEW_MATRIX, &mat[0][0]);
			
	incX._x = mat[0][0];
	incX._y = mat[1][0];
	incX._z = mat[2][0];

	incY._x = mat[0][1];
	incY._y = mat[1][1];
	incY._z = mat[2][1];

	incX = incX * _simulationRecord->getSize()._width * (_clickedPos.x()-eventPos.x());
	incX = incX/dist;
	
	incY = incY * _simulationRecord->getSize()._height * (eventPos.y()-_clickedPos.y());
	incY = incY/dist;

	_vrp = _vrp + incX;
	_vrp = _vrp + incY;
}

void Display3D::mouseMoveEvent(QMouseEvent *event)
{
	switch(_interactiveAction)
	{
		case eMove:
			move(event->pos());
			break;

		case eRotate:
			rotate(event->pos());
			break;

		case ePan:
			pan(event->pos());
			break;

		case eNone:
		case eZoom:
			break;

		default:	
			std::stringstream oss;
			oss << "Display3D::mouseMoveEvent - unrecognized interactive action: " << _interactiveAction;
			throw Engine::Exception(oss.str());
	}
	_clickedPos = event->pos();
    focus();
    update();
}

void Display3D::resetView()
{
	_orderedRasters.clear();
	_viewedStep = 0;
	_plane.resize(_simulationRecord->getSize());
	_plane.setInitValues(1,1,1);
	initializeGL();
}

void Display3D::setSimulationRecord( Engine::SimulationRecord * simulationRecord )
{
	_simulationRecord = simulationRecord;
	resetView();
	update();
}

void Display3D::rastersRearranged( std::list<std::string> items, std::list<bool> views )
{
	_orderedRasters.clear();

	std::list<bool>::const_iterator itView = views.begin();
	for(std::list<std::string>::const_iterator it=items.begin(); it!=items.end(); it++)
	{
		if((*itView)==true)
		{
			_orderedRasters.push_front(*it);
		}
		itView++;
	}
    update();
}

void Display3D::viewedStepChangedSlot( int newViewedStep )
{
	_viewedStep = newViewedStep;
	update();
}

void Display3D::setAgentFocus(  Engine::AgentRecord * agentFocus )
{
	_agentFocus = agentFocus;
}

} // namespace GUI

