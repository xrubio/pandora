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

#include <MainWindow.hxx>

#include <Display2D.hxx>
#include <AgentTypeSelection.hxx>
#include <AgentTraitSelection.hxx>
#include <RasterSelection.hxx>
#include <GenericStatistics.hxx>
#include <AgentConfigurator.hxx>
#include <Display3D.hxx>
#include <AgentConfiguration.hxx>
#include <RasterConfigurator.hxx>
#include <Exception.hxx>
#include <ProjectConfiguration.hxx>
#include <SimulationRecord.hxx>
#include <LoadingProgressBar.hxx>
#include <Settings.hxx>
#include <Laboratory.hxx>
#include <AgentAnalysis.hxx>
#include <RasterAnalysis.hxx>

#include <iostream>

#include <QFont>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSpinBox>
#include <QLabel>
#include <QTimer>
#include <QDockWidget>
#include <QListWidgetItem>
#include <QInputDialog>
#include <boost/filesystem.hpp>

#include <HeatMapDialog.hxx>
#include <TimeSeriesDialog.hxx>

namespace GUI
{

MainWindow::MainWindow() : _display2D(0), _display3D(0), _agentTypeSelection(0), _agentTraitSelection(0), _rasterSelection(0), _genericStatistics(0), _viewedStep(0), _progressBar(0)//, _Raster3D(0)//, _windowRaster(true)*/
{ 
	QDockWidget * agentTypeSelectionDock = new QDockWidget(tr("Agent Types"), this);
	agentTypeSelectionDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	_agentTypeSelection = new AgentTypeSelection(agentTypeSelectionDock);	
	agentTypeSelectionDock->setWidget(_agentTypeSelection);
	addDockWidget(Qt::LeftDockWidgetArea, agentTypeSelectionDock);

	QDockWidget * agentTraitSelectionDock = new QDockWidget(tr("Agent Traits"), this);
	agentTraitSelectionDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	_agentTraitSelection = new AgentTraitSelection(agentTraitSelectionDock);
	agentTraitSelectionDock->setWidget(_agentTraitSelection);
	addDockWidget(Qt::LeftDockWidgetArea, agentTraitSelectionDock);

	QDockWidget * rasterSelectionDock = new QDockWidget(tr("Raster"), this);
	rasterSelectionDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	_rasterSelection = new RasterSelection(rasterSelectionDock);
	rasterSelectionDock->setWidget(_rasterSelection);
	addDockWidget(Qt::LeftDockWidgetArea, rasterSelectionDock);

	QDockWidget * genericStatisticsDock = new QDockWidget(tr("Generic Statistics"), this);	
	genericStatisticsDock->setAllowedAreas(Qt::BottomDockWidgetArea);	
	_genericStatistics = new GenericStatistics(genericStatisticsDock);

	genericStatisticsDock->setWidget(_genericStatistics);
	addDockWidget(Qt::BottomDockWidgetArea, genericStatisticsDock);

	// resource display 2D
	_display2D = new Display2D(this);
	_display3D = new Display3D(0);
	_laboratory = new Laboratory(this);
	_agentAnalysis = new AgentAnalysis(this);
	_rasterAnalysis = new RasterAnalysis(this);
	_settings = new Settings;
	
	_display2D->show();
	_display3D->hide();
	_laboratory->hide();
	_agentAnalysis->hide();
	_settings->hide();
	
    setCentralWidget(_display2D);
//	setCentralWidget(_display3D);
	
	connect(this, SIGNAL(newViewedStep(int)), _display2D, SLOT(viewedStepChangedSlot(int)));
    connect(this, SIGNAL(newViewedStep(int)), _display3D, SLOT(viewedStepChangedSlot(int)));
	connect(this, SIGNAL(newViewedStep(int)), _genericStatistics, SLOT(viewedStepChangedSlot(int)));
	connect(_display2D, SIGNAL(calculateStatistics(const std::string &, const std::string &)), _genericStatistics, SLOT(calculateStatistics(const std::string &, const std::string &)));
	connect(_agentTypeSelection, SIGNAL(itemActivated(QListWidgetItem *)), _display2D, SLOT(typeSelected(QListWidgetItem *)));
	connect(_agentTypeSelection, SIGNAL(itemActivated(QListWidgetItem *)), _agentTraitSelection, SLOT(typeSelected(QListWidgetItem *)));
	connect(_agentTypeSelection, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(openAgentConfigurator(QListWidgetItem *)));
	connect(_agentTraitSelection, SIGNAL(itemActivated(QListWidgetItem *)), _display2D, SLOT(stateSelected(QListWidgetItem *)));
	connect(_rasterSelection, SIGNAL(rastersRearranged(std::list<std::string>, std::list<bool>)), _display2D, SLOT(rastersRearranged(std::list<std::string>, std::list<bool>)));
	connect(_rasterSelection, SIGNAL(rastersRearranged(std::list<std::string>, std::list<bool>)), _display3D, SLOT(rastersRearranged(std::list<std::string>, std::list<bool>)));
	connect(_rasterSelection, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(openRasterConfigurator(QListWidgetItem *)));
	connect(_settings, SIGNAL(radiusSelectionModified(int)), _display2D, SLOT(radiusSelectionModified(int)));

//    connect(this, SIGNAL(newViewedStep(int)), displayAgent, SLOT(viewedStepChangedSlot(int)));
  //  connect(_rasterSelection, SIGNAL(rastersRearranged(std::list<std::string>)), displayAgent, SLOT(rastersRearranged(std::list<std::string>)));


	// file menu 
	_newProjectAction = new QAction(QIcon(":share/cassandra/icons/project_new.png"), tr("&New Project"), this);
	_newProjectAction->setShortcut(tr("Ctrl+N"));
	_newProjectAction->setStatusTip(tr("Create new project"));
	connect(_newProjectAction, SIGNAL(triggered()), this, SLOT(newProject()));

	_loadProjectAction = new QAction(QIcon(":share/cassandra/icons/project_open.png"), tr("&Load Project..."), this);
	_loadProjectAction->setShortcut(tr("Ctrl+L"));
	_loadProjectAction->setStatusTip(tr("Load project"));
	connect(_loadProjectAction, SIGNAL(triggered()), this, SLOT(loadProject()));

	_saveProjectAction = new QAction(QIcon(":share/cassandra/icons/project_save.png"), tr("&Save Project..."), this);
	_saveProjectAction->setShortcut(tr("Ctrl+S"));
	_saveProjectAction->setStatusTip(tr("Save project"));
	connect(_saveProjectAction, SIGNAL(triggered()), this, SLOT(saveProject()));

	_saveProjectAsAction = new QAction(QIcon(":share/cassandra/icons/project_save_as.png"), tr("&Save Project as..."), this);
	_saveProjectAsAction->setShortcut(tr("Ctrl+Alt+S"));
	_saveProjectAsAction->setStatusTip(tr("Save project as"));
	connect(_saveProjectAsAction, SIGNAL(triggered()), this, SLOT(saveProjectAs()));
	
	_selectSimulationAction= new QAction(QIcon(":share/cassandra/icons/simulation_select.png"), tr("&Select Simulation..."), this);
	_selectSimulationAction->setStatusTip(tr("Select simulation to load"));
	connect(_selectSimulationAction, SIGNAL(triggered()), this, SLOT(selectSimulation()));

	_settingsAction = new QAction(QIcon(":share/cassandra/icons/settings.png"), tr("&Settings"), this);
	_settingsAction->setStatusTip(tr("General Settings"));
	connect(_settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

	_quitAction = new QAction(tr("&Quit"), this);
	_quitAction->setShortcut(tr("Ctrl+Q"));
	_quitAction->setStatusTip(tr("Quit the application"));
	connect(_quitAction, SIGNAL(triggered()), this, SLOT(quit()));

	// simulation menu
	_firstStepAction = new QAction(QIcon(":share/cassandra/icons/restart.png"), tr("&Restart"), this);
	_firstStepAction->setShortcut(tr("Ctrl+R"));
	_firstStepAction->setStatusTip(tr("Restart simulation"));
	connect(_firstStepAction, SIGNAL(triggered()), this, SLOT(firstStep()));

	_previousStepAction = new QAction(QIcon(":share/cassandra/icons/previous.png"), tr("&Previous"), this);
	_previousStepAction->setShortcut(tr("Ctrl+P"));
	_previousStepAction->setStatusTip(tr("Previous Step"));
	connect(_previousStepAction, SIGNAL(triggered()), this, SLOT(previousStep()));

	_nextStepAction = new QAction(QIcon(":share/cassandra/icons/next.png"), tr("&Next"), this);
	_nextStepAction->setShortcut(tr("Ctrl+N"));
	_nextStepAction->setStatusTip(tr("Next Step"));
	connect(_nextStepAction, SIGNAL(triggered()), this, SLOT(nextStep()));	
	
	_finalStepAction = new QAction(QIcon(":share/cassandra/icons/final.png"), tr("&Final"), this);
	_finalStepAction->setShortcut(tr("Ctrl+F"));
	_finalStepAction->setStatusTip(tr("Final Step"));
	connect(_finalStepAction, SIGNAL(triggered()), this, SLOT(finalStep()));
	
	_playAction = new QAction(QIcon(":share/cassandra/icons/play.png"), tr("&Play"), this);
	_playAction->setShortcut(tr("Ctrl+Space"));
	_playAction->setStatusTip(tr("Play/Pause"));
	connect(_playAction, SIGNAL(triggered()), this, SLOT(play()));

	// View Menu
	
	_zoomInAction = new QAction(QIcon(":share/cassandra/icons/zoomIn.png"), tr("&Zoom In"), this);
	_zoomInAction->setShortcut(tr("Ctrl++"));
	_zoomInAction->setStatusTip(tr("Zoom In"));
	connect(_zoomInAction, SIGNAL(triggered()), _display2D, SLOT(zoomIn()));
	
	_zoomOutAction = new QAction(QIcon(":share/cassandra/icons/zoomOut.png"), tr("&Zoom Out"), this);
	_zoomOutAction->setShortcut(tr("Ctrl+-"));
	_zoomOutAction->setStatusTip(tr("Zoom Out"));
	connect(_zoomOutAction, SIGNAL(triggered()), _display2D, SLOT(zoomOut()));
	
	_showAgentsAction = new QAction(QIcon(":share/cassandra/icons/agents.png"), tr("&Show Agents"), this);
	_showAgentsAction->setShortcut(tr("Ctrl+A"));
	_showAgentsAction->setStatusTip(tr("Show Agents"));
	connect(_showAgentsAction, SIGNAL(triggered()), _display2D, SLOT(showAgents()));
	
    _show3DAction = new QAction(QIcon(":share/cassandra/icons/3dview.png"), tr("&Raster 3D"), this);
	_show3DAction->setShortcut(tr("Ctrl+3"));
	_show3DAction->setStatusTip(tr("Show 3D raster"));
	connect(_show3DAction, SIGNAL(triggered()), this, SLOT(show3DWindow()));

	_showLabAction = new QAction(QIcon(":share/cassandra/icons/lab.png"), tr("&Laboratory"), this);
	_showLabAction->setStatusTip(tr("Open Laboratory"));
	connect(_showLabAction, SIGNAL(triggered()), this, SLOT(showLaboratory()));

	_agentAnalysisAction = new QAction(QIcon(":share/cassandra/icons/agentAnalysis.png"), tr("&Agent Analysis"), this);
	_agentAnalysisAction->setStatusTip(tr("Analyse agent stats"));
	connect(_agentAnalysisAction, SIGNAL(triggered()), this, SLOT(showAgentAnalysis()));

	_rasterAnalysisAction = new QAction(QIcon(":share/cassandra/icons/rasterAnalysis.png"), tr("&Raster Analysis"), this);
	_rasterAnalysisAction->setStatusTip(tr("Analyse raster stats"));
	connect(_rasterAnalysisAction, SIGNAL(triggered()), this, SLOT(showRasterAnalysis()));

    _screenshotAction = new QAction(QIcon(":share/cassandra/icons/screenshot.png"), tr("Take &Screenshot"), this);
	_screenshotAction->setStatusTip(tr("Take a screenshot of the model"));
	connect(_screenshotAction, SIGNAL(triggered()), this, SLOT(takeScreenshot()));

    _videoAction = new QAction(QIcon(":share/cassandra/icons/video.png"), tr("Take &Video"), this);
	_videoAction->setStatusTip(tr("Take a video of the model"));
	connect(_videoAction, SIGNAL(triggered()), this, SLOT(takeVideo()));

    _mosaicAction = new QAction(QIcon(":share/cassandra/icons/mosaic.png"), tr("Take &Mosaic"), this);
	_mosaicAction->setStatusTip(tr("Take a mosaic of different runs of the model"));
	connect(_mosaicAction, SIGNAL(triggered()), this, SLOT(takeMosaic()));

    _heatMapAction = new QAction(QIcon(":share/cassandra/icons/heatmap.png"), tr("Create heat map"), this);
	_heatMapAction->setStatusTip(tr("Create a heat map with multi run data"));
	connect(_heatMapAction, SIGNAL(triggered()), this, SLOT(heatMap()));
    _timeSeriesAction = new QAction(QIcon(":share/cassandra/icons/ts.png"), tr("Create time series"), this);
	_timeSeriesAction->setStatusTip(tr("Explore time series with multi run data"));
	connect(_timeSeriesAction, SIGNAL(triggered()), this, SLOT(timeSeries()));


	// menus
	QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(_newProjectAction);
	fileMenu->addAction(_loadProjectAction);
	fileMenu->addAction(_saveProjectAction);
	fileMenu->addAction(_saveProjectAsAction);
	fileMenu->addAction(_selectSimulationAction);
	fileMenu->addAction(_settingsAction);
	fileMenu->addAction(_quitAction);

	QMenu * simulationMenu = menuBar()->addMenu(tr("&Simulation"));
	simulationMenu->addAction(_firstStepAction);
	simulationMenu->addAction(_previousStepAction);
	simulationMenu->addAction(_nextStepAction);
	simulationMenu->addAction(_finalStepAction);
	simulationMenu->addSeparator();
	simulationMenu->addAction(_playAction);

	QMenu * viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addSeparator();
	viewMenu->addAction(_zoomOutAction);
	viewMenu->addAction(_zoomInAction);
	viewMenu->addAction(_showAgentsAction);
	viewMenu->addSeparator();
	viewMenu->addAction(_show3DAction);
	viewMenu->addAction(_showLabAction);

	QMenu * analysisMenu = menuBar()->addMenu(tr("&Analysis"));
	analysisMenu->addSeparator();
	analysisMenu->addAction(_agentAnalysisAction);
	analysisMenu->addAction(_rasterAnalysisAction);
    
    QMenu * outputMenu = menuBar()->addMenu(tr("&Output"));
	outputMenu->addSeparator();
	outputMenu->addAction(_screenshotAction);
	outputMenu->addAction(_videoAction);
	outputMenu->addAction(_mosaicAction);

    QMenu * edaMenu = menuBar()->addMenu(tr("Exploratory Data Analysis"));
    edaMenu->addSeparator();
    edaMenu->addAction(_heatMapAction);
    edaMenu->addAction(_timeSeriesAction);

	// toolbars
	QToolBar * fileBar= addToolBar(tr("File"));
	fileBar->addAction(_newProjectAction);
	fileBar->addAction(_loadProjectAction);
	fileBar->addAction(_saveProjectAction);
	fileBar->addAction(_saveProjectAsAction);
	fileBar->addAction(_selectSimulationAction);
	
	QToolBar * simulationBar = addToolBar(tr("Simulation Controls"));
	simulationBar->addAction(_firstStepAction);
	simulationBar->addAction(_previousStepAction);

	QToolBar * analysisBar = addToolBar(tr("Analysis"));
	analysisBar->addAction(_agentAnalysisAction);
	analysisBar->addAction(_rasterAnalysisAction);

	QToolBar * outputBar = addToolBar(tr("Output"));
	outputBar->addAction(_screenshotAction);
	outputBar->addAction(_videoAction);
	outputBar->addAction(_mosaicAction);	
    
    QToolBar * edaBar = addToolBar(tr("EDA"));
	edaBar->addAction(_heatMapAction);
	edaBar->addAction(_timeSeriesAction);
	
	QLabel * label = new QLabel("Step: ");
	simulationBar->addWidget(label);
	
	_stepBox = new QSpinBox();
	connect(_stepBox, SIGNAL(valueChanged(int)), this, SLOT(viewedStepChanged(int)));
	connect(this, SIGNAL(stepChangeToStepBox(int)), _stepBox, SLOT(setValue(int)));
	simulationBar->addWidget(_stepBox);
	
	QLabel * label2 = new QLabel(" of ");
	simulationBar->addWidget(label2);
	
	_numStepsLabel = new QLabel();
	_numStepsLabel->setNum(0);
	simulationBar->addWidget(_numStepsLabel);
	simulationBar->addAction(_nextStepAction);
	simulationBar->addAction(_finalStepAction);
	simulationBar->addAction(_playAction);

	QToolBar * viewBar = addToolBar(tr("View Options"));
	viewBar->addAction(_zoomOutAction);			
	viewBar->addAction(_zoomInAction);
	viewBar->addAction(_showAgentsAction);
	viewBar->addSeparator();
	viewBar->addAction(_show3DAction);
	viewBar->addAction(_showLabAction);
	
	// TODO un thread diferent?
	_playTimer = new QTimer(this);
	_playTimer->setInterval(10);
	connect(_playTimer, SIGNAL(timeout()), this, SLOT(updatePlay()));

	_loadSimulationTimer = new QTimer(this);
	_loadSimulationTimer->setInterval(100);

	_stepBox->setEnabled(false);
	
	_firstStepAction->setEnabled(false);
	_previousStepAction->setEnabled(false);
	_nextStepAction->setEnabled(false);
	_finalStepAction->setEnabled(false);
	_playAction->setEnabled(false);

	_zoomInAction->setEnabled(false);
	_zoomOutAction->setEnabled(false);
	_showAgentsAction->setEnabled(false);
	_show3DAction->setEnabled(false);
	_showLabAction->setEnabled(true);

	_saveProjectAction->setEnabled(false);
	_saveProjectAsAction->setEnabled(false);

    _screenshotAction->setEnabled(false);
    _videoAction->setEnabled(false);

	_progressBar = new LoadingProgressBar();
	connect(_loadSimulationTimer, SIGNAL(timeout()), _progressBar, SLOT(updateProgress()));
	connect(&_loadSimulationThread, SIGNAL(simulationLoaded(bool)), this, SLOT(loadSimulationFinished(bool)));
	resize(1920, 1240);
}

MainWindow::~MainWindow()
{
    if(_display3D!=0)
	{
		delete _display3D;
	}
	if(_progressBar!=0)
	{
		delete _progressBar;
	}
}

void MainWindow::selectSimulation( const std::string & fileName, int resolution )
{
	ProjectConfiguration::instance()->setResolution(resolution);
    ProjectConfiguration::instance()->setSimulationFileName(fileName);

	setEnabled(false);
	setUpdatesEnabled(false);

	QRect windowSize(geometry());
	_progressBar->move(windowSize.x()+(windowSize.width()-_progressBar->geometry().width())/2, windowSize.y()+(windowSize.height()-_progressBar->geometry().height())/2);
	_progressBar->initLoading();

	_loadSimulationThread.start();
	_loadSimulationTimer->start();

}

void MainWindow::selectSimulation()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Simulation"), "", tr("Simulation Record (*.h5);;All Files (*)"));
	if (fileName.isEmpty())
	{
		return;
	}
    // ask for step resolution
	bool resolutionCorrect;
	int resolution = QInputDialog::getInt(this, tr("Select time step resolution"),tr("Resolution Step:"), 1, 1, 1000, 1, &resolutionCorrect);
	if(!resolutionCorrect)
	{
		resolution = 1;
	}
	repaint();
    selectSimulation(fileName.toStdString(), resolution);
}

void MainWindow::adjustGUI()
{
	std::cout << "adjusting GUI" << std::endl;

    _display2D->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());
    _display3D->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());
	_agentTypeSelection->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());
	_agentTraitSelection->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());
	_rasterSelection->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());
    _genericStatistics->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());

	if(!ProjectConfiguration::instance()->getSimulationRecord())
	{	
		_stepBox->setEnabled(false);

		_firstStepAction->setEnabled(false);
		_previousStepAction->setEnabled(false);
		_nextStepAction->setEnabled(false);
		_finalStepAction->setEnabled(false);
		_playAction->setEnabled(false);

		_zoomInAction->setEnabled(false);
		_zoomOutAction->setEnabled(false);
		_showAgentsAction->setEnabled(false);
		_show3DAction->setEnabled(false);
		
		_saveProjectAction->setEnabled(false);
		_saveProjectAsAction->setEnabled(false);

		_screenshotAction->setEnabled(false);
		_videoAction->setEnabled(false);

		return;
	}
	_stepBox->setValue(0);
	_stepBox->setRange(0, ProjectConfiguration::instance()->getSimulationRecord()->getNumSteps());
	_stepBox->setSingleStep(ProjectConfiguration::instance()->getResolution());

	_numStepsLabel->setNum(ProjectConfiguration::instance()->getSimulationRecord()->getNumSteps());	

	_stepBox->setEnabled(true);
	
	_firstStepAction->setEnabled(true);
	_previousStepAction->setEnabled(true);
	_nextStepAction->setEnabled(true);
	_finalStepAction->setEnabled(true);
	_playAction->setEnabled(true);

	_zoomInAction->setEnabled(true);
	_zoomOutAction->setEnabled(true);
	_showAgentsAction->setEnabled(true);
	_show3DAction->setEnabled(true);

	_saveProjectAction->setEnabled(true);
	_saveProjectAsAction->setEnabled(true);

    _screenshotAction->setEnabled(true);
    _videoAction->setEnabled(true);
	
	_settings->setRadiusSelection(3);

	update();
}

