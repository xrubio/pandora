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
#include <Configurator3D.hxx>
#include <Exceptions.hxx>
#include <ProjectConfiguration.hxx>
#include <SimulationRecord.hxx>
#include <LoadingProgressBar.hxx>

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
	
	_display2D->show();
	_display3D->hide();
	
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
	connect(_rasterSelection, SIGNAL(rastersRearranged(std::list<std::string>)), _display2D, SLOT(rastersRearranged(std::list<std::string>)));
	connect(_rasterSelection, SIGNAL(rastersRearranged(std::list<std::string>)), _display3D, SLOT(rastersRearranged(std::list<std::string>)));
	connect(_rasterSelection, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(openRasterConfigurator(QListWidgetItem *)));
	
	// file menu 
	_newProjectAction = new QAction(QIcon(":/resources/icons/project_new.png"), tr("&New Project"), this);
	_newProjectAction->setShortcut(tr("Ctrl+N"));
	_newProjectAction->setStatusTip(tr("Create new project"));
	connect(_newProjectAction, SIGNAL(triggered()), this, SLOT(newProject()));

	_loadProjectAction = new QAction(QIcon(":/resources/icons/project_open.png"), tr("&Load Project..."), this);
	_loadProjectAction->setShortcut(tr("Ctrl+L"));
	_loadProjectAction->setStatusTip(tr("Load project"));
	connect(_loadProjectAction, SIGNAL(triggered()), this, SLOT(loadProject()));

	_saveProjectAction = new QAction(QIcon(":/resources/icons/project_save.png"), tr("&Save Project..."), this);
	_saveProjectAction->setShortcut(tr("Ctrl+S"));
	_saveProjectAction->setStatusTip(tr("Save project"));
	connect(_saveProjectAction, SIGNAL(triggered()), this, SLOT(saveProject()));

	_saveProjectAsAction = new QAction(QIcon(":/resources/icons/project_save_as.png"), tr("&Save Project as..."), this);
	_saveProjectAsAction->setShortcut(tr("Ctrl+Alt+S"));
	_saveProjectAsAction->setStatusTip(tr("Save project as"));
	connect(_saveProjectAsAction, SIGNAL(triggered()), this, SLOT(saveProjectAs()));
	
	_selectSimulationAction= new QAction(QIcon(":/resources/icons/simulation_select.png"), tr("&Select Simulation..."), this);
	_selectSimulationAction->setShortcut(tr("Ctrl+S"));
	_selectSimulationAction->setStatusTip(tr("Select simulation to load"));
	connect(_selectSimulationAction, SIGNAL(triggered()), this, SLOT(selectSimulation()));
   
	_quitAction = new QAction(tr("&Quit"), this);
	_quitAction->setShortcut(tr("Ctrl+Q"));
	_quitAction->setStatusTip(tr("Quit the application"));
	connect(_quitAction, SIGNAL(triggered()), this, SLOT(quit()));

	// simulation menu
	_firstStepAction = new QAction(QIcon(":/resources/icons/restart.png"), tr("&Restart"), this);
	_firstStepAction->setShortcut(tr("Ctrl+R"));
	_firstStepAction->setStatusTip(tr("Restart simulation"));
	connect(_firstStepAction, SIGNAL(triggered()), this, SLOT(firstStep()));

	_previousStepAction = new QAction(QIcon(":/resources/icons/previous.png"), tr("&Previous"), this);
	_previousStepAction->setShortcut(tr("Ctrl+P"));
	_previousStepAction->setStatusTip(tr("Previous Step"));
	connect(_previousStepAction, SIGNAL(triggered()), this, SLOT(previousStep()));

	_nextStepAction = new QAction(QIcon(":/resources/icons/next.png"), tr("&Next"), this);
	_nextStepAction->setShortcut(tr("Ctrl+N"));
	_nextStepAction->setStatusTip(tr("Next Step"));
	connect(_nextStepAction, SIGNAL(triggered()), this, SLOT(nextStep()));	
	
	_finalStepAction = new QAction(QIcon(":/resources/icons/final.png"), tr("&Final"), this);
	_finalStepAction->setShortcut(tr("Ctrl+F"));
	_finalStepAction->setStatusTip(tr("Final Step"));
	connect(_finalStepAction, SIGNAL(triggered()), this, SLOT(finalStep()));
	
	_playAction = new QAction(QIcon(":/resources/icons/play.png"), tr("&Play"), this);
	_playAction->setShortcut(tr("Ctrl+Space"));
	_playAction->setStatusTip(tr("Play/Pause"));
	connect(_playAction, SIGNAL(triggered()), this, SLOT(play()));

	// View Menu
	
	_zoomInAction = new QAction(QIcon(":/resources/icons/zoomIn.png"), tr("&Zoom In"), this);
	_zoomInAction->setShortcut(tr("Ctrl++"));
	_zoomInAction->setStatusTip(tr("Zoom In"));
	connect(_zoomInAction, SIGNAL(triggered()), _display2D, SLOT(zoomIn()));
	
	_zoomOutAction = new QAction(QIcon(":/resources/icons/zoomOut.png"), tr("&Zoom Out"), this);
	_zoomOutAction->setShortcut(tr("Ctrl+-"));
	_zoomOutAction->setStatusTip(tr("Zoom Out"));
	connect(_zoomOutAction, SIGNAL(triggered()), _display2D, SLOT(zoomOut()));
	
	_showAgentsAction = new QAction(QIcon(":/resources/icons/agents.png"), tr("&Show Agents"), this);
	_showAgentsAction->setShortcut(tr("Ctrl+A"));
	_showAgentsAction->setStatusTip(tr("Show Agents"));
	connect(_showAgentsAction, SIGNAL(triggered()), _display2D, SLOT(showAgents()));
	
	_show3DAction = new QAction(QIcon(":/resources/icons/3dview.png"), tr("&Raster 3D"), this);
	_show3DAction->setShortcut(tr("Ctrl+3"));
	_show3DAction->setStatusTip(tr("Show 3D raster"));
	connect(_show3DAction, SIGNAL(triggered()), this, SLOT(show3DWindow()));
	
	_options3DAction = new QAction(QIcon(":/resources/icons/3doptions.png"), tr("&Edit 3D View"), this);
	_options3DAction->setShortcut(tr("Ctrl+E"));
	_options3DAction->setStatusTip(tr("Edit 3D View"));
	connect(_options3DAction, SIGNAL(triggered()), this, SLOT(show3DOptions()));
	
	// menus
	_fileMenu = menuBar()->addMenu(tr("&File"));
	_fileMenu->addAction(_newProjectAction);
	_fileMenu->addAction(_loadProjectAction);
	_fileMenu->addAction(_saveProjectAction);
	_fileMenu->addAction(_saveProjectAsAction);
	_fileMenu->addAction(_selectSimulationAction);
	_fileMenu->addAction(_quitAction);

	_simulationMenu = menuBar()->addMenu(tr("&Simulation"));
	_simulationMenu->addAction(_firstStepAction);
	_simulationMenu->addAction(_previousStepAction);
	_simulationMenu->addAction(_nextStepAction);
	_simulationMenu->addAction(_finalStepAction);
	_simulationMenu->addSeparator();
	_simulationMenu->addAction(_playAction);

	_viewMenu = menuBar()->addMenu(tr("&View"));
	_viewMenu->addSeparator();
	_viewMenu->addAction(_zoomOutAction);
	_viewMenu->addAction(_zoomInAction);
	_viewMenu->addAction(_showAgentsAction);
	_viewMenu->addSeparator();
	_viewMenu->addAction(_show3DAction);
	_viewMenu->addAction(_options3DAction);
	
	// toolbars
	_fileBar= addToolBar(tr("File"));
	_fileBar->addAction(_newProjectAction);
	_fileBar->addAction(_loadProjectAction);
	_fileBar->addAction(_saveProjectAction);
	_fileBar->addAction(_saveProjectAsAction);
	_fileBar->addAction(_selectSimulationAction);
	
	_simulationBar = addToolBar(tr("Simulation Controls"));
	_simulationBar->addAction(_firstStepAction);
	_simulationBar->addAction(_previousStepAction);
	
	QLabel * label = new QLabel("Step: ");
	_simulationBar->addWidget(label);
	
	_stepBox = new QSpinBox();
	connect(_stepBox, SIGNAL(valueChanged(int)), this, SLOT(viewedStepChanged(int)));
	connect(this, SIGNAL(stepChangeToStepBox(int)), _stepBox, SLOT(setValue(int)));
	_simulationBar->addWidget(_stepBox);
	
	QLabel * label2 = new QLabel(" of ");
	_simulationBar->addWidget(label2);
	
	_numStepsLabel = new QLabel();
	_numStepsLabel->setNum(0);
	_simulationBar->addWidget(_numStepsLabel);
	
	
	_simulationBar->addAction(_nextStepAction);
	_simulationBar->addAction(_finalStepAction);
	_simulationBar->addAction(_playAction);

	_viewBar = addToolBar(tr("View Options"));
	_viewBar->addAction(_zoomOutAction);			
	_viewBar->addAction(_zoomInAction);
	_viewBar->addAction(_showAgentsAction);
	_viewBar->addSeparator();
	_viewBar->addAction(_show3DAction);
	_viewBar->addAction(_options3DAction);
	
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
	_options3DAction->setEnabled(false);

	_saveProjectAction->setEnabled(false);
	_saveProjectAsAction->setEnabled(false);

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

