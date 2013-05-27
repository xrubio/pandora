
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

#ifdef PANDORAMPI
#include <Serializer.hxx>
#endif

#include <Logger.hxx>
#include <Statistics.hxx>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctime>

#ifdef PANDORAMPI
#include <MpiFactory.hxx>
#endif

namespace Engine
{

World::World( const Simulation & simulation, const int & overlap, const bool & allowMultipleAgentsPerCell, const std::string & fileName ) 
    : _simulation(simulation)
    , _worldPos(-1,-1)
    , _globalBoundaries(Point2D<int>(0,0) , Point2D<int>(simulation.getSize(), simulation.getSize()))
    , _allowMultipleAgentsPerCell(allowMultipleAgentsPerCell)
    , _step(0)
    , _overlap(overlap)
    , _searchAgents(true)
    , _initialTime(0.0f)
{
#ifdef PANDORAMPI
	GeneralState::serializer().setResultsFile(fileName);
#endif
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
	for(int i=0; i<_rasters.size(); i++)
	{
		if(_rasters.at(i))
		{
			delete _rasters.at(i);
		}
	}
}

void World::initialize()
{
	init(0,0);
}

void World::init( int argc, char *argv[] )
{
#ifdef PANDORAMPI
	MPI_Init(&argc, &argv);
#endif
	_simulation.init();
	checkOverlapSize();

	_initialTime = getWallTime();
	
	stablishPosition();
	createRasters();
	createAgents();
#ifdef PANDORAMPI
	GeneralState::serializer().init(_simulation, _rasters, _dynamicRasters, _serializeRasters, *this);
	serializeStaticRasters();
#endif
#ifdef PANDORAMPI
	MpiFactory::instance()->registerTypes();
#endif

	std::stringstream logName;
	logName << "simulation_" << _simulation.getId();
	log_INFO(logName.str(), "finished init at: "  << getWallTime());
}

void World::checkOverlapSize()
{
	int subfieldSize = _simulation.getLocalRasterSize()/2;
	if(_overlap*2>subfieldSize)
	{
		std::stringstream oss;
		oss << "World::checkOverlapSize- subfieldize: " << subfieldSize << " must be at lest twice the value of overlap: " << _overlap << " to avoid conflicts between non adjacent subfields";
		throw Exception(oss.str());
	}
}

void World::stablishPosition()
{
#ifdef PANDORAMPI
	int worldsPerRow = sqrt(_simulation.getNumTasks());	
	if(_simulation.getSize()%worldsPerRow!=0)
	{
		std::stringstream oss;
		oss << "World::stablishPosition - globalRasterSize: " << _simulation.getSize() << " is not divisible by sqrt(worlds): " << worldsPerRow;
		throw Engine::Exception(oss.str());
	}
	_worldPos = getPositionFromId(_simulation.getId());

	for(int x=_worldPos._x-1; x<=_worldPos._x+1; x++)
	{
		for(int y=_worldPos._y-1; y<=_worldPos._y+1; y++)
		{
			if(x>-1 && x<worldsPerRow && y>-1 && y<worldsPerRow)
			{
				if(x!=_worldPos._x || y!=_worldPos._y)
				{
					_neighbors.push_back(y*worldsPerRow+x);
				}
			}
		}
	}

	// stablishing boundaries
	_boundaries._origin = _worldPos*_simulation.getLocalRasterSize();
	_boundaries._size._x = _simulation.getLocalRasterSize();
	_boundaries._size._y = _simulation.getLocalRasterSize();

	// defining overlap boundaries
	_overlapBoundaries = _boundaries;
	// west boundary
	if(_boundaries._origin._x!=_globalBoundaries._origin._x)
	{
		_overlapBoundaries._origin._x -= _overlap;
		_overlapBoundaries._size._x += _overlap;
	}
	// east boundary
	if(_boundaries._origin._x!=_globalBoundaries._size._x-_boundaries._size._x)
	{
		_overlapBoundaries._size._x += _overlap;
	}
	// north boundary
	if(_boundaries._origin._y!=_globalBoundaries._origin._y)
	{
		_overlapBoundaries._origin._y -= _overlap;
		_overlapBoundaries._size._y += _overlap;
	}
	// south boundary
	if(_boundaries._origin._y!=_globalBoundaries._size._y-_boundaries._size._y)
	{
		_overlapBoundaries._size._y += _overlap;
	}
#else
	_boundaries._size = Point2D<int>(_simulation.getLocalRasterSize(), _simulation.getLocalRasterSize());
	_boundaries._origin = Point2D<int>(0,0);
	_overlapBoundaries = _boundaries;
#endif
	// creating sections
	_sections.resize(4);
	_sections[0] = Rectangle<int>(_boundaries._origin, _boundaries._size/2);
	_sections[1] = Rectangle<int>(Point2D<int>(_boundaries._origin._x+_boundaries._size._x/2, _boundaries._origin._y), _boundaries._size/2);
	_sections[2] = Rectangle<int>(Point2D<int>(_boundaries._origin._x, _boundaries._origin._y+_boundaries._size._y/2), _boundaries._size/2);
	_sections[3] = Rectangle<int>(Point2D<int>(_boundaries._origin._x+_boundaries._size._x/2, _boundaries._origin._y+_boundaries._size._y/2), _boundaries._size/2);

	std::stringstream logName;
	logName << "simulation_" << _simulation.getId();
	log_INFO(logName.str(), getWallTime() << " pos: " << _worldPos << ", global boundaries: " << _globalBoundaries << ", boundaries: " << _boundaries << " and overlapped area: " << _overlapBoundaries << " with size: " << _overlapBoundaries._size );
	log_INFO(logName.str(), getWallTime() << " sections 0: " << _sections[0] << " - 1: " << _sections[1] << " - 2:" << _sections[2] << " - 3: " << _sections[3]);
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

bool World::hasBeenExecuted( Agent * agent )
{
	if(_executedAgentsHash.find(agent->getId())==_executedAgentsHash.end())
	{
		return false;
	}
	return true;
}

bool World::willBeRemoved( Agent * agent )
{
	for(AgentsList::iterator it=_removedAgents.begin(); it!=_removedAgents.end(); it++)
	{	
		if((*it)->getId().compare(agent->getId())==0)
		{
			return true;
		}
	}
	return false;
}

void World::addAgent( Agent * agent )
{
	agent->setWorld(this);
	// we need to remove any previous instance of this agent in overlap

	AgentsList::iterator it = getGhostAgent(agent->getId());
	if(it!=_overlapAgents.end())
	{
		_overlapAgents.erase(it);
	}
	_agents.push_back(agent);	
	
	std::stringstream logName;
	logName << "agents_" << getId() << "_" << agent->getId();
	log_EDEBUG( logName.str(), "agent: " << agent << " added at time step: " << getCurrentTimeStep());
}

void World::stepSection( const int & sectionIndex )
{
	std::stringstream logName;
	logName << "simulation_" << _simulation.getId();

	log_DEBUG(logName.str(), getWallTime() << " beginning step: " << _step << " section: " << sectionIndex);

	// first of all we will execute raster rules for the zone where stepSection is executed
	stepRastersSection(sectionIndex, _sections[sectionIndex]);

	//std::cout << _simulation.getId() << " executing section: " << sectionIndex << " in step: " << _step << " with zone: " << _sections[sectionIndex] << std::endl;
	AgentsList::iterator it=_agents.begin();
	std::vector<Agent*> agentsToExecute;
	// we have to randomize the execution of agents in a given section index
	while(it!=_agents.end())
	{
		Agent * agent = *it;
		if(_sections[sectionIndex].isInside(agent->getPosition()) && !hasBeenExecuted(agent))
		{
			agentsToExecute.push_back(agent);
		}
		it++;
	}
	std::random_shuffle(agentsToExecute.begin(), agentsToExecute.end());
	int numExecutedAgents = 0;
	AgentsList agentsToSend;

#ifndef PANDORAEDEBUG
	// plan actions, disabled for debug
	#pragma omp parallel for
#endif
	for(int i=0; i<agentsToExecute.size(); i++)
	{
		Agent * agent = agentsToExecute[i];
		//Agent * agent = *it;
		if(_sections[sectionIndex].isInside(agent->getPosition()) && !hasBeenExecuted(agent))
		{
			agent->logAgentState();
			agent->updateKnowledge();
			agent->selectActions();
		}
	}

	// execute actions
	for(int i=0; i<agentsToExecute.size(); i++)
	{
		Agent * agent = agentsToExecute.at(i);
		if(_sections[sectionIndex].isInside(agent->getPosition()) && !hasBeenExecuted(agent))
		{
			log_DEBUG(logName.str(), getWallTime() << " agent: " << agent << " being executed at index: " << sectionIndex << " of task: "<< _simulation.getId() << " in step: " << _step );
			agent->executeActions();
			agent->updateState();
			log_DEBUG(logName.str(), getWallTime() << " agent: " << agent << " has been executed at index: " << sectionIndex << " of task: "<< _simulation.getId() << " in step: " << _step );

#ifdef PANDORAMPI
			if(!_boundaries.isInside(agent->getPosition()) && !willBeRemoved(agent))
			{
				log_DEBUG(logName.str(), getWallTime() << " migrating agent: " << agent << " being executed at index: " << sectionIndex << " of task: "<< _simulation.getId() );
				agentsToSend.push_back(agent);

				// the agent is no longer property of this world
				AgentsList::iterator itErase  = getOwnedAgent(agent->getId());
				// it will be deleted
				_agents.erase(itErase);
				_overlapAgents.push_back(agent);
				log_DEBUG(logName.str(), getWallTime() <<  "putting agent: " << agent << " to overlap");
			}
			else
			{
				log_DEBUG(logName.str(), getWallTime() << " finished agent: " << agent);
			}
#endif
			_executedAgentsHash.insert(make_pair(agent->getId(), agent));
			numExecutedAgents++;
			log_DEBUG(logName.str(), getWallTime()  << " num executed agents: " << numExecutedAgents );
		}
	}
#ifdef PANDORAMPI
	log_DEBUG(logName.str(), getWallTime()  << " sending agents in section: " << sectionIndex << " and step: " << _step);
	sendAgents(agentsToSend);
#endif
	log_DEBUG(logName.str(), getWallTime() << " has finished section: " << sectionIndex << " and step: " << _step);
	
	log_DEBUG(logName.str(), getWallTime() << " executed step: " << _step << " section: " << sectionIndex << " in zone: " << _sections[sectionIndex] << " with num executed agents: " << numExecutedAgents << " total agents: " << _agents.size() << " and overlap agents: " << _overlapAgents.size());
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
#ifdef PANDORAMPI
			GeneralState::serializer().serializeAgent((*it), _step, *this, i);
#endif
			i++;
		}
		it++;
	}
	// serialize remaining agents
#ifdef PANDORAMPI
	GeneralState::serializer().finishAgentsSerialization(_step, *this);
#endif
}

