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

#ifndef __Display3D_hxx__
#define __Display3D_hxx__

#include <map>
#include <Material.hxx>

#include <QGLWidget>
#include <Point3D.hxx>
#include <QTime>
#include <DynamicRaster.hxx>

class QListWidgetItem;

namespace Engine
{
	class SimulationRecord;
	class AgentRecord;
}

namespace GUI
{

class Model3D;
class AgentConfiguration;
class RasterConfiguration;
class QuadTree;

enum InteractiveAction
{
	eNone,
	eRotate,
	eZoom,
	ePan,
	eMove	
};

class Display3D : public QGLWidget
{
	Q_OBJECT
	
	typedef std::map<std::string, Model3D * > Models3DMap;
	typedef std::map<std::string, std::string > FileNamesMap;
	typedef std::map<std::string, AgentConfiguration *> AgentsConfigurationMap;
	typedef std::map<std::string, RasterConfiguration *> RastersConfigurationMap;
	typedef std::map<std::string, QuadTree * > QuadTreeMap;

public:
	Display3D(QWidget *parent);
	virtual ~Display3D();

	// size of widget
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	void setSimulationRecord( Engine::SimulationRecord * simulationRecord );

	// if this setter is used the Display3D will follow this position
	void setAgentFocus( Engine::AgentRecord * agentRecord );

	void updateRasterConfig();

public slots:
	void viewedStepChangedSlot( int newViewedStep );
	void rastersRearranged( std::list<std::string> items, std::list<bool> views );
	
private:

	// OpenGL basic related functions
	//TODO Zoom y moviments amb el mouse
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);

	// interaction
	InteractiveAction _interactiveAction;
	QPoint _clickedPos;

	void mousePressEvent(QMouseEvent *event);

	// modifies camera point of view based on mouse actions
	void mouseMoveEvent(QMouseEvent *event);
	void move( const QPoint & eventPos );
	void pan( const QPoint & eventPos );
	void rotate( const QPoint & eventPos );

    void mouseReleaseEvent (QMouseEvent *e);
	void wheelEvent(QWheelEvent * event);
    void keyPressEvent(QKeyEvent *event);

	// drawing methods
	void paintLandscape();
	void paintAgents();
	void extractFrustum();
	void setCellColor( const QColor & color );
    
	// selection of agent
	void focus();

	QuadTreeMap _quadTrees;
	Engine::Point3D<float> _angle;
	double dist, anterior, posterior, _radius, anglecam, ra;
	// point of view of observer
	Engine::Point3D<float> _vrp;

	// helper raster used when no elevation raster is defined
	Engine::DynamicRaster _plane;

	Engine::SimulationRecord * _simulationRecord;
	int _viewedStep;
	Models3DMap _models;
	// we need this map in order to check if models are new or already loadd (from registerModels)
	FileNamesMap _paths;
	Material _landscapeMaterial;

	float _zoom;
	QPoint _position;
	QPoint _targetPosition;
	QPoint _lastPos;
	// TODO improve mouse navigation. At the moment this attribute is not used
	QPoint _lastTargetPos;	
	QPoint _rotation;
	int _rotationZ;

    int terrainSize;


	Engine::Point3D<float> _cellScale;

	// order of rasters
	std::list<std::string> _orderedRasters;

    QTime _time;

	// cleans all the display options
	void resetView();
	Engine::AgentRecord * _agentFocus;
    float _frustum[6][4];

	bool _randomColor;

};

}

#endif // __Display3D_hxx__