void MainWindow::quit()
{
	close();
	//_Raster3D.close();
}

void MainWindow::firstStep()
{
	setViewedStep(0);
}

void MainWindow::previousStep()
{
	if(_viewedStep>0)
	{
		setViewedStep(_viewedStep-ProjectConfiguration::instance()->getSimulationRecord()->getSerializedResolution()*ProjectConfiguration::instance()->getResolution());
	}
}

void MainWindow::nextStep()
{
	if(_viewedStep<ProjectConfiguration::instance()->getSimulationRecord()->getNumSteps())
	{
		setViewedStep(_viewedStep+ProjectConfiguration::instance()->getSimulationRecord()->getSerializedResolution()*ProjectConfiguration::instance()->getResolution());
	}
}

void MainWindow::finalStep()
{
	setViewedStep(ProjectConfiguration::instance()->getSimulationRecord()->getNumSteps());
}

void MainWindow::play()
{
	if(_playTimer->isActive())
	{
		_playTimer->stop();
	}
	else
	{
		_playTimer->start();
	}
}

void MainWindow::updatePlay()
{
	setViewedStep(_viewedStep+ProjectConfiguration::instance()->getSimulationRecord()->getSerializedResolution()*ProjectConfiguration::instance()->getResolution());
	if(_viewedStep>=ProjectConfiguration::instance()->getSimulationRecord()->getNumSteps())
	{
		_playTimer->stop();
	}
}

