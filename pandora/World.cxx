 /*
 * Copyright (c) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software; 
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or (at your option) any later version.
 * 
 * Pandora is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <World.hxx>
#include <Agent.hxx>
#include <Exceptions.hxx>
#include <Scheduler.hxx>
#include <SpacePartition.hxx>
#include <OpenMPSingleNode.hxx>

#include <GeneralState.hxx>

#include <Logger.hxx>
#include <Statistics.hxx>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace Engine
{

World::World( const Simulation & simulation, Scheduler * scheduler, const bool & allowMultipleAgentsPerCell) : _simulation(simulation), _allowMultipleAgentsPerCell(allowMultipleAgentsPerCell), _step(0), _scheduler(scheduler)
{
	// default Scheduler 
	if(!_scheduler)
	{
		_scheduler = useOpenMPSingleNode();
	}
	_scheduler->setWorld(this);
}

World::~World()
{
	AgentsList::iterator it=_agents.begin();
	while(it!=_agents.end())
	{
		Agent * agent = *it;
		it = _agents.erase(it);
		delete agent;
	}
	for(size_t i=0; i<_rasters.size(); i++)
	{
		if(_rasters.at(i))
		{
			delete _rasters.at(i);
		}
	}
	if(_scheduler)
	{
		delete _scheduler;
	}
}

void World::initialize(int argc, char *argv[])
{
	_scheduler->init(argc,argv);

	createRasters();
	createAgents();		
	
	_scheduler->initData();
}


void World::updateRasterToMaxValues( const std::string & key )
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	updateRasterToMaxValues(it->second);
}

void World::updateRasterToMaxValues( const int & index )
{
	((Raster *)_rasters.at(index))->updateRasterToMaxValues();
}

void World::addAgent( Agent * agent, bool executedAgent )
{
	agent->setWorld(this);
	_agents.push_back(agent);
	if(executedAgent)
	{
		_scheduler->agentAdded(agent, executedAgent);
	}
	
	std::stringstream logName;
	logName << "agents_" << getId() << "_" << agent->getId();
	log_EDEBUG( logName.str(), "agent: " << agent << " added at time step: " << getCurrentTimeStep());
}



void World::step()
{
	std::stringstream logName;
	logName << "simulation_" << getId();
	log_INFO(logName.str(), getWallTime() << " executing step: " << _step );

	if(_step%_simulation.getSerializerResolution()==0)
	{
		_scheduler->serializeRasters(_step);
		_scheduler->serializeAgents(_step);
		log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " serialization done");
	}
	stepEnvironment();
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " has executed step environment");
	_scheduler->executeAgents();
	_scheduler->removeAgents();
	log_INFO(logName.str(), getWallTime() << " finished step: " << _step);
}

void World::run()
{
	std::stringstream logName;
	logName << "simulation_" << getId();
	log_INFO(logName.str(), getWallTime() << " executing " << _simulation.getNumSteps() << " steps...");

	for(_step=0; _step<_simulation.getNumSteps(); _step++)
	{
		step();
	}
	// storing last step data
	if(_step%_simulation.getSerializerResolution()==0)
	{
		_scheduler->serializeRasters(_step);
		_scheduler->serializeAgents(_step);
	}
	
	log_INFO(logName.str(), getWallTime() << " closing files");
	_scheduler->finish();
}



int World::getCurrentStep() const
{
	return _step;
}

void World::stepEnvironment()
{
	for(size_t d=0; d<_rasters.size(); d++)
	{
		if(!_rasters.at(d) || !_dynamicRasters.at(d))
		{
			continue;
		}
		// TODO initial pos and matrix size are needed?
		stepRaster(d);
	}
}

void World::stepRaster( const int & index  )
{
	((Raster*)_rasters.at(index))->updateRasterIncrement();
}

void World::registerDynamicRaster( const std::string & key, const bool & serialize, int index )
{
	// if no index is provided, add one at the end
	if(index==-1)
	{
		index = _rasters.size();
	}
	
	if(_rasters.size()<=index)
	{
		size_t oldSize = _rasters.size();
		_rasters.resize(index+1);
		for(size_t i=oldSize; i<_rasters.size(); i++)
		{
			_rasters.at(i) = 0;
		}
		_dynamicRasters.resize(index+1);
		_serializeRasters.resize(index+1);
	}
	_rasterNames.insert(make_pair(key, index));
	_dynamicRasters.at(index) = true;
	if(_rasters.at(index))
	{
		delete _rasters.at(index);
	}
	_rasters.at(index) = new Raster();
	_rasters.at(index)->resize(_scheduler->getBoundaries()._size);
	_serializeRasters.at(index) = serialize;
}

void World::registerStaticRaster( const std::string & key, const bool & serialize, int index )
{
	// if no index is provided, add one at the end
	if(index==-1)
	{
		index = _rasters.size();
	}
		
	if(_rasters.size()<=index)
	{	
		size_t oldSize = _rasters.size();
		_rasters.resize(index+1);
		for(size_t i=oldSize; i<_rasters.size(); i++)
		{
			_rasters.at(i) = 0;
		}
		_dynamicRasters.resize(index+1);
		_serializeRasters.resize(index+1);
	}
	_rasterNames.insert(make_pair(key, index));
	if(_rasters.at(index))
	{
		delete _rasters.at(index);
	}
	_rasters.at(index) = new StaticRaster();
	_rasters.at(index)->resize(_scheduler->getBoundaries()._size);
	
	_dynamicRasters.at(index) = false;
	_serializeRasters.at(index) = serialize;
}

bool World::checkPosition( const Point2D<int> & newPosition )
{
	// checking size: if environment is a border of the real world
	if(!getBoundaries().contains(newPosition))
	{
		return false;
	}

	if(_allowMultipleAgentsPerCell)
	{
		return true;
	}
	
	// checking if it is already occupied
	std::vector<Agent *> hosts = _scheduler->getAgent(newPosition);
	if(hosts.size()==0)
	{
		return true;
	}
	for(size_t i=0; i<hosts.size(); i++)
	{
		Agent * agent = hosts.at(i);
		if(agent->exists())
		{
			return false;
		}
	}
	return true;
}

Simulation & World::getSimulation()
{
	return _simulation;
}

StaticRaster & World::getStaticRaster( const size_t & index )
{
	return *(_rasters.at(index));
}

StaticRaster & World::getStaticRaster( const std::string & key )
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	return getStaticRaster(it->second);
}

Raster & World::getDynamicRaster( const size_t & index)
{
	if(index>=_rasters.size())
	{
		std::stringstream oss;
		oss << "World::getDynamicRaster - index: " << index << " out of bound with size: " << _rasters.size();
		throw Exception(oss.str());
	}
	return (Raster &)*(_rasters.at(index));
}

Raster & World::getDynamicRaster( const std::string & key )
{	
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	if(it==_rasterNames.end())
	{
		std::stringstream oss;
		oss << "World::getDynamicRaster - raster: " << key << " not registered";
		throw Exception(oss.str());
	}
	return getDynamicRaster(it->second);
}

const Raster & World::getDynamicRaster( const size_t & index ) const
{	
	return (const Raster &)*(_rasters.at(index));
}

void World::setValue( const std::string & key, const Point2D<int> & position, int value )
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	setValue(it->second, position, value);
}

void World::setValue( const int & index, const Point2D<int> & position, int value )
{
	Raster * raster = (Raster*)(_rasters.at(index));
	_scheduler->setValue(*raster, position, value);
}

int World::getValue( const std::string & key, const Point2D<int> & position ) const
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	return getValue(it->second, position);
}

int World::getValue( const int & index, const Point2D<int> & position ) const
{
	Raster * raster = (Raster*)(_rasters.at(index));
	_scheduler->getValue(*raster, position);
}

void World::setMaxValue( const std::string & key, const Point2D<int> & position, int value )
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	setMaxValue(it->second, position, value);
}

void World::setMaxValue( const int & index, const Point2D<int> & position, int value )
{
	Raster * raster = (Raster*)(_rasters.at(index));
	_scheduler->setMaxValueAt(*raster, position, value);
}

int World::getMaxValueAt( const std::string & key, const Point2D<int> & position ) const
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	return getMaxValueAt(it->second, position);
}

int World::getMaxValueAt( const int & index, const Point2D<int> & position ) const
{
	Raster * raster = (Raster*)(_rasters.at(index));
	_scheduler->getMaxValueAt(*raster, position);
}

int World::countNeighbours( Agent * target, const double & radius, const std::string & type )
{
	return _scheduler->countNeighbours(target, radius, type);
}

AgentsVector World::getNeighbours( Agent * target, const double & radius, const std::string & type )
{
	return _scheduler->getNeighbours(target, radius, type);
}

Point2D<int> World::getRandomPosition()
{
	while(1)
	{
		Point2D<int> position = _scheduler->getRandomPosition();
		if(checkPosition(position))
		{
			return position;
		}
	}
}

double World::getWallTime() const
{
	return _scheduler->getWallTime();
}

const std::string & World::getRasterName( const int & index) const
{
	for(RasterNameMap::const_iterator it=_rasterNames.begin(); it!=_rasterNames.end(); it++)
	{
		if(it->second==index)
		{
			return it->first;
		}
	}
	std::stringstream oss;
	oss << "World::getRasterName - index: " << index << " doest no have a name";
	throw Exception(oss.str());
}


Scheduler * World::useSpacePartition(const std::string & fileName, int overlap, bool finalize )
{
	return new SpacePartition(fileName, overlap, finalize);
}

Scheduler * World::useOpenMPSingleNode(const std::string & fileName)
{
	return new OpenMPSingleNode(fileName);
}


const int & World::getId() const { return _scheduler->getId(); }
const int & World::getNumTasks() const { return _scheduler->getNumTasks(); }
const Rectangle<int> & World::getBoundaries() const{ return _scheduler->getBoundaries(); }
void World::removeAgent( Agent * agent ) { _scheduler->removeAgent(agent); }
Agent * World::getAgent( const std::string & id ) { return _scheduler->getAgent(id); }
AgentsVector World::getAgent( const Point2D<int> & position, const std::string & type) { return _scheduler->getAgent(position, type); }
void World::addStringAttribute( const std::string & type, const std::string & key, const std::string & value ) { _scheduler->addStringAttribute(type, key, value);}
void World::addIntAttribute( const std::string & type, const std::string & key, int value ) { _scheduler->addIntAttribute(type, key, value); }

} // namespace Engine

