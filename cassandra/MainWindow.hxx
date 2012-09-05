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

#ifndef __MainWindow_hxx__
#define __MainWindow_hxx__

#include <QMainWindow>
#include <map>
#include <Point3D.hxx>
#include <Configuration3D.hxx>
#include <LoadSimulationThread.hxx>

class QMenu;
class QAction;
class QToolBar;
class QSpinBox;
class QLabel;
class QTimer;
class QListWidgetItem;

namespace GUI
{

class Display2D;
class AgentTraitSelection;
class AgentTypeSelection;
class RasterSelection;
class GenericStatistics;
class Display3D;
class AgentConfiguration;
class RasterConfiguration;

class LoadingProgressBar;

class MainWindow : public QMainWindow
{

private:
	Q_OBJECT

	QMenu * _fileMenu;
	QMenu * _simulationMenu;
	QMenu * _viewMenu;

	QToolBar * _fileBar;
	QToolBar * _simulationBar;
	QToolBar * _viewBar;

	QAction * _newProjectAction;
	QAction * _loadProjectAction;
	QAction * _saveProjectAction;
	QAction * _saveProjectAsAction;
	QAction * _selectSimulationAction;
	QAction * _quitAction;

	QAction * _firstStepAction;
	QAction * _previousStepAction;
	QAction * _nextStepAction;
	QAction * _finalStepAction;
	QAction * _playAction;

	QAction * _zoomInAction;
	QAction * _zoomOutAction;
	QAction * _showAgentsAction;

	QAction * _show3DAction;
	QAction * _options3DAction;

	QSpinBox * _stepBox;
	QLabel * _numStepsLabel;

	// used for playing the simulation
	QTimer * _playTimer;
	QTimer * _loadSimulationTimer;

	Display2D * _display2D;
	Display3D * _display3D;

	AgentTypeSelection * _agentTypeSelection;
	AgentTraitSelection * _agentTraitSelection;
	RasterSelection * _rasterSelection;
	GenericStatistics * _genericStatistics;
	
	int _viewedStep;
	void setViewedStep( int viewedStep );
//	Raster3D _Raster3D;
	
	bool _windowRaster;

	void adjustGUI();

	LoadingProgressBar * _progressBar;
	LoadSimulationThread _loadSimulationThread;
public:
	MainWindow();
	virtual ~MainWindow();

private slots:

	void newProject();
	void loadProject();
	void saveProject();
	void saveProjectAs();
	void selectSimulation();
	void quit();

	void firstStep();
	void previousStep();
	void nextStep();
	void finalStep();
	void play();

	void viewedStepChanged(int viewedStep);
	void updatePlay();

	void openAgentConfigurator(QListWidgetItem * item);
	void openRasterConfigurator(QListWidgetItem * item);
	
	void show3DWindow();
	void show3DOptions();

	void agentConfigured( const std::string & , const AgentConfiguration &); 
	void rasterConfigured( const std::string & , const RasterConfiguration &); 	
	void configured3D( const Configuration3D &);

	void loadSimulationFinished( bool correct);
signals:
	void stepChangeToStepBox(int stepChangeToStepBox);
	void newViewedStep(int newViewedStep);
};

} // namespace GUI

#endif // __MainWindow_hxx__