void MainWindow::setViewedStep( int viewedStep )
{
	emit stepChangeToStepBox(viewedStep);
}

void MainWindow::viewedStepChanged( int viewedStep )
{
	int mod = viewedStep%(ProjectConfiguration::instance()->getSimulationRecord()->getSerializedResolution()*ProjectConfiguration::instance()->getResolution());
	/*
	if(mod!=0)
	{	
		emit stepChangeToStepBox(viewedStep);
		return;
	}
	*/
	_viewedStep = viewedStep - mod;
	emit newViewedStep(_viewedStep);
}
	
void MainWindow::openAgentConfigurator(QListWidgetItem * item)
{
	std::string type(item->text().toStdString());
	AgentConfigurator * agentConfigurator = new AgentConfigurator(this, type);
	connect(agentConfigurator, SIGNAL(agentConfigured(const std::string &, const AgentConfiguration &)), this, SLOT(agentConfigured(const std::string &, const AgentConfiguration &)));
}

void MainWindow::openRasterConfigurator(QListWidgetItem * item)
{
	std::string type(item->text().toStdString());
	RasterConfigurator * rasterConfigurator = new RasterConfigurator(this, type, _rasterSelection->getRasterList());
	connect(rasterConfigurator, SIGNAL(rasterConfigured(const std::string &, const RasterConfiguration &)), this, SLOT(rasterConfigured(const std::string &, const RasterConfiguration &)));
}