#ifdef PANDORAMPI

void World::sendAgents( AgentsList & agentsToSend )
{
	if(_neighbors.size()==0)
	{
		return;
	}
	//TODO: es poden enviar missatges de forma no sincronitzada ni predestinada?
	std::stringstream logName;
	logName << "MPI_agents_world_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " sendAgent: " << agentsToSend.size() << " agents");

	// for each neighbor, we send the number of agents to send
	for(MpiFactory::TypesMap::iterator itType=MpiFactory::instance()->beginTypes(); itType!=MpiFactory::instance()->endTypes(); itType++)
	{
		log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " sendAgent - checking mpi type: " << itType->first );
		// add each agent to the list of the neighbour where it will be sent
		std::vector< AgentsList > agentsToNeighbors;
		agentsToNeighbors.resize(_neighbors.size());


		for(AgentsList::iterator it=agentsToSend.begin(); it!=agentsToSend.end(); it++)
		{
			Agent * agent = (*it);
			if(agent->isType(itType->first))
			{
				int newID = getIdFromPosition(agent->getPosition());
				agentsToNeighbors[getNeighborIndex(newID)].push_back(agent);
			}
		}

		MPI_Datatype * agentType = itType->second;
		for(int i=0; i<_neighbors.size(); i++)
		{	
			int numAgents = agentsToNeighbors[i].size();
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " sendAgent - sending num agents: " << numAgents << " to: " << _neighbors[i]);
			int error = MPI_Send(&numAgents, 1, MPI_INTEGER, _neighbors[i], eNumAgents, MPI_COMM_WORLD);
			if(error != MPI_SUCCESS)
			{
				std::stringstream oss;
				oss << "World::sendAgents - " << _simulation.getId() << " error in MPI_Send : " << error;
				throw Exception(oss.str());
			}
			AgentsList::iterator it= agentsToNeighbors[i].begin();
			while(it!=agentsToNeighbors[i].end())
			{
				Agent * agent = *it;
				void * package = agent->fillPackage();
				log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " sendAgent - sending agent: " << *it << " to: " << _neighbors[i] );
				error = MPI_Send(package, 1, *agentType, _neighbors[i], eAgent, MPI_COMM_WORLD);
				delete package;
				if(error != MPI_SUCCESS)
				{
					std::stringstream oss;
					oss << "World::sendAgents - " << _simulation.getId() << " error in MPI_Send : " << error;
					throw Exception(oss.str());
				}
				agent->sendVectorAttributes(_neighbors[i]);
				// it is not deleted, as it is sent to overlap list
				it = agentsToNeighbors[i].erase(it);
			}
		}
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " sendAgent -  end checking agents to send: " << agentsToSend.size());
}

void World::sendOverlapZones( const int & sectionIndex, const bool & entireOverlap )
{
	std::stringstream logName;
	logName << "MPI_raster_world_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " sendOverlapZones");
	std::vector<int> neighborsToUpdate;
	
	for(int i=0; i<_neighbors.size(); i++)
	{
		if(needsToBeUpdated(_neighbors[i], sectionIndex))
		{
			neighborsToUpdate.push_back(_neighbors[i]);
		}
	}

	for(int d=0; d<_rasters.size(); d++)
	{
		if(!_rasters.at(d) || !_dynamicRasters.at(d))
		{
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " index: " << d << " not sending");
			continue;
		}
		log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " sending raster: " << d);
		for(int i=0; i<neighborsToUpdate.size(); i++)
		{
			MpiOverlap * send = new MpiOverlap;
			if(entireOverlap)
			{
				send->_overlap= getOverlap(neighborsToUpdate[i], sectionIndex);
				log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " send entire overlap: " << send->_overlap << " to: " << neighborsToUpdate[i]);
			}
			else
			{
				send->_overlap = getInternalOverlap(neighborsToUpdate[i]);
				log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " send partial overlap: " << send->_overlap << " to: " << neighborsToUpdate[i]);
			}
			const Rectangle<int> & overlapZone = send->_overlap;
			send->_data.resize(overlapZone._size._x * overlapZone._size._y);
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " will send overlap to: " << neighborsToUpdate[i] << " with size: " << send->_data.size() << " and zone: " << overlapZone);
			for(int n=0; n<send->_data.size(); n++)
			{
				Point2D<int> index(overlapZone._origin._x+n%overlapZone._size._x, overlapZone._origin._y+n/overlapZone._size._x);
				send->_data.at(n) = getDynamicRaster(d).getValue(index);
				log_EDEBUG(logName.str(), "\t" << getWallTime() << " step: " << _step << "/" << sectionIndex << " send index: " << index << " in global pos: " << index+_overlapBoundaries._origin << " value: " << send->_data.at(n));
			}
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " raster: " << d << " will be sent");
			MPI_Isend(&send->_data[0], send->_data.size(), MPI_INTEGER, neighborsToUpdate[i], eRasterData, MPI_COMM_WORLD, &send->_request);
			_sendRequests.push_back(send);
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " raster: " << d << " data sent to: " << _neighbors[i]);
		}
		log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " raster: " << d << " sent");
	}
	log_DEBUG(logName.str(), getWallTime() << " step: "  << "/" << sectionIndex << _step << " sendOverlapZones ended");
}

