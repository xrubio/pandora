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

#include <Exceptions.hxx>
#include <SimulationRecord.hxx>
#include <Model3D.hxx>
#include <Point2D.hxx>
#include <QtGui>
#include <AgentConfiguration.hxx>
#include <RasterConfiguration.hxx>
#include <Configuration3D.hxx>
#include <ColorSelector.hxx>
#include <StaticRaster.hxx>
#include <ProjectConfiguration.hxx>

#include <algorithm>

namespace GUI
{

//Inicialitzem els paràmetres al constructor per defecte i carreguem l'arxiu on hi ha tota la informació referent al ràster
//que posteriorment haurem de representar (x,y,z).
Display3D::Display3D(QWidget *parent ) : QGLWidget(parent), _simulationRecord(0), _viewedStep(0), _zoom(1.0f), _position(0,0), _lastPos(0,0), _rotation(0,0), _rotationZ(0),  _cellScale(1.0f, 1.0f, 1.0f), _config3D(ProjectConfiguration::instance()->getConfig3D())
{
	//_landscapeMaterial.setTextureFileName("landscape1.bmp");
}

Display3D::~Display3D()
{
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
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
	//Definim variables d'il·luminació i material
	float light_ambient[] = {1.0, 1.0, 1.0, 1.0};
	float light_color[] = {1.0, 1.0, 1.0};
	float light_direction[] = {-1.0, -1.0, -1.0, 0.0};
	
	float spot_cutoff = 180.0;
	float spot_exponent = 1.0;
	float spot_dir[] = {-1.0, -1.0, -1.0};
	float spot_color[] = {1.0, 1.0, 1.0};
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_COLOR_MATERIAL);
	
	//Tipus d'il·luminació que li volem donar a l'escena
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, light_ambient);
	
// 	Tipus d'il·luminació amb un SPOT
	glLightfv(GL_LIGHT1, GL_DIFFUSE, spot_color);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spot_cutoff);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spot_exponent);
	//glEnable -> "encèn" (Recordatori: OpenGL es com una màquina d'estats)
	//Iluminació disponible.
	*/

	GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat materialShininess[] = {50.0f};

	GLfloat lightPosition[] = {10.0f, 3.0f, 5.0f, 0.0f};	
	GLfloat whiteLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat modelAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel(GL_FLAT);

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteLight);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, modelAmbient);

	glEnable(GL_LIGHTING);
//	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);


	/*
	for(Models3DMap::iterator it=_models.begin(); it!=_models.end(); it++)
	{
		Model3D * model = (Model3D*)(it->second);
		model->registerTextures();
	}
	*/
	
	_landscapeMaterial.registerTexture();
	//glEnable(GL_CULL_FACE);
	/*
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
*/	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Definim la posició de la nostra llum ambient.
	/*
	glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
	//Definim la posició del SPOT.
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_dir);
	*/
}