void MainWindow::agentConfigured( const std::string & type, const AgentConfiguration & config )
{
    ProjectConfiguration::instance()->updateAgentConfig(type, config);
}
	
void MainWindow::rasterConfigured( const std::string & type, const RasterConfiguration & config )
{	
	ProjectConfiguration::instance()->updateRasterConfig(type, config);
	_display3D->updateRasterConfig();
}

void MainWindow::show3DWindow()
{
    if(_display3D->isVisible())
    {
        _display3D->hide();
    }
    else
    {
        _display3D->show();
    }
}

void MainWindow::showLaboratory()
{
    if(_laboratory->isVisible())
    {
        _laboratory->hide();
    }
    else
    {
        _laboratory->show();
    }
}

void MainWindow::showAgentAnalysis()
{
    if(_agentAnalysis->isVisible())
    {
        _agentAnalysis->hide();
    }
    else
    {
        _agentAnalysis->show();
    }
}

void MainWindow::showRasterAnalysis()
{
    if(_rasterAnalysis->isVisible())
    {
        _rasterAnalysis->hide();
    }
    else
    {
        _rasterAnalysis->show();
    }
}

void MainWindow::showSettings()
{
	_settings->setRadiusSelection(_display2D->getRadiusSelection());
	_settings->show();
}