void World::sendMaxOverlapZones()
{
	std::stringstream logName;
	logName << "MPI_raster_world_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " step: "  << _step << " sendMaxOverlapZones");
	for(int d=0; d<_rasters.size(); d++)
	{
		if(!_rasters.at(d) || !_dynamicRasters.at(d))
		{
			continue;
		}

		log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " sending max raster: " << d);
		for(int i=0; i<_neighbors.size(); i++)
		{
			MpiOverlap * send = new MpiOverlap;
			send->_overlap = getInternalOverlap(_neighbors[i]);
			send->_data.resize(send->_overlap._size._x * send->_overlap._size._y);
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " will send max overlap of: " << d << " to: " << _neighbors[i] << " with size: " << send->_data.size() << " and zone: " << send->_overlap << " to " << _neighbors[i]);
			const Rectangle<int> & overlapZone = send->_overlap;
			for(int n=0; n<send->_data.size(); n++)
			{
				Point2D<int> index(overlapZone._origin._x+n%overlapZone._size._x, overlapZone._origin._y+n/overlapZone._size._x);
				send->_data.at(n) = getDynamicRaster(d).getMaxValueAt(index);
				log_EDEBUG(logName.str(), "\t" << getWallTime() << " step: " << _step << " send index: " << index << " in global pos: " << index+_overlapBoundaries._origin << " max value: " << send->_data.at(n));
			}
			MPI_Isend(&send->_data[0], send->_data.size(), MPI_INTEGER, _neighbors[i], eRasterMaxData, MPI_COMM_WORLD, &send->_request);
			_sendRequests.push_back(send);
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " raster: " << d << " max data sent to: " << _neighbors[i]);
		}
		log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " raster: " << d << " max data sent");
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " sendMaxOverlapZones ended");
}

void World::sendGhostAgents( const int & sectionIndex )
{
	std::stringstream logName;
	logName << "MPI_agents_world_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " send ghost agents for section index: " << sectionIndex );

	std::vector<int> neighborsToUpdate;
	for(int i=0; i<_neighbors.size(); i++)
	{
		if(needsToBeUpdated(_neighbors[i], sectionIndex))
		{
			neighborsToUpdate.push_back(_neighbors[i]);
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " section index: " << sectionIndex << " will send overlap to: " << _neighbors[i]);
		}
	}

	// for each type of agent we will send the collection of agents of the particular type to neighbors
	for(MpiFactory::TypesMap::iterator itType=MpiFactory::instance()->beginTypes(); itType!=MpiFactory::instance()->endTypes(); itType++)
	{
		log_DEBUG(logName.str(),  getWallTime() << " step: " << _step << " section index: " << sectionIndex << " checking type: " << itType->first );
		MPI_Datatype * agentType = itType->second;

		std::vector< AgentsList > agentsToNeighbors;
		agentsToNeighbors.resize(neighborsToUpdate.size());

		for(int i=0; i<neighborsToUpdate.size(); i++)
		{
			Rectangle<int> overlapZone = getOverlap(neighborsToUpdate[i], sectionIndex);
			for(AgentsList::iterator it=_agents.begin(); it!=_agents.end(); it++)
			{
				Agent * agent = *it;
				// we check the type. TODO register the type in another string
				// TODO refactor!!!
				log_DEBUG(logName.str(),  getWallTime() << " step: " << _step << " agent: " << agent << " of type: " << itType->first << " test will be removed: " << willBeRemoved(agent) << " checking overlap zone: " << overlapZone << " overlap boundaries: " << _overlapBoundaries << " - test is inside zone: " << overlapZone.isInside(agent->getPosition()-_overlapBoundaries._origin));
				if(agent->isType(itType->first))
				{
					if((!willBeRemoved(agent)) && (overlapZone.isInside(agent->getPosition()-_overlapBoundaries._origin)))
					{
						agentsToNeighbors[i].push_back(*it);
						log_DEBUG(logName.str(),  getWallTime() << " step: " << _step << " sending ghost agent: " << agent << " to: " << neighborsToUpdate[i] << " in section index: " << sectionIndex);
					}
				}
			}
			for(AgentsList::iterator it=_overlapAgents.begin(); it!=_overlapAgents.end(); it++)
			{
				Agent * agent = *it;	
				if(agent->isType(itType->first))
				{
					if((!willBeRemoved(agent)) && (overlapZone.isInside(agent->getPosition()-_overlapBoundaries._origin)))
					{
						agentsToNeighbors[i].push_back(*it);
						log_DEBUG(logName.str(),  getWallTime() << " step: " << _step << " will send modified ghost agent: " << agent << " to: " << neighborsToUpdate[i] << " in section index: " << sectionIndex << " and step: " << _step);
					}
				}
			}

			int numAgents = agentsToNeighbors[i].size();
			log_DEBUG(logName.str(),  getWallTime() << " step: " << _step << " sending num ghost agents: " << numAgents << " to : " << neighborsToUpdate[i] << " in step: " << _step << " and section index: " << sectionIndex );
			int error = MPI_Send(&numAgents, 1, MPI_INTEGER, neighborsToUpdate[i], eNumGhostAgents, MPI_COMM_WORLD);
			if(error != MPI_SUCCESS)
			{
				std::stringstream oss;
				oss << "World::sendGhostAgents - " << _simulation.getId() << " error in MPI_Send : " << error;
				throw Exception(oss.str());
			}
			for(AgentsList::iterator it=agentsToNeighbors[i].begin(); it!=agentsToNeighbors[i].end(); it++)
			{
				Agent * agent = *it;
				void * package = agent->fillPackage();
				log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " sending ghost agent: " << *it << " from: " << _simulation.getId() << " to: " << neighborsToUpdate[i]);
				error = MPI_Send(package, 1, *agentType, neighborsToUpdate[i], eGhostAgent, MPI_COMM_WORLD);
				delete package;
				if(error != MPI_SUCCESS)
				{
					std::stringstream oss;
					oss << "World::sendGhostAgents - " << _simulation.getId() << " error in MPI_Send : " << error;
					throw Exception(oss.str());
				}
				agent->sendVectorAttributes(neighborsToUpdate[i]);
			}
		}
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " send ghost agents for section index: " << sectionIndex << " finished");

}