void Display3D::paintLandscape()
{
	Engine::Point2D<int> index;
//	Engine::Raster & raster = _simulationRecord->getRaster(_selectedRaster, _viewedStep);
	Engine::StaticRaster & DEMRaster = _simulationRecord->getRasterTmp(_config3D.getDEMRaster(), _viewedStep);
	Engine::StaticRaster & colorRaster = _simulationRecord->getRasterTmp(_config3D.getColorRaster(), _viewedStep);

	glPushMatrix();
	_landscapeMaterial.activate();
	
	// general plane covering the entire world
	/*
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3d(5000, 5000, 1);
		
		glTexCoord2f(0.0f, 1.0f);
		glVertex3d(-5000, 5000, 1);
		
		glTexCoord2f(1.0f, 1.0f);
		glVertex3d(-5000, -5000, 1);
		
		glTexCoord2f(1.0f, 0.0f);
		glVertex3d(5000, -5000, 1);
	glEnd();
	*/
	const Engine::Point3D<float> scale(_config3D.getSize3D());
	
//	glTranslatef(-_cellScale._x*_simulationRecord->getSize()/2, -_cellScale._y*_simulationRecord->getSize()/2, 0.0f);
	RasterConfiguration * rasterConfig = ProjectConfiguration::instance()->getRasterConfig(_config3D.getColorRaster());
	const ColorSelector & colorSelector(rasterConfig->getColorRamp());

	for(index._x=0; index._x<DEMRaster.getSize()._x-1; index._x++)
	{	
		for(index._y=0; index._y<DEMRaster.getSize()._y-1; index._y++)
		{		
			Engine::Point3D<float> topLeft(index._x, index._y, 0.0f);
			topLeft._z = DEMRaster.getValue(Engine::Point2D<int>(topLeft._x,topLeft._y));
			float topLeftColor = colorRaster.getValue(Engine::Point2D<int>(topLeft._x,topLeft._y));

			Engine::Point3D<float> topRight(index._x+1, index._y, 0.0f);
			topRight._z = DEMRaster.getValue(Engine::Point2D<int>(topRight._x,topRight._y));
			float topRightColor = colorRaster.getValue(Engine::Point2D<int>(topRight._x,topRight._y));

			Engine::Point3D<float> bottomRight(index._x+1, index._y+1, 0.0f);
			bottomRight._z = DEMRaster.getValue(Engine::Point2D<int>(bottomRight._x,bottomRight._y));
			float bottomRightColor = colorRaster.getValue(Engine::Point2D<int>(bottomRight._x,bottomRight._y));

			Engine::Point3D<float> bottomLeft(index._x, index._y+1, 0.0f);
			bottomLeft._z = DEMRaster.getValue(Engine::Point2D<int>(bottomLeft._x,bottomLeft._y));
			float bottomLeftColor = colorRaster.getValue(Engine::Point2D<int>(bottomLeft._x,bottomLeft._y));

			// normal
			Engine::Point3D<float> a = bottomLeft;
			a = a - topRight;
			Engine::Point3D<float> b = bottomRight;
			b = b - topRight;

			Engine::Point3D<float> c = a.crossProduct(b);
			c = c.normalize();
			glNormal3f(c._x, c._y, -c._z);
			
			glBegin(GL_QUADS);

			setCellColor(colorSelector.getColor(topLeftColor));
			glTexCoord2f(topLeft._x/_simulationRecord->getSize(), topLeft._y/_simulationRecord->getSize());
			glVertex3f(scale._x*topLeft._x, -scale._y*topLeft._y, scale._z*topLeft._z);
			
			setCellColor(colorSelector.getColor(bottomLeftColor));
			glTexCoord2f(bottomLeft._x/_simulationRecord->getSize(), bottomLeft._y/_simulationRecord->getSize());
			glVertex3f(scale._x*bottomLeft._x, -scale._y*bottomLeft._y, scale._z*bottomLeft._z);
			
			setCellColor(colorSelector.getColor(bottomRightColor));
			glTexCoord2f(bottomRight._x/_simulationRecord->getSize(), bottomRight._y/_simulationRecord->getSize());
			glVertex3f(scale._x*bottomRight._x, -scale._y*bottomRight._y, scale._z*bottomRight._z);
			
			setCellColor(colorSelector.getColor(topRightColor));
			glTexCoord2f(topRight._x/_simulationRecord->getSize(), topRight._y/_simulationRecord->getSize());
			glVertex3f(scale._x*topRight._x, -scale._y*topRight._y, scale._z*topRight._z);
			
			glEnd();
		}
	}
	_landscapeMaterial.deactivate();
	glPopMatrix();
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
	Engine::StaticRaster & raster = _simulationRecord->getRasterTmp(_config3D.getDEMRaster(), _viewedStep);
	const Engine::Point3D<float> scale(_config3D.getSize3D());

	for(Engine::SimulationRecord::AgentTypesMap::const_iterator itType = _simulationRecord->beginTypes(); itType!=_simulationRecord->endTypes(); itType++)
	{
		AgentConfiguration * agentConfig = ProjectConfiguration::instance()->getAgentConfig(itType->first);
		for(Engine::SimulationRecord::AgentRecordsMap::const_iterator it= _simulationRecord->beginAgents(itType); it!=_simulationRecord->endAgents(itType); it++)
		{
			Engine::AgentRecord * agent = it->second;
			bool exists = agent->getState(_viewedStep/_simulationRecord->getResolution(), "exists");

			if(!exists)
			{
				continue;
			}
			Engine::Point3D<int> position(agent->getState(_viewedStep/_simulationRecord->getResolution(), "x"), agent->getState(_viewedStep/_simulationRecord->getResolution(), "y"), 0);
			position._z = -1+raster.getValue(Engine::Point2D<int>(position._x,position._y));
					
			glPushMatrix();
			//std::cout << "painting agent: " << it->first << " at pos: " << position << std::endl;
			glTranslatef(scale._x*position._x, -scale._y*position._y, scale._z*position._z);
			//glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
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
	if(!_simulationRecord || _orderedRasters.empty())
	{
		return;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

//	float rotateX = 1 + float(_rotation.x());
	//std::cout << "rotation x: " << _rotation.x() << " float: " << rotateX << std::endl;
//	gluLookAt(_position.x(), _position.y(), _zoom*1000.0f, _position.x(), _position.y(), 0.0f, 0.0f, 1.0f, 0.0f);
//	gluLookAt(-_position.x(), _position.y(), _zoom*5000.0f, -_position.x(), _position.y(), 0.0f, 0.0f, 1.0f, 0.0f);
//	gluLookAt(0, 0, 0, 0, 0, -_zoom*100.0, 0.0f, 1.0f, 0.0f);
	gluLookAt(0, 0, 0, 0, 0, -1, 0.0f, 1.0f, 0.0f);

//	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	/*
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0,0, _zoom*_simulationRecord->getSize()*0.3f);
	*/


	// TODO multiplicar el valor per sinus i cosinus de l'eix?
	/*
	glTranslatef(_position.x(), _position.y(), 0.0f);
	*/
	glTranslatef(0,0,-_zoom*100.0f);

	glRotatef(_rotation.x(), 0.0f, 1.0f, 0.0f);
	glRotatef(_rotation.y(), 1.0f, 0.0f, 0.0f);
	glRotatef(_rotationZ, 0.0f, 0.0f, 1.0f);

	glTranslatef(_position.x()-_cellScale._x*_simulationRecord->getSize()/2, -_position.y()+_cellScale._y*_simulationRecord->getSize()/2, 0.0f);
//	glTranslatef(_position.x()-_cellScale._x*_simulationRecord->getSize()/2, -_position.y()+_cellScale._y*_simulationRecord->getSize()/2, -_zoom*100.0);


	/*
	glRotatef(_rotation.y(), 1.0f, 0.0f, 0.0f);
	*/

	paintLandscape();
		
	paintAgents();

	glPopMatrix();
	
	glFlush();
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
	_zoom -= event->delta()/120;
	updateGL();
}

//Mètode que ens retorna la posició del cursor en un moment determinat.
void Display3D::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{	
		if(event->modifiers() & Qt::ShiftModifier)
		{
			_lastTargetPos = event->globalPos() - _targetPosition;
		}
		else
		{
			_lastTargetPos = event->globalPos() - _targetPosition;
			_lastPos = event->globalPos() - _position;
		}
		update();
		return;
	}
	else if(event->button() == Qt::MidButton)
	{
		_lastPos = event->globalPos() - _rotation;	
		update();
		return;
	}
	else if(event->button() == Qt::RightButton)
	{	
		//_lastPos = event->globalPos() - _targetPosition;
		_lastPos.setX(event->globalPos().x() - _rotationZ);
		update();
		return;
	}
}

//Mètode per modificar el punt de vista des d'on mirem el raster a partir del mouse.
void Display3D::mouseMoveEvent(QMouseEvent *event)
{
	if(event->buttons() & Qt::LeftButton)
	{
		if(event->modifiers() & Qt::ShiftModifier)
		{
			_targetPosition = event->globalPos() - _lastTargetPos;
		}
		else
		{
			_position = event->globalPos() - _lastPos;
			_targetPosition = event->globalPos() - _lastTargetPos;
		}
		update();
		return;
	}
	if(event->buttons() & Qt::MidButton)
	{
		_rotation = event->globalPos() - _lastPos;

	
		/*_rotation.setX(std::max(-85, _rotation.x()));
		_rotation.setX(std::min(85, _rotation.x()));
		_rotation.setY(std::max(-85, _rotation.y()));
		_rotation.setY(std::min(85, _rotation.y()));
		*/
		update();
		return;
	}
	if(event->buttons() & Qt::RightButton)
	{
		_rotationZ = event->globalPos().x() - _lastPos.x();
		update();
		return;
	}
}

//Normalitzem l'angle per tenir un angle entre 0 i 2PI
void Display3D::normalizeAngle(int & angle)
{
	//Mentre l'angle sigui menor que 0 anirem sumant al angle que tenim 
	//NOTA: Multipliquem per 16 pq es el rang del QSlider que em definit.
	while (angle < 0)
	{
		angle += 360 * 16;
	}
	while (angle > 360 * 16)
	{
		angle -= 360 * 16;
	}
}

void Display3D::setSimulationRecord( Engine::SimulationRecord * simulationRecord )
{
	_simulationRecord = simulationRecord;
	if(!_simulationRecord)
	{
		_orderedRasters.clear();
	}
	update();
}

void Display3D::rastersRearranged( std::list<std::string> items )
{
	_orderedRasters.clear();	
	_orderedRasters.resize(items.size());
	std::reverse_copy(items.begin(), items.end(), _orderedRasters.begin());
	update();
}

void Display3D::viewedStepChangedSlot( int newViewedStep )
{
	_viewedStep = newViewedStep;
	update();
}

} // namespace GUI

