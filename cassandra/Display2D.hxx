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

#ifndef __Display2D_hxx__
#define __Display2D_hxx__

#include <Point2D.hxx>

#include <QWidget>
#include <QWheelEvent>
#include <vector>
#include <QListWidget>
#include <string>
#include <AgentRecord.hxx>

#include <QTreeWidget>

class QListWidgetItem;

namespace Engine
{
	class SimulationRecord;
	class Raster;
}

namespace GUI
{
class AgentConfiguration;
class RasterConfiguration;

class Display2D : public QWidget
{
	Q_OBJECT
	
	typedef std::map<std::string, AgentConfiguration *> AgentsConfigurationMap;
	typedef std::map<std::string, RasterConfiguration *> RastersConfigurationMap;
	typedef std::map<std::string, QColor > StringToColorMap;

	Engine::SimulationRecord * _simulationRecord;
	
	int _viewedStep;
	
	float _zoom;
	bool _showAgents;
    int _radiusSelection;

	QPoint _offset;
	QPoint _clickedPos;

	// state being selected
	std::string _type;
	std::string _state;
    StringToColorMap _strToColor;
	// order of rasters
	std::list<std::string> _orderedRasters;

	std::string getRasterToolTip( const Engine::Point2D<int> & position );
	std::string getAgentToolTip( const Engine::Point2D<int> & position );

    // how many pixels correspond to one raster pixel
    int _sizePixel;
    
    QColor getRandomColor() const;
public:
	void zoom( float value );
	Display2D(QWidget * parent);
	virtual ~Display2D();
	void setSimulationRecord( Engine::SimulationRecord * simulationRecord );
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
    QTreeWidget *agentList;

	void setRadiusSelection( int radiusSelection );
	int getRadiusSelection() const;
    QSize getRealSize() const;
	// cleans all the display options
	void resetView();
    void setViewedStep( int viewedStep );

protected:  
	bool event(QEvent *event);
	void paintEvent(QPaintEvent * event);
	void wheelEvent(QWheelEvent * event);
	void mousePressEvent(QMouseEvent * event);	
    void mouseMoveEvent(QMouseEvent * event);
    void mouseDoubleClickEvent(QMouseEvent *event);
public slots:
	void viewedStepChangedSlot( int newViewedStep );
	void zoomIn();
	void zoomOut();
	void showAgents();
	void typeSelected( QListWidgetItem * item);
	void stateSelected( QListWidgetItem * item);
	void rastersRearranged( std::list<std::string> items, std::list<bool> views );
	void radiusSelectionModified(int);
signals:
	void calculateStatistics( const std::string &, const std::string & );
    void updateAgentsSelected(std::list<Engine::AgentRecord*>,Engine::SimulationRecord *);
};

} // namespace GUI

#endif // __Display2D_hxx__

