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

#include <ProjectConfiguration.hxx>

#include <SimulationRecord.hxx>
#include <AgentConfiguration.hxx>
#include <RasterConfiguration.hxx>
#include <Exception.hxx>
#include <ColorSelector.hxx>
#include <ColorInterval.hxx>

#include <QMessageBox>

#include <tinyxml.h>
#include <iostream>

namespace GUI
{

ProjectConfiguration * ProjectConfiguration::_instance = 0;

ProjectConfiguration * ProjectConfiguration::instance()
{
	if(!_instance)
	{
		_instance = new ProjectConfiguration();
	}
	return _instance;
}

ProjectConfiguration::ProjectConfiguration() : _simulationRecord(0), _projectCreated(false)
{
	reset();
}

ProjectConfiguration::~ProjectConfiguration()
{
	if(_simulationRecord)
	{
		delete _simulationRecord;
	}
	cleanConfigs();
}

void ProjectConfiguration::cleanSimulationRecord()
{
	if(_simulationRecord)
	{
		delete _simulationRecord;
		_simulationRecord = 0;
	}
	cleanConfigs();

}

void ProjectConfiguration::reset()
{
	_projectCreated = false;
	_fileName = "unknown.cas";
	_simulationFileName = "unknown.h5";
	cleanSimulationRecord();
}

void ProjectConfiguration::loadProject( const std::string & fileName )
{
	reset();
	_fileName = fileName;
	_projectCreated = true;

	
	TiXmlDocument fileProject;
	bool loadOkay = fileProject.LoadFile(_fileName);
	if(!loadOkay)
	{
		QMessageBox::information(0, "Failed to open project file", _fileName.c_str());
		return;
	}

	TiXmlElement * simulation = fileProject.FirstChildElement("simulation");
	_simulationFileName = simulation->Attribute("execution");
	_resolution = atoi(simulation->Attribute("resolution"));

	loadAgentsConfigs(simulation->FirstChildElement("agents"));
	loadRastersConfigs(simulation->FirstChildElement("rasters"));

}

bool ProjectConfiguration::loadSimulation()
{
	if(!_projectCreated)
	{
		cleanSimulationRecord();
	}
	else
	{
		if(_simulationRecord)
		{
			delete _simulationRecord;
			_simulationRecord = 0;
		}
	}
	_simulationRecord = new Engine::SimulationRecord(_resolution);

	if(!_simulationRecord->loadHDF5(_simulationFileName))
	{
		delete _simulationRecord;
		_simulationRecord = 0;
		return false;
	}

	// if project was not already created we need to create configs again
	if(!_projectCreated)
	{
		loadConfigs();
	}
	return true;
}

void ProjectConfiguration::storeProject( const std::string & fileName )
{
	_fileName = fileName;
	// if extension ".cas" is not present, append it.
	if(_fileName.find(".cas")==std::string::npos)
	{
		_fileName.append(".cas");
	}
	storeProject();
}

void ProjectConfiguration::loadAgentsConfigs( TiXmlElement * agents )
{
	TiXmlElement * agent = agents->FirstChildElement();
	while(agent)
	{
		AgentConfiguration * agentConfig = new AgentConfiguration();

		TiXmlElement * config2d = agent->FirstChildElement("shape2D");
		TiXmlElement * config3d = agent->FirstChildElement("shape3D");

		agentConfig->setSize( atof(config2d->Attribute("size")));
		agentConfig->setUseIcon( atoi(config2d->Attribute("useIcon")));
		agentConfig->showValue( atoi(config2d->Attribute("showValue")));
		agentConfig->setFileName2D( config2d->Attribute("iconPath"));
		agentConfig->setColor(QColor(config2d->Attribute("color")));

		agentConfig->setFileName3D( config3d->Attribute("model3D"));		
		Engine::Point3D<float> size3d(atof(config3d->Attribute("x3D")), atof(config3d->Attribute("y3D")), atof(config3d->Attribute("z3D")));
		agentConfig->setSize3D( size3d );
		
		_agentsConfig.insert(make_pair(std::string(agent->Attribute("type")), agentConfig));
		agent = agent->NextSiblingElement();
	}
}

void ProjectConfiguration::loadRastersConfigs( TiXmlElement * rasters )
{
	std::list<std::string> rastersNames;
	
	TiXmlElement * raster = rasters->FirstChildElement();
	while(raster)
	{
		// create a raster without initialization
		int rasterMinValue = atoi(raster->Attribute("minRaster"));
		int rasterMaxValue = atoi(raster->Attribute("maxRaster"));		
		RasterConfiguration * rasterConfig = new RasterConfiguration( rasterMinValue, rasterMaxValue, false);
		rasterConfig->setTransparentValue( atoi(raster->Attribute("transparentValue")));
		rasterConfig->setTransparentEnabled( atoi(raster->Attribute("transparentEnabled")));

		ColorSelector & selector = rasterConfig->getColorRamp();
		TiXmlElement * colorRamp = raster->FirstChildElement("colorRamp");
		TiXmlElement * colorInterval = colorRamp->FirstChildElement();

		int index = 0;
		while(colorInterval)
		{
			int breakStep = atoi(colorInterval->Attribute("break"));
			QColor color = QColor(colorInterval->Attribute("color"));

			if(breakStep!=rasterMinValue && breakStep!=rasterMaxValue)
			{
				selector.addBreak(breakStep);
				index++;
			}
			selector.setColor(color, index);
			colorInterval = colorInterval->NextSiblingElement();
		}
		_rastersConfig.insert(make_pair( std::string(raster->Attribute("name")), rasterConfig));
		rastersNames.push_back(raster->Attribute("name"));
		raster = raster->NextSiblingElement();
	}
}

TiXmlElement * ProjectConfiguration::storeAgentsConfigs()
{
	TiXmlElement * agents = new TiXmlElement("agents");
	for(AgentsConfigurationMap::iterator it=_agentsConfig.begin(); it!=_agentsConfig.end(); it++)
	{
		AgentConfiguration * config = it->second;
		TiXmlElement * currentAgent = new TiXmlElement("agent");
		currentAgent->SetAttribute("type", it->first);

		TiXmlElement * config2d = new TiXmlElement("shape2D");
		config2d->SetDoubleAttribute("size", config->getSize());
		config2d->SetAttribute("color", config->getColor().name().toStdString());
		config2d->SetAttribute("iconPath", config->getFileName2D());
		config2d->SetAttribute("useIcon", config->useIcon());
		config2d->SetAttribute("showValue", config->showValue());

		TiXmlElement * config3d = new TiXmlElement("shape3D");
		config3d->SetAttribute("model3D", config->getFileName3D());
		config3d->SetDoubleAttribute("x3D", config->getSize3D()._x);
		config3d->SetDoubleAttribute("y3D", config->getSize3D()._y);
		config3d->SetDoubleAttribute("z3D", config->getSize3D()._z);

		currentAgent->LinkEndChild(config2d);
		currentAgent->LinkEndChild(config3d);
		agents->LinkEndChild(currentAgent);
	}
	return agents;
}

TiXmlElement * ProjectConfiguration::storeRastersConfigs()
{
	TiXmlElement * rasters = new TiXmlElement("rasters");
	for(RastersConfigurationMap::iterator it=_rastersConfig.begin(); it!=_rastersConfig.end(); it++)
	{
		RasterConfiguration * config = it->second;
		TiXmlElement * currentRaster = new TiXmlElement("raster");
		currentRaster->SetAttribute("name", it->first);
		currentRaster->SetAttribute("transparentValue", config->getTransparentValue());
		currentRaster->SetAttribute("transparentEnabled", config->isTransparentEnabled());
		currentRaster->SetAttribute("minRaster", config->getMinValue());
		currentRaster->SetAttribute("maxRaster", config->getMaxValue());

		TiXmlElement * colorRamp = new TiXmlElement("colorRamp");
		ColorSelector & selector = config->getColorRamp();
		for(unsigned int i=0; i<selector.getNumIntervals(); i++)
		{
			int breakStep = selector.getBreak(i);
			TiXmlElement * colorInterval = new TiXmlElement("colorInterval");
			colorInterval->SetAttribute("break", breakStep);
			colorInterval->SetAttribute("color", selector.getColor(breakStep).name().toStdString());
			colorRamp->LinkEndChild(colorInterval);
		}
		currentRaster->LinkEndChild(colorRamp);
		rasters->LinkEndChild(currentRaster);
	}
	return rasters;
}

void ProjectConfiguration::storeProject()
{
	TiXmlDocument fileProject;
	TiXmlDeclaration * declaration = new TiXmlDeclaration( "1.0", "", "" );
	TiXmlElement * simulation = new TiXmlElement( "simulation" );
	simulation->SetAttribute("execution", _simulationFileName);
	simulation->SetAttribute("resolution", _resolution);

	simulation->LinkEndChild(storeAgentsConfigs());
	simulation->LinkEndChild(storeRastersConfigs());

	// store agents configs
	// store rasters configs
	
	fileProject.LinkEndChild( declaration );
	fileProject.LinkEndChild( simulation );
	fileProject.SaveFile(_fileName);
	_projectCreated = true;
}

void ProjectConfiguration::setResolution( const int & resolution )
{
	_resolution = resolution;
}

int ProjectConfiguration::getResolution()
{
	return _resolution;
}

void ProjectConfiguration::setSimulationFileName( const std::string & simulationFileName )
{
	_simulationFileName = simulationFileName;
}

const std::string & ProjectConfiguration::getSimulationFileName()
{
	return _simulationFileName;
}

bool ProjectConfiguration::alreadySaved()
{
	return _fileName.compare("unknown.cas");
}

Engine::SimulationRecord * ProjectConfiguration::getSimulationRecord()
{
	return _simulationRecord;
}

void ProjectConfiguration::cleanConfigs()
{
	for(AgentsConfigurationMap::iterator it=_agentsConfig.begin(); it!=_agentsConfig.end(); it++)
	{
		AgentConfiguration * agentConfig = it->second;
		delete agentConfig;
		it->second = 0;
	}
	_agentsConfig.clear();

	for(RastersConfigurationMap::iterator it=_rastersConfig.begin(); it!=_rastersConfig.end(); it++)
	{
		RasterConfiguration * rasterConfig = it->second;
		delete rasterConfig;
		it->second = 0;
	}
	_rastersConfig.clear();
}

void ProjectConfiguration::loadConfigs()
{	
	if(_agentsConfig.size()!=0 || _rastersConfig.size()!=0)
	{
		std::stringstream oss;
		oss << "ProjectConfiguration::loadConfigs - trying to load configs without cleaning previous data";
		throw Engine::Exception(oss.str());
		return;
	}

	for(Engine::SimulationRecord::AgentTypesMap::const_iterator itType = _simulationRecord->beginTypes(); itType!=_simulationRecord->endTypes(); itType++)
	{
		_agentsConfig.insert(make_pair( itType->first, new AgentConfiguration() ));
	}	
	
	std::list<std::string> rastersNames;
	for(Engine::SimulationRecord::RasterMap::const_iterator itRaster = _simulationRecord->beginRasters(); itRaster!=_simulationRecord->endRasters(); itRaster++)
	{
		int minValue = itRaster->second[0].getMinValue();
		int maxValue = itRaster->second[0].Engine::StaticRaster::getMaxValue();
		std::cout << itRaster->first << " dynamic, min: " << minValue << " max: " << maxValue << std::endl;
		_rastersConfig.insert(make_pair( itRaster->first, new RasterConfiguration(minValue,maxValue) ));
		rastersNames.push_back(itRaster->first);
	}

	for(Engine::SimulationRecord::StaticRasterMap::const_iterator itRaster = _simulationRecord->beginStaticRasters(); itRaster!=_simulationRecord->endStaticRasters(); itRaster++)
	{
		int minValue = itRaster->second.getMinValue();
		int maxValue = itRaster->second.getMaxValue();
		std::cout << itRaster->first << " static, min: " << minValue << " max: " << maxValue << std::endl;
		_rastersConfig.insert(make_pair( itRaster->first, new RasterConfiguration(minValue,maxValue)));
		rastersNames.push_back(itRaster->first);
	}
}

void ProjectConfiguration::updateAgentConfig( const std::string & type, const AgentConfiguration & config )
{
	AgentsConfigurationMap::iterator it = _agentsConfig.find(type);
	if(it==_agentsConfig.end())
	{
		std::stringstream oss;
		oss << "ProjectConfiguration::agentConfigured - config not found for agent type: " << type;
		throw Engine::Exception(oss.str());
		return;
	}
	else
	{
		AgentConfiguration * agentConfig = it->second;
		delete agentConfig;
		it->second = new AgentConfiguration(config);
	}
}

void ProjectConfiguration::updateRasterConfig( const std::string & type, const RasterConfiguration & config )
{
	RastersConfigurationMap::iterator it = _rastersConfig.find(type);
	if(it==_rastersConfig.end())
	{
		std::stringstream oss;
		oss << "ProjectConfiguration::rasterConfigured - config not found for raster type: " << type;
		throw Engine::Exception(oss.str());
		return;
	}
	else
	{
		RasterConfiguration * rasterConfig = it->second;
		delete rasterConfig;
		it->second = new RasterConfiguration(config);
	}
}

AgentConfiguration * ProjectConfiguration::getAgentConfig( const std::string & type )
{
	AgentsConfigurationMap::iterator it = _agentsConfig.find(type);
	if(it==_agentsConfig.end())
	{
		std::stringstream oss;
		oss << "ProjectConfiguration::getAgentConfig - config not found for agent type: " << type; 
		throw Engine::Exception(oss.str());
		return 0;
	}
	return it->second;
}

RasterConfiguration * ProjectConfiguration::getRasterConfig( const std::string & type )
{
	RastersConfigurationMap::iterator it = _rastersConfig.find(type);
	if(it==_rastersConfig.end())
	{
		std::stringstream oss;
		oss << "ProjectConfiguration::getRasterConfig - config not found for raster type: " << type; 
		throw Engine::Exception(oss.str());
		return 0;
	}
	return it->second;
}

} // namespace GUI