void MainWindow::selectSimulation()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Simulation"), "", tr("Simulation Record (*.h5);;All Files (*)"));
	if (fileName.isEmpty())
	{
		return;
	}
	else
	{
		// ask for step resolution
		bool resolutionCorrect;
		int resolution = QInputDialog::getInt(this, tr("Select time step resolution"),tr("Resolution Step:"), 1, 1, 1000, 1, &resolutionCorrect);
		if(!resolutionCorrect)
		{
			resolution = 1;
		}
		repaint();

		ProjectConfiguration::instance()->setResolution(resolution);
		ProjectConfiguration::instance()->setSimulationFileName(fileName.toStdString());

		setEnabled(false);
		setUpdatesEnabled(false);

		QRect windowSize(geometry());
		_progressBar->move(windowSize.x()+(windowSize.width()-_progressBar->geometry().width())/2, windowSize.y()+(windowSize.height()-_progressBar->geometry().height())/2);
		_progressBar->initLoading();

		_loadSimulationThread.start();
		_loadSimulationTimer->start();
	}
}

void MainWindow::adjustGUI()
{
	std::cout << "adjusting GUI" << std::endl;

	_display2D->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());
	_agentTypeSelection->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());
	_agentTraitSelection->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());
	_rasterSelection->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());
	_genericStatistics->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());
	_display3D->setSimulationRecord(ProjectConfiguration::instance()->getSimulationRecord());

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
		_options3DAction->setEnabled(false);	
		
		_saveProjectAction->setEnabled(false);
		_saveProjectAsAction->setEnabled(false);

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
	_options3DAction->setEnabled(true);

	_saveProjectAction->setEnabled(true);
	_saveProjectAsAction->setEnabled(true);

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
		setViewedStep(_viewedStep-ProjectConfiguration::instance()->getResolution());
	}
}