void MainWindow::newProject()
{
	ProjectConfiguration::instance()->reset();
}

void MainWindow::loadProject()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Cassandra Project"), "", tr("Cassandra Project (*.cas);;All Files (*)"));
	if (fileName.isEmpty())
	{
		return;
	}
	else
	{
		ProjectConfiguration::instance()->loadProject(fileName.toStdString());
		setEnabled(false);

		QRect windowSize(geometry());
		_progressBar->move(windowSize.x()+(windowSize.width()-_progressBar->geometry().width())/2, windowSize.y()+(windowSize.height()-_progressBar->geometry().height())/2);
	
		adjustGUI();
		_progressBar->initLoading();
		_loadSimulationThread.start();
		_loadSimulationTimer->start();
	}
}

void MainWindow::saveProject()
{
	if(ProjectConfiguration::instance()->alreadySaved())
	{
		ProjectConfiguration::instance()->storeProject();
		return;
	}
	saveProjectAs();
}

void MainWindow::saveProjectAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Project"), "", tr("Cassandra Project (*.cas);;All Files (*)"));
	if(fileName.isEmpty())
	{
		return;
	}
	else
	{
		ProjectConfiguration::instance()->storeProject(fileName.toStdString());
	}
}
	
void MainWindow::loadSimulationFinished( bool correct )
{
	_progressBar->hide();
	_loadSimulationTimer->stop();
	setEnabled(true);
	setUpdatesEnabled(true);
	adjustGUI();
	if(!correct)
	{
		QMessageBox::warning(0, "Cassandra", "Error trying to load simulation");
	}
}