void World::receiveGhostAgents( const int & sectionIndex )
{
	std::stringstream logName;
	logName << "MPI_agents_world_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " receive ghost agents for section index: " << sectionIndex );


	// we need to calculate how many neighbors will send data to this id
	std::vector<int> neighborsToUpdate;
	for(int i=0; i<_neighbors.size(); i++)
	{
		if(needsToReceiveData(_neighbors[i], sectionIndex))
		{
			neighborsToUpdate.push_back(_neighbors[i]);
		}
	}

	// for each type of agent we will send the collection of agents of the particular type to neighbors	
	for(MpiFactory::TypesMap::iterator itType=MpiFactory::instance()->beginTypes(); itType!=MpiFactory::instance()->endTypes(); itType++)
	{
		MPI_Datatype * agentType = itType->second;

		for(int i=0; i<neighborsToUpdate.size(); i++)
		{
			AgentsList newGhostAgents;
			int numAgentsToReceive;
			MPI_Status status;

			int error = MPI_Recv(&numAgentsToReceive, 1, MPI_INTEGER, neighborsToUpdate[i], eNumGhostAgents, MPI_COMM_WORLD, &status);			
			if(error!=MPI_SUCCESS)
			{
				std::stringstream oss;
				oss << "World::receiveGhostAgents - " << _simulation.getId() << " error in MPI_Recv: " << error;
				throw Exception(oss.str());
			}
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " has received message from " << neighborsToUpdate[i] << ", num ghost agents: " << numAgentsToReceive );
			for(int j=0; j<numAgentsToReceive; j++)
			{
				void * package = MpiFactory::instance()->createDefaultPackage(itType->first);
				error = MPI_Recv(package, 1, *agentType, neighborsToUpdate[i], eGhostAgent, MPI_COMM_WORLD, &status);					
				if(error!=MPI_SUCCESS)
				{
					std::stringstream oss;
					oss << "World::receiveGhostAgents - " << _simulation.getId() << " error in MPI_Recv: " << error;
					throw Exception(oss.str());
				}
				Agent * agent = MpiFactory::instance()->createAndFillAgent(itType->first, package);
				log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " has received ghost agent: " << agent << " number: " << j << " from: " << neighborsToUpdate[i] << " in section index: " << sectionIndex << " and step: " << _step );
				delete package;
				agent->receiveVectorAttributes(neighborsToUpdate[i]);

				// we must check if it is an update of an agent, or a ghost agent
				bool worldOwnsAgent = false;
				AgentsList::iterator it = getOwnedAgent(agent->getId());
				if(it!=_agents.end())
				{
					log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " has received update of own agent: " << *it << " in step: " << _step );
					_agents.erase(it);
					addAgent(agent);
					worldOwnsAgent = true;
				}
				if(!worldOwnsAgent)
				{
					newGhostAgents.push_back(agent);
				}
			}
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " num ghost agents sent for neighbor: " << neighborsToUpdate[i] << " in section index: " << sectionIndex << ": " << newGhostAgents.size());
			// if the agent is in the zone to be updated, remove it
			Rectangle<int> overlapZone = getOverlap(neighborsToUpdate[i], sectionIndex);
			AgentsList::iterator it=_overlapAgents.begin();
			while(it!=_overlapAgents.end())
			{
				Agent * agent = (*it);
				if(agent->isType(itType->first))
				{
					// si l'agent no està en zona que s'ha d'actualitzar, continuar
					if(overlapZone.isInside((*it)->getPosition()-_overlapBoundaries._origin))
					{
						log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " in section index: " << sectionIndex << " with overlap zone: " << overlapZone << " erasing agent: " << *it);
						it = _overlapAgents.erase(it);
					}
					else
					{
						log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " in section index: " << sectionIndex <<  " with overlap zone: " << overlapZone << " maintaining agent: " << *it );
						it++;
					}
				}
				else
				{
					it++;
				}
			}
			// afterwards we will add the new ghost agents
			for(it=newGhostAgents.begin(); it!=newGhostAgents.end(); it++)
			{
				log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " in section index: " << sectionIndex << " adding ghost agent: " << *it );
				_overlapAgents.push_back(*it);
			}
		}
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " receive ghost agents for section index: " << sectionIndex << " finished");

}

void World::receiveAgents( const int & sectionIndex )
{
	std::stringstream logName;
	logName << "MPI_agents_world_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " receiving agents for section index: " << sectionIndex);
	for(MpiFactory::TypesMap::iterator itType=MpiFactory::instance()->beginTypes(); itType!=MpiFactory::instance()->endTypes(); itType++)
	{
		log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " receiveAgent - checking mpi type: " << itType->first);
		MPI_Datatype * agentType = itType->second;

		for(int i=0; i<_neighbors.size(); i++)
		{
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " receiveAgent - checking mpi type: " << itType->first << " for neighbor: " << _neighbors[i] );

			int numAgentsToReceive;
			MPI_Status status;
			int error = MPI_Recv(&numAgentsToReceive, 1, MPI_INTEGER, _neighbors[i], eNumAgents, MPI_COMM_WORLD, &status);			
			if(error!=MPI_SUCCESS)
			{
				std::stringstream oss;
				oss << "World::receiveAgents - " << _simulation.getId() << " error in MPI_Recv: " << error;
				throw Exception(oss.str());
			}
			log_DEBUG(logName.str(), getWallTime() <<  " receiveAgents - received message from " << _neighbors[i] << ", num agents: " << numAgentsToReceive);
			for(int j=0; j<numAgentsToReceive; j++)
			{
				void * package = MpiFactory::instance()->createDefaultPackage(itType->first);
				error = MPI_Recv(package, 1, *agentType, _neighbors[i], eAgent, MPI_COMM_WORLD, &status);					
				if(error!=MPI_SUCCESS)
				{
					std::stringstream oss;
					oss << "World::receiveAgents - " << _simulation.getId() << " error in MPI_Recv: " << error;
					throw Exception(oss.str());
				}
				Agent * agent = MpiFactory::instance()->createAndFillAgent(itType->first, package);
				log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " receiveAgents - received agent: " << agent << " number: " << j << " from: " << _neighbors[i]);
				delete package;
				agent->receiveVectorAttributes(_neighbors[i]);
				_executedAgentsHash.insert(make_pair(agent->getId(), agent));
				addAgent(agent);
			}
		}
	}
}

void World::receiveOverlapData( const int & sectionIndex, const bool & entireOverlap )
{
	std::stringstream logName;
	logName << "MPI_raster_world_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " step: "  << "/" << sectionIndex << _step << " receiveOverlapData");

	// we need to calculate how many neighbors will send data to this id
	std::vector<int> neighborsToUpdate;
	for(int i=0; i<_neighbors.size(); i++)
	{
		if(needsToReceiveData(_neighbors[i], sectionIndex))
		{
			neighborsToUpdate.push_back(_neighbors[i]);
			//std::cout << _simulation.getId() << " - step: " << _step << "/" << sectionIndex << " will receive overlap from: " << _neighbors[i] << std::endl;
		}
	}

	// for each raster, we receive data from all the active neighbors
	for(int d=0; d<_rasters.size(); d++)
	{	
		if(!_rasters.at(d) || !_dynamicRasters.at(d))
		{
			continue;
		}
		
		log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " receiving raster: " << d);
		for(int i=0; i<neighborsToUpdate.size(); i++)
		{
			MpiOverlap* receive = new MpiOverlap;
			// TODO move to index
			receive->_rasterName = getRasterName(d);
			if(entireOverlap)
			{
				receive->_overlap = getOverlap(neighborsToUpdate[i], sectionIndex);
				log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " receive entire overlap: " << receive->_overlap << " from " << neighborsToUpdate[i]);
			}
			else
			{
				receive->_overlap = getExternalOverlap(neighborsToUpdate[i]);
				log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " receive external overlap: " << receive->_overlap << " from " << neighborsToUpdate[i]);
			}
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " AAA will receive overlap from: " << neighborsToUpdate[i] << " with size: " << receive->_data.size() << " and zone: " << receive->_overlap );
			receive->_data.resize(receive->_overlap._size._x*receive->_overlap._size._y);
			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " will receive overlap from: " << neighborsToUpdate[i] << " with size: " << receive->_data.size() << " and zone: " << receive->_overlap );
			MPI_Irecv(&receive->_data[0], receive->_data.size(), MPI_INTEGER, neighborsToUpdate[i], eRasterData, MPI_COMM_WORLD, &receive->_request);
			_receiveRequests.push_back(receive);
		}
		log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " raster: " << d << " received");
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << "/" << sectionIndex << " receiveOverlapData ended");
}