void MainWindow::nextStep()
{
	if(_viewedStep<ProjectConfiguration::instance()->getSimulationRecord()->getNumSteps())
	{
		setViewedStep(_viewedStep+ProjectConfiguration::instance()->getResolution());
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
	setViewedStep(_viewedStep+ProjectConfiguration::instance()->getResolution());
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
	int mod = viewedStep%ProjectConfiguration::instance()->getResolution();
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
	RasterConfigurator * rasterConfigurator = new RasterConfigurator(this, type);
	connect(rasterConfigurator, SIGNAL(rasterConfigured(const std::string &, const RasterConfiguration &)), this, SLOT(rasterConfigured(const std::string &, const RasterConfiguration &)));
}
	
void MainWindow::agentConfigured( const std::string & type, const AgentConfiguration & config )
{
	ProjectConfiguration::instance()->updateAgentConfig(type, config);
}
	
void MainWindow::rasterConfigured( const std::string & type, const RasterConfiguration & config )
{	
	ProjectConfiguration::instance()->updateRasterConfig(type, config);
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

void MainWindow::show3DOptions()
{
	Configurator3D * config = new Configurator3D(this);
	connect(config, SIGNAL(configured3D(const Configuration3D &)), this, SLOT(configured3D(const Configuration3D &)));
}

void MainWindow::configured3D( const Configuration3D & config3D )
{
	ProjectConfiguration::instance()->getConfig3D().setRasters(config3D.getDEMRaster(), config3D.getColorRaster());
	ProjectConfiguration::instance()->getConfig3D().setSize3D(config3D.getSize3D());
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

} // namespace GUI