void MainWindow::show3Dagent(QTreeWidgetItem * item, int )
{
    if (item->child(0) != 0);
    else
    {
        Engine::AgentRecord* agentSelected = 0;
        std::list<Engine::AgentRecord*>::iterator it = agentsSelected.begin();
        while(it != agentsSelected.end())
        {
			Engine::AgentRecord * agentAux = *it;
            if (agentAux->getId().compare(item->parent()->text(0).toStdString())==0)
			{
				agentSelected = agentAux;
			}
            it++;
        }
		if(!agentSelected)
		{
			return;
		}

        Display3D * displayAgent = new Display3D(0);
        connect(this, SIGNAL(newViewedStep(int)), displayAgent, SLOT(viewedStepChangedSlot(int)));
        connect(_rasterSelection, SIGNAL(rastersRearranged(std::list<std::string>)), displayAgent, SLOT(rastersRearranged(std::list<std::string>)));
        displayAgent->setWindowTitle(item->parent()->text(0));

        displayAgent->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());
        displayAgent->rastersRearranged(_rasterSelection->getRasterList(), _rasterSelection->getRasterView());
        displayAgent->setAgentFocus(agentSelected);
		displayAgent->show();
    }
}

void MainWindow::updateAgentsSelected(std::list<Engine::AgentRecord*> agents,Engine::SimulationRecord * sim)
{
    agentsSelected.clear();
    agentsSelected = agents;
    _simulationRecord = sim;
}