void World::receiveMaxOverlapData()
{
	std::stringstream logName;
	logName << "MPI_raster_world_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " step: "  << _step << " receiveMaxOverlapData");
	// for each raster, we receive data from all the active neighbors
	for(int d=0; d<_rasters.size(); d++)
	{
		if(!_rasters.at(d) || !_dynamicRasters.at(d))
		{
			continue;
		}
		log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " receiving max raster: " << d);
		for(int i=0; i<_neighbors.size(); i++)			
		{
			MpiOverlap* receive = new MpiOverlap;
			receive->_rasterName = getRasterName(d);
			receive->_overlap = getExternalOverlap(_neighbors[i]);
			receive->_data.resize(receive->_overlap._size._x*receive->_overlap._size._y);

			log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " will receive max overlap to: " << _neighbors[i] << " with size: " << receive->_data.size() << " and zone: " << receive->_overlap << " from " << _neighbors[i]);
			MPI_Irecv(&receive->_data[0], receive->_data.size(), MPI_INTEGER, _neighbors[i], eRasterMaxData, MPI_COMM_WORLD, &receive->_request);
			_receiveRequests.push_back(receive);
		}
		log_DEBUG(logName.str(), getWallTime() << " step: " << _step  << " raster: " << d << " max data received");
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " receiveMaxOverlapData ended");
}

void World::clearRequests( bool updateMaxValues )
{	
	std::stringstream logName;
	logName << "MPI_raster_world_" << _simulation.getId();
	int finished = 0;

	// MPI_Isend
	while(_sendRequests.size()!=0)
	{
		std::list<MpiOverlap*>::iterator it=_sendRequests.begin();
		while(it!=_sendRequests.end())
		{
			MpiOverlap* send = *it;
			MPI_Status status;
			MPI_Test(&send->_request, &finished, &status);
			if(finished)
			{
				it = _sendRequests.erase(it);
				log_DEBUG(logName.str(), getWallTime() << " request finished, lacking: " << _sendRequests.size() << " requests");
				delete send;
			}
		}
	}

	// MPI_Irecv
	while(_receiveRequests.size()!=0)
	{
		std::list<MpiOverlap*>::iterator it=_receiveRequests.begin();
		while(it!=_receiveRequests.end())
		{
			MpiOverlap* receive = *it;
			MPI_Status status;
			MPI_Test(&receive->_request, &finished, &status);
			if(finished)
			{
				it = _receiveRequests.erase(it);
				log_DEBUG(logName.str(), getWallTime() << " receive request finished, lacking: " << _receiveRequests.size() << " requests");

				const Rectangle<int> & overlapZone = receive->_overlap;
				log_DEBUG(logName.str(), getWallTime() << " processing request for overlap: " << overlapZone << " raster: " << receive->_rasterName << " for max values");
				for(int i=0; i<receive->_data.size(); i++)
				{
					Point2D<int> index(overlapZone._origin._x+i%overlapZone._size._x, overlapZone._origin._y+i/overlapZone._size._x);
					if(updateMaxValues)
					{
						log_EDEBUG(logName.str(), "\t" << getWallTime() << " step: " << _step << " receive index: " << index << " max value: " << receive->_data.at(i));
						getDynamicRasterStr(receive->_rasterName).setMaxValue(index, receive->_data.at(i));
					}
					else
					{
						log_EDEBUG(logName.str(), "\t" << getWallTime() << " step: " << _step << " receive index: " << index << " current value: " << receive->_data.at(i));
						getDynamicRasterStr(receive->_rasterName).setValue(index, receive->_data.at(i));
					}
				}

				if(updateMaxValues)
				{
					log_DEBUG(logName.str(), getWallTime() << " receive request finished for max values of raster: " <<  receive->_rasterName);
				}
				else
				{
					log_DEBUG(logName.str(), getWallTime() << " receive request finished for current values of raster: " <<  receive->_rasterName);
				}
				delete receive;
			}
		}
	}
	
}

#endif

int World::getIdFromPosition( const Point2D<int> & position )
{
	Point2D<int> nodePosition = position/_boundaries._size;
	return nodePosition._y*sqrt(_simulation.getNumTasks())+nodePosition._x;
}

Point2D<int> World::getPositionFromId( const int & id ) const
{
	int worldsPerRow = sqrt(_simulation.getNumTasks());	
	Point2D<int> worldPos(id%worldsPerRow, id/worldsPerRow);
	return worldPos;
}

int World::getNeighborIndex( const int & id )
{
	for(int i=0; i<_neighbors.size(); i++)
	{
		if(_neighbors[i] == id)
		{
			return i;
		}
	}
	std::stringstream oss;
	oss << "World::getNeighborIndex- " << _simulation.getId() << " looking for neighbor with id: " << id << " not found";
	throw Exception(oss.str());
}

void World::step()
{

	std::stringstream logName;
	logName << "simulation_" << _simulation.getId();
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
#ifdef PANDORAMPI
	for(int sectionIndex=0; sectionIndex<4; sectionIndex++)
	{
		// section index doesn't matter if is the entire overlap
		// TODO refactor? we are sending 4 times all the info
		sendOverlapZones(sectionIndex, false);
		receiveOverlapData(sectionIndex, false);
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " has executed update overlap");
#endif
	_executedAgentsHash.clear();

	std::stringstream logNameMpi;
	logNameMpi << "simulation_" << _simulation.getId();

	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " executing sections");
	for(int sectionIndex=0; sectionIndex<4; sectionIndex++)
	{
		stepSection(sectionIndex);
#ifdef PANDORAMPI
		log_DEBUG(logNameMpi.str(), getWallTime() << " executing step: " << _step << " and section: " << sectionIndex << " has been executed");
		receiveAgents(sectionIndex);
		log_DEBUG(logNameMpi.str(), getWallTime() << " executing step: " << _step << " and section: " << sectionIndex << " has received agents");

		sendGhostAgents(sectionIndex);
		log_DEBUG(logNameMpi.str(), getWallTime() << " executing step: " << _step << " and section: " << sectionIndex << " sent ghosts");
		receiveGhostAgents(sectionIndex);
		log_DEBUG(logNameMpi.str(), getWallTime() << " executing step: " << _step << " and section: " << sectionIndex << " received ghosts");

		sendOverlapZones(sectionIndex);
		log_DEBUG(logNameMpi.str(), getWallTime() << " executing step: " << _step << " and section: " << sectionIndex << " sent overlap");
		receiveOverlapData(sectionIndex);
		log_DEBUG(logNameMpi.str(), getWallTime() << " executing step: " << _step << " and section: " << sectionIndex << " received overlap" );
		MPI_Barrier(MPI_COMM_WORLD);
#endif
	}

	removeAgents();
	log_INFO(logName.str(), getWallTime() << " - world at pos: " << _worldPos << " finished step: " << _step);
}

