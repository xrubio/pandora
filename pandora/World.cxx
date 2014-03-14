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

#include <GeneralState.hxx>

#include <Serializer.hxx>
#include <Logger.hxx>
#include <Statistics.hxx>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctime>

#include <SpacePartition.hxx>

namespace Engine
{

World::World( const Simulation & simulation, const int & overlap, const bool & allowMultipleAgentsPerCell, const std::string & fileName ) : _scheduler(0), _simulation(simulation), _allowMultipleAgentsPerCell(allowMultipleAgentsPerCell), _step(0)
{
	_scheduler = new SpacePartition(simulation, overlap, *this);
	GeneralState::serializer().setResultsFile(fileName);
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
	
	GeneralState::serializer().init(_simulation, _rasters, _dynamicRasters, _serializeRasters, *this);
	serializeStaticRasters();

	_scheduler->init2();
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



void World::serializeAgents()
{
	if(_step%_simulation.getSerializerResolution()!=0)
	{
		return;
	}

	AgentsList::iterator it=_agents.begin();
	int i = 0;
	while(it!=_agents.end())
	{
		if((*it)->exists())
		{
			GeneralState::serializer().serializeAgent((*it), _step, *this, i);
			i++;
		}
		it++;
	}
	// serialize remaining agents
	GeneralState::serializer().finishAgentsSerialization(_step, *this);
}

void World::step()
{

	std::stringstream logName;
	logName << "simulation_" << getId();
	log_INFO(logName.str(), getWallTime() << " executing step: " << _step );

	// actualitzar el món (raster i agents), si s'escau
	serializeRasters();
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " has serialized rasters");
	serializeAgents();
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " has serialized agents");
	
	// TODO if we modify agents in this step, they won't be updated in other nodes!
	stepAgents();
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " has executed step agents");
	// TODO the same with raster modification?
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

	_scheduler->initExecution();

	for(_step=0; _step<_simulation.getNumSteps(); _step++)
	{
		step();
	}
	// storing last step data
	serializeRasters();
	serializeAgents();
	
	log_INFO(logName.str(), getWallTime() << " closing files");
	GeneralState::serializer().finish();
	_scheduler->finishExecution();
}



int World::getCurrentStep() const
{
	return _step;
}

void World::serializeRasters()
{
	if(_step%_simulation.getSerializerResolution()!=0)
	{
		return;
	}

	for(size_t d=0; d<_rasters.size(); d++)
	{
		if(!_rasters.at(d) || !_serializeRasters.at(d) || !_dynamicRasters.at(d))
		{
			continue;
		}
		GeneralState::serializer().serializeRaster(d, (Raster&)(*_rasters.at(d)), *this, _step);
	}
}

void World::serializeStaticRasters()
{
	for(size_t d=0; d<_rasters.size(); d++)
	{
		if(!_rasters.at(d) || !_serializeRasters.at(d) || _dynamicRasters.at(d))			
		{
			continue;
		}
		GeneralState::serializer().serializeStaticRaster(d, *_rasters.at(d), *this);
	}
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

void World::stepAgents()
{
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
	((Raster*)_rasters.at(index))->setValue(_scheduler->getRealPosition(position), value);
}

int World::getValue( const std::string & key, const Point2D<int> & position ) const
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	return getValue(it->second, position);
}

int World::getValue( const int & index, const Point2D<int> & position ) const
{
	return _rasters.at(index)->getValue(_scheduler->getRealPosition(position));
}

void World::setMaxValue( const std::string & key, const Point2D<int> & position, int value )
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	setMaxValue(it->second, position, value);
}

void World::setMaxValue( const int & index, const Point2D<int> & position, int value )
{
	((Raster*)_rasters.at(index))->setMaxValue(_scheduler->getRealPosition(position), value);
}

int World::getMaxValueAt( const std::string & key, const Point2D<int> & position )
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	return getMaxValueAt(it->second, position);
}

int World::getMaxValueAt( const int & index, const Point2D<int> & position )
{
	std::stringstream logName;
	logName << "max_value_" << getId();
	log_DEBUG(logName.str(), getWallTime() << " accessing to pos: " << position << " real: " << _scheduler->getRealPosition(position) << " for index: " << index);
	return ((Raster*)_rasters.at(index))->getMaxValueAt(_scheduler->getRealPosition(position));
}

int World::countNeighbours( Agent * target, const double & radius, const std::string & type )
{
	int numAgents = for_each(_agents.begin(), _agents.end(), aggregatorCount<Engine::Agent>(radius,*target, type))._count;
	int numOverlapAgents = for_each(_scheduler->_overlapAgents.begin(), _scheduler->_overlapAgents.end(), aggregatorCount<Engine::Agent>(radius,*target, type))._count;
	return numAgents+numOverlapAgents;
}

World::AgentsVector World::getNeighbours( Agent * target, const double & radius, const std::string & type )
{
	AgentsVector agentsVector = for_each(_agents.begin(), _agents.end(), aggregatorGet<Engine::Agent>(radius,*target, type))._neighbors;
	AgentsVector overlapAgentsVector =  for_each(_scheduler->_overlapAgents.begin(), _scheduler->_overlapAgents.end(), aggregatorGet<Engine::Agent>(radius,*target, type))._neighbors;
	std::copy(overlapAgentsVector.begin(), overlapAgentsVector.end(), std::back_inserter(agentsVector));
	std::random_shuffle(agentsVector.begin(), agentsVector.end());
	return agentsVector;
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

int World::getId() const{ return _scheduler->getId(); }
const Rectangle<int> & World::getBoundaries() const{ return _scheduler->getBoundaries(); }
const Rectangle<int> & World::getOwnedArea() const{ return _scheduler->getOwnedArea(); }
const int & World::getOverlap() { return _scheduler->getOverlap(); }
const int & World::getNumTasks() const{ return _scheduler->getNumTasks(); }
const Size<int> & World::getSize() const{ return _simulation.getSize(); }
void World::removeAgent( Agent * agent ) { _scheduler->removeAgent(agent); }
Agent * World::getAgent( const std::string & id ) { return _scheduler->getAgent(id); }
World::AgentsVector World::getAgent( const Point2D<int> & position, const std::string & type) { return _scheduler->getAgent(position, type); }
void World::setFinalize( const bool & finalize ) { _scheduler->setFinalize(finalize); }

} // namespace Engine