void MainWindow::takeScreenshot()
{
    if(_display2D->getRealSize().width()==0)
    {
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Screenshot"), "", tr("Image Files (*.png *.jpg *.bmp)"));
	if(fileName.isEmpty())
	{
		return;
	}
    _display2D->resetView();
    QImage img(_display2D->getRealSize(), QImage::Format_RGB16);
    QPainter painter(&img);
    _display2D->render(&painter);
    img.save(fileName);
}

void MainWindow::takeVideo( const std::string & outputDir )
{ 
    int incrementStep = ProjectConfiguration::instance()->getSimulationRecord()->getSerializedResolution()*ProjectConfiguration::instance()->getResolution();
    int finalStep = ProjectConfiguration::instance()->getSimulationRecord()->getNumSteps();
    
    _display2D->resetView();
    QImage img(_display2D->getRealSize(), QImage::Format_RGB16);
    QPainter painter(&img);

    for(int i=0; i<=finalStep; i=i+incrementStep)
    {
        _display2D->setViewedStep(i);
        _display2D->render(&painter);
        img.save(QString::fromUtf8(outputDir.c_str())+"/step_"+QString("%1").arg(i, 8, 10, QChar('0')).toUpper()+".png");
    }
    update();

}

void MainWindow::takeVideo()
{
    if(_display2D->getRealSize().width()==0)
    {
        return;
    }
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Dir to Store a screenshot for each time step"), "");
	if(dir.isEmpty())
	{
		return;
	}	
   
    std::string outputDir = dir.toStdString();
    if(boost::filesystem::exists(outputDir) && !boost::filesystem::is_empty(outputDir))
	{
		QMessageBox::StandardButton button = QMessageBox::question(0, "Directory exists", QString("Directory \"").append(QString(outputDir.c_str()).append("\" is not empty; do you want to overwrite it?")), QMessageBox::Yes | QMessageBox::No);
		if(button==QMessageBox::No)
		{
			return;
		}
		boost::filesystem::remove_all(outputDir);
	}
	boost::filesystem::create_directory(outputDir);
    takeVideo(outputDir);
}

void MainWindow::takeMosaic()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Dir with runs to create a mosaic"), "");
	if(dir.isEmpty())
	{
		return;
	}
    std::string baseDir = dir.toStdString();

    dir = QFileDialog::getExistingDirectory(this, tr("Select Dir to Store the mosaic"), "");
	if(dir.isEmpty())
	{
		return;
	}	
    std::string outputDir = dir.toStdString();
    if(boost::filesystem::exists(outputDir) && !boost::filesystem::is_empty(outputDir))
	{
		QMessageBox::StandardButton button = QMessageBox::question(0, "Directory exists", QString("Directory \"").append(QString(outputDir.c_str()).append("\" is not empty; do you want to overwrite it?")), QMessageBox::Yes | QMessageBox::No);
		if(button==QMessageBox::No)
		{
			return;
		}
		boost::filesystem::remove_all(outputDir);
	}
	boost::filesystem::create_directory(outputDir);

    int numberOfSimulations = 0;
    // compute number of simulations to analyse
	for( boost::filesystem::directory_iterator it(baseDir); it!=boost::filesystem::directory_iterator(); it++ )
	{
		if(!boost::filesystem::is_directory(it->status()))
		{
			continue;
		}
		std::stringstream oss;
		oss << (*it).path().native() << "/config.xml";
		if(boost::filesystem::exists(oss.str()))
		{
			numberOfSimulations++;
		}
	}
    int numRows = 8;
    for(;numRows>0;numRows--)
    {
        if(numRows*numRows<=numberOfSimulations)
        {
            break;
        }
    }
	std::cout << "num sims: " << numberOfSimulations << " creating mosaic of: " << numRows << "x" << numRows << std::endl;

    int i=0;
	for( boost::filesystem::directory_iterator it(baseDir); it!=boost::filesystem::directory_iterator(); it++ )
	{
        if(i>=numRows*numRows)
        {
            break;
        }
        i++;
		std::cout << "next sim" << std::endl;
		if(!boost::filesystem::is_directory(it->status()))
		{
			continue;
		}
		std::stringstream oss;
		oss << (*it).path().native() << "/config.xml";	
		TiXmlDocument doc(oss.str().c_str());
		if (!doc.LoadFile())
		{
			QMessageBox msgBox;
			msgBox.setText("Unable to open config file in dir: "+QString(oss.str().c_str()));
			msgBox.exec();
			continue;
		}
		TiXmlHandle hDoc(&doc);
		TiXmlHandle hRoot(0);
    
		TiXmlElement * root = doc.FirstChildElement( "config" );
		TiXmlElement * output = root->FirstChildElement("output");
		std::string dataFile = (*it).path().native()+"/"+output->Attribute("resultsFile");

		boost::filesystem::path pathToDir = *it;
		std::cout << "creating series of screenshots for : " << dataFile << std::endl;

        // load simulation
        ProjectConfiguration::instance()->setResolution(1);
        ProjectConfiguration::instance()->setSimulationFileName(dataFile);
        ProjectConfiguration::instance()->loadSimulation();
        adjustGUI();
        std::stringstream dir;
        dir << outputDir << "/shots_" << i << "/";
	    boost::filesystem::create_directory(dir.str());
        takeVideo(dir.str());
        std::cout << " done for: " << dataFile << " folder: " << dir.str() << std::endl;
    }
    std::cout << "done!" << std::endl;
}

void MainWindow::heatMap()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select file with grouped results"), "", tr("Comma Separated Value (*.csv);;All Files (*)"));
	if (fileName.isEmpty())
	{
		return;
	}

    HeatMapDialog * dialog = new HeatMapDialog(0, fileName.toStdString());
    dialog->show();
    dialog->update();
}  

void MainWindow::timeSeries()
{    
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select file with grouped results"), "", tr("Comma Separated Value (*.csv);;All Files (*)"));
	if (fileName.isEmpty())
	{
		return;
	}

    TimeSeriesDialog * dialog = new TimeSeriesDialog(0, fileName.toStdString());
    dialog->show();
    dialog->update();

}


} // namespace GUI