void World::run()
{
	std::stringstream logName;
	logName << "simulation_" << _simulation.getId();
	log_INFO(logName.str(), getWallTime() << " executing " << _simulation.getNumSteps() << " steps...");

#ifdef PANDORAMPI
	// we need to send the agents and data in overlap zone to adjacent computer nodes	
	sendMaxOverlapZones();
	receiveMaxOverlapData();

	clearRequests(true);
	// all nodes must finish receiving max values before receiving current values
	MPI_Barrier(MPI_COMM_WORLD);

	for(int sectionIndex=0; sectionIndex<4; sectionIndex++)
	{

		// section index doesn't matter if is the entire overlap
		// TODO refactor? we are sending 4 times all the info
		sendOverlapZones(sectionIndex, false);
		receiveOverlapData(sectionIndex, false);
		clearRequests(false);
		
		// all nodes must finish receiving max values before receiving current values
		MPI_Barrier(MPI_COMM_WORLD);

		sendGhostAgents(sectionIndex);
		receiveGhostAgents(sectionIndex);
	}
#endif
	for(_step=0; _step<_simulation.getNumSteps(); _step++)
	{
		step();
	}
	// storing last step data
	serializeRasters();
	serializeAgents();
	
	log_INFO(logName.str(), getWallTime() << " closing files");
#ifdef PANDORAMPI
	GeneralState::serializer().finish();
	MpiFactory::instance()->cleanTypes();
#endif
	log_INFO(logName.str(), getWallTime() << " simulation finished");
#ifdef PANDORAMPI
	MPI_Finalize();
#endif
}

World::AgentsList::iterator World::getOwnedAgent( const std::string & id )
{
	for(AgentsList::iterator it=_agents.begin(); it!=_agents.end(); it++)
	{
		if((*it)->getId().compare(id)==0)
		{
			return it;
		}
	}
	return _agents.end();
}

World::AgentsList::iterator World::getGhostAgent( const std::string & id )
{
	for(AgentsList::iterator it=_overlapAgents.begin(); it!=_overlapAgents.end(); it++)		
	{
		if((*it)->getId().compare(id)==0)
		{
			return it;
		}
	}	
	return _overlapAgents.end();
}

Agent * World::getAgent( const std::string & id )
{
	AgentsList::iterator it = getOwnedAgent(id);
	if(it!=_agents.end())
	{
		Agent * agent = (*it);
		if(agent->exists())
		{
			return agent;
		}
	}
	it = getGhostAgent(id);
	if(it!=_overlapAgents.end())
	{
		Agent * agent = (*it);
		if(agent->exists())
		{
			return agent;
		}
	}
	return 0;
}

World::AgentsVector World::getAgent( const Point2D<int> & position, const std::string & type )
{
	std::vector<Agent *> result;
	for(AgentsList::iterator it=_agents.begin(); it!=_agents.end(); it++)
	{
		if((*it)->getPosition().isEqual(position))
		{
			if(type.compare("all")==0 || (*it)->isType(type))
			{
				result.push_back(*it);
			}
		}
	}
	for(AgentsList::iterator it=_overlapAgents.begin(); it!=_overlapAgents.end(); it++)		
	{
		if((*it)->getPosition().isEqual(position))
		{	
			if(type.compare("all")==0 || (*it)->isType(type))
			{
				result.push_back(*it);
			}
		}
	}
	return result;
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

	for(int d=0; d<_rasters.size(); d++)
	{
		if(!_rasters.at(d) || !_serializeRasters.at(d) || !_dynamicRasters.at(d))
		{
			continue;
		}
#ifdef PANDORAMPI
		GeneralState::serializer().serializeRaster(d, (Raster&)(*_rasters.at(d)), *this, _step);
#endif
	}
}

void World::serializeStaticRasters()
{
	for(int d=0; d<_rasters.size(); d++)
	{
		if(!_rasters.at(d) || !_serializeRasters.at(d) || _dynamicRasters.at(d))			
		{
			continue;
		}
#ifdef PANDORAMPI
		GeneralState::serializer().serializeStaticRaster(d, *_rasters.at(d), *this);
#endif
	}
}

void World::stepEnvironment()
{
	for(int d=0; d<_rasters.size(); d++)
	{
		if(!_rasters.at(d) || !_dynamicRasters.at(d))
		{
			continue;
		}
		// TODO initial pos and matrix size are needed?
		stepRaster(d);
	}
}

void World::stepRastersSection(  const int & indexSection, const Rectangle<int> & section )
{
}

void World::stepRaster( const int & index  )
{
	((Raster*)_rasters.at(index))->updateRasterIncrement();
}

void World::stepAgents()
{
}

void World::registerDynamicRaster( const std::string & key, const bool & serialize, int index,  Engine::Point2D<int> size )
{
	// if no index is provided, add one at the end
	if(index==-1)
	{
		index = _rasters.size();
	}
	
	if(_rasters.size()<=index)
	{
		int oldSize = _rasters.size();
		_rasters.resize(index+1);
		for(int i=oldSize; i<_rasters.size(); i++)
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
	if (size._x == -1 && size._y == -1) 
	{ 
		size = _overlapBoundaries._size;
	}
	_rasters.at(index)->resize(size);
	_serializeRasters.at(index) = serialize;
}

void World::registerStaticRaster( const std::string & key, const bool & serialize, int index, Engine::Point2D<int> size )
{
	// if no index is provided, add one at the end
	if(index==-1)
	{
		index = _rasters.size();
	}
	
	if(_rasters.size()<=index)
	{	
		int oldSize = _rasters.size();
		_rasters.resize(index+1);
		for(int i=oldSize; i<_rasters.size(); i++)
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
	if (size._x == -1 && size._y == -1) 
	{ 
		size = _overlapBoundaries._size;
	}
	_rasters.at(index)->resize(size);
	
	_dynamicRasters.at(index) = false;
	_serializeRasters.at(index) = serialize;
}


bool World::checkPosition( const Point2D<int> & newPosition )
{
	// checking global boundaries: if environment is a border of the real world
	if(!_globalBoundaries.isInside(newPosition))
	{
		//std::cout << " not valid position: " << newPosition << std::endl;
		return false;
	}

	if(_allowMultipleAgentsPerCell)
	{
		return true;
	}
	
	// checking if it is already occupied
	std::vector<Agent *> hosts = getAgent(newPosition);
	if(hosts.size()==0)
	{
		return true;
	}
	for(int i=0; i<hosts.size(); i++)
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

StaticRaster & World::getStaticRaster( const int & index )
{
	return *(_rasters.at(index));
}

StaticRaster & World::getStaticRasterStr( const std::string & key )
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	return getStaticRaster(it->second);
}

Raster & World::getDynamicRaster( const int & index)
{
	if(index>=_rasters.size())
	{
		std::stringstream oss;
		oss << "World::getDynamicRaster - index: " << index << " out of bound with size: " << _rasters.size();
		throw Exception(oss.str());
	}
	return (Raster &)*(_rasters.at(index));
}

Raster & World::getDynamicRasterStr( const std::string & key )
{	
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	if(it==_rasterNames.end())
	{
		std::stringstream oss;
		oss << "World::getDynamicRasterStr - raster: " << key << " not registered";
		throw Exception(oss.str());
	}
	return getDynamicRaster(it->second);
}

const Raster & World::getConstDynamicRaster( const int & index ) const
{	
	return (const Raster &)*(_rasters.at(index));
}

void World::setValueStr( const std::string & key, const Point2D<int> & position, int value )
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	setValue(it->second, position, value);
}

void World::setValue( const int & index, const Point2D<int> & position, int value )
{
	Point2D<int> localPosition(position - _overlapBoundaries._origin);
	((Raster*)_rasters.at(index))->setValue(localPosition, value);
}

int World::getValueStr( const std::string & key, const Point2D<int> & position ) const
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	return getValue(it->second, position);
}

int World::getValue( const int & index, const Point2D<int> & position ) const
{
	Point2D<int> localPosition(position - _overlapBoundaries._origin);
	return _rasters.at(index)->getValue(localPosition);
}

void World::setMaxValue( const std::string & key, const Point2D<int> & position, int value )
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	setMaxValue(it->second, position, value);
}

void World::setMaxValue( const int & index, const Point2D<int> & position, int value )
{
	Point2D<int> localPosition(position - _overlapBoundaries._origin);
	((Raster*)_rasters.at(index))->setMaxValue(localPosition, value);
}

int World::getMaxValueAt( const std::string & key, const Point2D<int> & position )
{
	RasterNameMap::const_iterator it = _rasterNames.find(key);
	return getMaxValueAt(it->second, position);
}

int World::getMaxValueAt( const int & index, const Point2D<int> & position )
{
	std::stringstream logName;
	logName << "max_value_" << _simulation.getId();
	Point2D<int> localPosition(position - _overlapBoundaries._origin);	
	log_DEBUG(logName.str(), getWallTime() << " accessing to pos: " << position << " real: " << localPosition << " with overlap: " << _overlapBoundaries << " for index: " << index);
	return ((Raster*)_rasters.at(index))->getMaxValueAt(localPosition);
}

const Rectangle<int> & World::getBoundaries() const
{
	return _boundaries;
}

const Rectangle<int> & World::getOverlapBoundaries() const
{
	return _overlapBoundaries;
}


bool World::isCorner( const int & neighbor ) const
{
	Point2D<int> worldPos = getPositionFromId(neighbor);
	Point2D<int> diff = worldPos - _worldPos;
	if(std::abs(diff._x)==1 && std::abs(diff._y)==1)
	{
		return true;
	}
	return false;
}

Rectangle<int> World::getInternalOverlap( const int & id ) const
{
	Point2D<int> diff = getPositionFromId(id)-_worldPos;
	// left
	
	Rectangle<int> result;

	// origin
	if(diff._x==-1)
	{
		if(_boundaries._origin._x==0)
		{
			result._origin._x = 0;
		}
		else
		{
			result._origin._x = _overlap;
		}
	}
	else if(diff._x==0)
	{
		if(_boundaries._origin._x==0)
		{
			result._origin._x = 0;
		}
		else
		{
			result._origin._x = _overlap;
		}
	}
	else
	{
		// if left border just remove an overlap
		if(_boundaries._origin._x==0)
		{
			result._origin._x = _boundaries._size._x - _overlap;
		}
		// else sum and remove an overlap
		else
		{
			result._origin._x = _boundaries._size._x;
		}
	}
	
	if(diff._y==-1)
	{
		if(_boundaries._origin._y==0)
		{
			result._origin._y = 0;
		}
		else
		{
			result._origin._y = _overlap;
		}
	}
	else if(diff._y==0)
	{
		if( _boundaries._origin._y==0)
		{
			result._origin._y = 0;
		}
		else
		{
			result._origin._y = _overlap;
		}
	}
	else
	{	
		// if top border just remove an overlap
		if(_boundaries._origin._y==0)
		{
			result._origin._y = _boundaries._size._y - _overlap;
		}
		// else sum and remove an overlap
		else
		{
			result._origin._y = _boundaries._size._y;
		}
	}

	// size
	if(diff._x!=0)
	{
		result._size._x = _overlap;
	}
	else
	{
		result._size._x = _boundaries._size._x;
	}
	
	if(diff._y!=0)
	{
		result._size._y = _overlap;
	}
	else
	{
		result._size._y = _boundaries._size._y;
	}
	std::stringstream logName;
	logName << "MPI_raster_world_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " internal overlap with: " << id << " and diff: " << diff << " - " << result);
	return result;
}

Rectangle<int> World::getExternalOverlap( const int & id) const
{
	Point2D<int> diff = getPositionFromId(id)-_worldPos;
	// left
	
	Rectangle<int> result;

	// origin
	if(diff._x==-1)
	{
		result._origin._x = 0;
	}
	else if(diff._x==0)
	{
		if(_boundaries._origin._x == 0)
		{
			result._origin._x = 0;
		}
		else
		{
			result._origin._x = _overlap;
		}
	}
	else
	{	
		// if left border it doesn't have a left overlap		
		if(_boundaries._origin._x==0)
		{
			result._origin._x = _boundaries._size._x;
		}
		// else sum an overlap
		else
		{
			result._origin._x = _boundaries._size._x + _overlap;
		}
	}
	
	if(diff._y==-1)
	{	
		result._origin._y = 0;
	}
	else if(diff._y==0)
	{
		if(_boundaries._origin._y == 0)
		{
			result._origin._y = 0;
		}
		else
		{
			result._origin._y = _overlap;
		}
	}
	else
	{	
		// if left border it doesn't have a left overlap		
		if(_boundaries._origin._y==0)
		{
			result._origin._y = _boundaries._size._y;
		}
		// else sum an overlap
		else
		{
			result._origin._y = _boundaries._size._y + _overlap;
		}
	}

	// size
	if(diff._x!=0)
	{
		result._size._x = _overlap;
	}
	else
	{
		result._size._x = _boundaries._size._x;
	}
	
	if(diff._y!=0)
	{
		result._size._y = _overlap;
	}
	else
	{	
		result._size._y = _boundaries._size._y;
	}
	
	std::stringstream logName;
	logName << "MPI_raster_world_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " external overlap with: " << id << " and diff: " << diff << " - " << result);
	return result;
}

Rectangle<int> World::getOverlap( const int & id, const int & sectionIndex) const	
{
	Point2D<int> diff = getPositionFromId(id)-_worldPos;
	// left
	
	Rectangle<int> result;

	// origin
	if(diff._x==-1)
	{
		result._origin._x = 0;
	}
	else if(diff._x==0)
	{
		if(sectionIndex==0 || sectionIndex==2)
		{
			result._origin._x = 0;
		}
		else
		{
			result._origin._x = _boundaries._size._x/2;	
			if(_boundaries._origin._x+_boundaries._size._x!=_globalBoundaries._size._x)
			{
				result._origin._x -= _overlap;
			}
		}
	}
	else
	{
		// if left border just remove an overlap
		if(_boundaries._origin._x==0)
		{
			result._origin._x = _boundaries._size._x - _overlap;
		}
		// else sum and remove an overlap
		else
		{
			result._origin._x = _boundaries._size._x;
		}
	}
	
	if(diff._y==-1)
	{
		result._origin._y = 0;
	}
	else if(diff._y==0)
	{
		if(sectionIndex==0 || sectionIndex==1)
		{
			result._origin._y = 0;
		}
		else
		{
			result._origin._y = _boundaries._size._y/2;
			if(_boundaries._origin._y+_boundaries._size._y!=_globalBoundaries._size._y)
			{
				result._origin._y -= _overlap;
			}
		}
	}
	else
	{
		// if top border just remove an overlap
		if(_boundaries._origin._y==0)
		{
			result._origin._y = _boundaries._size._y - _overlap;
		}
		// else sum and remove an overlap
		else
		{
			result._origin._y = _boundaries._size._y;
		}
	}

	// size
	if(diff._x!=0)
	{
		result._size._x = _overlap*2;
	}
	else
	{
		// this result is = to boundaries + 1 overlap
		result._size._x = _boundaries._size._x/2;
		// border
		if(_boundaries._origin._x!=0)
		{
			result._size._x += _overlap;
		}
		else
		{
			if(sectionIndex!=0 && sectionIndex!=2)
			{
				result._size._x += _overlap;
			}
		}

		if(_boundaries._origin._x+_boundaries._size._x!=_globalBoundaries._size._x)
		{
			result._size._x += _overlap;
		}
		else
		{
			if(sectionIndex!=1 && sectionIndex!=3)
			{
				result._size._x += _overlap;
			}
		}
	}
	
	if(diff._y!=0)
	{
		result._size._y = _overlap*2;
	}
	else
	{
		result._size._y = _boundaries._size._y/2;
		if(_boundaries._origin._y!=0)
		{
			result._size._y += _overlap;
		}
		else
		{
			if(sectionIndex!=0 && sectionIndex!=1)
			{
				result._size._y += _overlap;
			}
		}

		if(_boundaries._origin._y+_boundaries._size._y!=_globalBoundaries._size._y)
		{
			result._size._y += _overlap;
		}
		else
		{
			if(sectionIndex!=2 && sectionIndex!=3)
			{
				result._size._y += _overlap;
			}
		}
	}
	std::stringstream logName;
	logName << "MPI_raster_world_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " overlap with: " << id << " and diff: " << diff << " in section Index: " << sectionIndex << " - " << result);
	return result;
}

bool World::needsToBeUpdated( const int & id, const int & sectionIndex )
{	
	Point2D<int> diff = getPositionFromId(id)-_worldPos;
	switch(sectionIndex)
	{
		case 0:
			// top left
			if(diff._x==-1 && diff._y==-1)
			{
				return true;
			}
			// left
			if(diff._x==-1 && diff._y==0)
			{
				return true;
			}
			// top
			if(diff._x==0 && diff._y==-1)
			{
				return true;
			}
			return false;
			break;
		case 1:	
			// top
			if(diff._x==0 && diff._y==-1)
			{
				return true;
			}
			// top right
			if(diff._x==1 && diff._y==-1)
			{
				return true;
			}
			// right
			if(diff._x==1 && diff._y==0)
			{
				return true;
			}
			return false;
			break;
		case 2:	
			// bottom
			if(diff._x==0 && diff._y==1)
			{
				return true;
			}
			// bottom left
			if(diff._x==-1 && diff._y==1)
			{
				return true;
			}
			// left
			if(diff._x==-1 && diff._y==0)
			{
				return true;
			}
			return false;
			break;
		case 3:	
			// right
			if(diff._x==1 && diff._y==0)
			{
				return true;
			}
			// bottom right
			if(diff._x==1 && diff._y==1)
			{
				return true;
			}
			// bottom
			if(diff._x==0 && diff._y==1)
			{
				return true;
			}
			return false;
			break;
		default:
			std::stringstream oss;
			oss << "World::needsToBeUpdated - wrong section index: " << sectionIndex;
			throw Engine::Exception(oss.str());
	}
	return false;
}

bool World::needsToReceiveData( const int & id, const int & sectionIndex )
{	
	Point2D<int> diff = getPositionFromId(id)-_worldPos;
	switch(sectionIndex)
	{
		case 0:
			// bottom
			if(diff._x==0 && diff._y==1)
			{
				return true;
			}
			// right
			if(diff._x==1 && diff._y==0)
			{
				return true;
			}
			// bottom right
			if(diff._x==1 && diff._y==1)
			{
				return true;
			}
			return false;
			break;
		case 1:	
			// left
			if(diff._x==-1 && diff._y==0)
			{
				return true;
			}
			// bottom left
			if(diff._x==-1 && diff._y==1)
			{
				return true;
			}
			// bottom
			if(diff._x==0 && diff._y==1)
			{
				return true;
			}
			return false;
			break;
		case 2:	
			// top
			if(diff._x==0 && diff._y==-1)
			{
				return true;
			}
			// top right
			if(diff._x==1 && diff._y==-1)
			{
				return true;
			}
			// right
			if(diff._x==1 && diff._y==0)
			{
				return true;
			}
			return false;
			break;
		case 3:	
			// top left
			if(diff._x==-1 && diff._y==-1)
			{
				return true;
			}
			// left
			if(diff._x==-1 && diff._y==0)
			{
				return true;
			}
			// top
			if(diff._x==0 && diff._y==-1)
			{
				return true;
			}
			return false;
			break;
		default:
			std::stringstream oss;
			oss << "World::needsToReceiveData - wrong section index: " << sectionIndex;
			throw Engine::Exception(oss.str());
	}
	return false;
}

int World::getId() const
{
	return _simulation.getId();
}

void World::removeAgent( Agent * agent )
{
	//std::cout << _simulation.getId() << " removing agent: " << agent << std::endl;
	AgentsList::iterator it = getOwnedAgent(agent->getId());
	if(it==_agents.end())
	{
		std::stringstream oss;
		oss << "World::removeAgent - agent: " << agent << " not found";
		throw Exception(oss.str());
	}
	// TODO it is not needed if it has modified position, as it is already done after the executed of a given agent step
	//sendDeleteOverlapAgent(it, agent->getPosition());
	_removedAgents.push_back(*it);
}

void World::removeAgents()
{
	//std::cout << _simulation.getId() << " execution agents removal: " << _removedAgents.size() << std::endl;
	AgentsList::iterator it=_removedAgents.begin();
	while(it!=_removedAgents.end())
	{
		Agent * agent = *it;
		//std::cout << "removing agent: " << agent << std::endl;
		AgentsList::iterator itAg = getOwnedAgent(agent->getId());
		if(itAg==_agents.end())
		{
			std::stringstream oss;
			oss << "World::removeAgents - agent: " << agent << " not found";
			throw Exception(oss.str());
			return;
		}
		_agents.erase(itAg);
		it = _removedAgents.erase(it);
		delete agent;
	}
	_removedAgents.clear();
}

void World::setSearchAgents( const bool & searchAgents )
{
	_searchAgents = searchAgents;
}

bool World::getSearchAgents()
{
	return _searchAgents;
}

int World::countNeighbours( Agent * target, const double & radius, const std::string & type )
{
	int numAgents = for_each(_agents.begin(), _agents.end(), aggregatorCount<Engine::Agent>(radius,*target, type))._count;
	int numOverlapAgents = for_each(_overlapAgents.begin(), _overlapAgents.end(), aggregatorCount<Engine::Agent>(radius,*target, type))._count;
	return numAgents+numOverlapAgents;
}

World::AgentsVector World::getNeighbours( Agent * target, const double & radius, const std::string & type )
{
	AgentsVector agentsVector = for_each(_agents.begin(), _agents.end(), aggregatorGet<Engine::Agent>(radius,*target, type))._neighbors;
	AgentsVector overlapAgentsVector =  for_each(_overlapAgents.begin(), _overlapAgents.end(), aggregatorGet<Engine::Agent>(radius,*target, type))._neighbors;
	std::copy(overlapAgentsVector.begin(), overlapAgentsVector.end(), std::back_inserter(agentsVector));
	std::random_shuffle(agentsVector.begin(), agentsVector.end());
	return agentsVector;
}

const int & World::getOverlap()
{
	return _overlap;
}

Point2D<int> World::getRandomPosition()
{
	while(1)
	{
		Engine::Point2D<int> pos(GeneralState::statistics().getUniformDistValue(0,_boundaries._size._x-1), GeneralState::statistics().getUniformDistValue(0,_boundaries._size._y-1));
		pos += _boundaries._origin;
		if(checkPosition(pos) && _boundaries.isInside(pos))
		{
			return pos;
		}
	}
}

double World::getWallTime() const
{
#ifdef PANDORAMPI
	return MPI_Wtime() - _initialTime;
#else
	return time(0) - _initialTime;
#endif
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

} // namespace Engine

