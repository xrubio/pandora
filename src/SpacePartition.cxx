/*
 * Copyright (c) 2014
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

#include <SpacePartition.hxx>
#include <Agent.hxx>
#include <MpiFactory.hxx>
#include <Logger.hxx>
#include <Exception.hxx>
#include <Config.hxx>

namespace Engine
{

SpacePartition::SpacePartition( const int & overlap, bool finalize ) : _serializer(*this), _worldPos(-1,-1), _overlap(overlap), _finalize(finalize), _initialTime(0.0f)
{
}

SpacePartition::~SpacePartition()
{
}

void SpacePartition::init( int argc, char *argv[] )
{
	int alreadyInitialized;
	MPI_Initialized(&alreadyInitialized);
	if(!alreadyInitialized)
	{
		MPI_Init(&argc, &argv);
	}
	_initialTime = getWallTime();

	MPI_Comm_size(MPI_COMM_WORLD, &_numTasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&_id);	
	std::cout << "simulation: " << _id << " of: " << _numTasks << " initialized" << std::endl;
	stablishBoundaries();
}

void SpacePartition::initData()
{
	// serializer init
	_serializer.init(*_world);

	// mpi type registering
	MpiFactory::instance()->registerTypes();
	initOverlappingData();

	std::stringstream logName;
	logName << "simulation_" << _id;
	log_INFO(logName.str(), "finished init at: "  << getWallTime());
}

void SpacePartition::checkOverlapSize()
{
	int subfieldSizeX = _ownedArea._size._width/2;
	int subfieldSizeY = _ownedArea._size._height/2;
	if(_overlap*2>subfieldSizeX || _overlap*2>subfieldSizeY)
	{
		std::stringstream oss;
		oss << "SpacePartition::checkOverlapSize- subfield sizes: " << subfieldSizeX << "/" << subfieldSizeY << " from global: " << _world->getConfig().getSize() << " and owned area: " << _ownedArea << " must be at least twice the value of overlap: " << _overlap << " to avoid conflicts between non adjacent subfields";
		throw Exception(oss.str());
	}
}

void SpacePartition::stablishBoundaries()
{
	// position of world related to the complete set of computer nodes
	int worldsPerRow = sqrt(_numTasks);
	_worldPos = getPositionFromId(_id);

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
	// owned area inside global coordinates, depending on worldPos
	_ownedArea._size._width = _world->getConfig().getSize()._width/sqrt(_numTasks);
	_ownedArea._size._height = _world->getConfig().getSize()._height/sqrt(_numTasks);
	_ownedArea._origin._x = _worldPos._x*_ownedArea._size._width;
	_ownedArea._origin._y = _worldPos._y*_ownedArea._size._height;

	// defining overlap boundaries
	_boundaries = _ownedArea;
	// west boundary
	if(_ownedArea._origin._x!=0)
	{
		_boundaries._origin._x -= _overlap;
		_boundaries._size._width += _overlap;
	}
	// east boundary
	if(_ownedArea._origin._x!=_world->getConfig().getSize()._width-_ownedArea._size._width)
	{
		_boundaries._size._width += _overlap;
	}
	// north boundary
	if(_ownedArea._origin._y!=0)
	{
		_boundaries._origin._y -= _overlap;
		_boundaries._size._height += _overlap;
	}
	// south boundary
	if(_ownedArea._origin._y!=_world->getConfig().getSize()._height-_ownedArea._size._height)
	{
		_boundaries._size._height += _overlap;
	}
	// creating sections
	_sections.resize(4);
	_sections[0] = Rectangle<int>(_ownedArea._size/2, _ownedArea._origin);
	_sections[1] = Rectangle<int>(_ownedArea._size/2, Point2D<int>(_ownedArea._origin._x+_ownedArea._size._width/2, _ownedArea._origin._y));
	_sections[2] = Rectangle<int>(_ownedArea._size/2, Point2D<int>(_ownedArea._origin._x, _ownedArea._origin._y+_ownedArea._size._height/2));
	_sections[3] = Rectangle<int>(_ownedArea._size/2, Point2D<int>(_ownedArea._origin._x+_ownedArea._size._width/2, _ownedArea._origin._y+_ownedArea._size._height/2));

	if(_ownedArea._size._width%2!=0 || _ownedArea._size._height%2!=0)
	{
		std::stringstream oss;
		oss << "SpacePartition::init - local raster size: " << _ownedArea._size << " must be divisible by 2";
		throw Exception(oss.str());
	}
	checkOverlapSize();

	std::stringstream logName;
	logName << "simulation_" << _id;
	log_INFO(logName.str(), getWallTime() << " pos: " << _worldPos << ", global size: " << _world->getConfig().getSize() << ", boundaries: " << _boundaries << " and owned area: " << _ownedArea);
	log_INFO(logName.str(), getWallTime() << " sections 0: " << _sections[0] << " - 1: " << _sections[1] << " - 2:" << _sections[2] << " - 3: " << _sections[3]);
}

void SpacePartition::stepSection( const int & sectionIndex )
{
	std::stringstream logName;
	logName << "simulation_" << _id;
	log_DEBUG(logName.str(), getWallTime() << " beginning step: " << _world->getCurrentStep() << " section: " << sectionIndex);
	
    AgentsList::iterator it = _world->beginAgents();
	AgentsVector agentsToExecute;
	// we have to randomize the execution of agents in a given section index
	while(it!=_world->endAgents())
	{
        AgentPtr agent = *it;
		if(_sections[sectionIndex].contains(agent->getPosition()) && !hasBeenExecuted(agent->getId()))
		{
            log_DEBUG(logName.str(), "agent to execute: :" << agent);
			agentsToExecute.push_back(agent);
		}
		it++;
	}
	std::random_shuffle(agentsToExecute.begin(), agentsToExecute.end());
	int numExecutedAgents = 0;
	AgentsList agentsToSend;

#ifndef PANDORAEDEBUG
	// shared memory distibution for read-only planning actions, disabled for extreme debug
	#pragma omp parallel for
#endif
	for(size_t i=0; i<agentsToExecute.size(); i++)
	{
        agentsToExecute.at(i)->updateKnowledge();
        agentsToExecute.at(i)->selectActions();
	}

	// execute actions
	for(size_t i=0; i<agentsToExecute.size(); i++)
	{
        AgentPtr agent = agentsToExecute.at(i);
		log_DEBUG(logName.str(), getWallTime() << " agent: " << agent << " being executed at index: " << sectionIndex << " of task: "<< _id << " in step: " << _world->getCurrentStep() );
		agentsToExecute.at(i)->executeActions();
		agentsToExecute.at(i)->updateState();
		log_DEBUG(logName.str(), getWallTime() << " agent: " << agent << " has been executed at index: " << sectionIndex << " of task: "<< _id << " in step: " << _world->getCurrentStep() );

		if(!_ownedArea.contains(agent->getPosition()) && !willBeRemoved(agent->getId()))
		{
			log_DEBUG(logName.str(), getWallTime() << " migrating agent: " << agent << " being executed at index: " << sectionIndex << " of task: "<< _id );
			agentsToSend.push_back(agent);

			// the agent is no longer property of this world
			AgentsList::iterator itErase  = getOwnedAgent(agent->getId());
			// it will be deleted
			_world->eraseAgent(itErase);
			_overlapAgents.push_back(agent);
			log_DEBUG(logName.str(), getWallTime() <<  "putting agent: " << agent << " to overlap");
		}
		else
		{
			log_DEBUG(logName.str(), getWallTime() << " finished agent: " << agent);
		}
		_executedAgentsHash.insert(make_pair(agent->getId(), agent));
		numExecutedAgents++;
		log_DEBUG(logName.str(), getWallTime()  << " num executed agents: " << numExecutedAgents );
	}
	log_DEBUG(logName.str(), getWallTime()  << " sending agents in section: " << sectionIndex << " and step: " << _world->getCurrentStep());
	sendAgents(agentsToSend);
	log_DEBUG(logName.str(), getWallTime() << " has finished section: " << sectionIndex << " and step: " << _world->getCurrentStep());
	
	log_DEBUG(logName.str(), getWallTime() << " executed step: " << _world->getCurrentStep() << " section: " << sectionIndex << " in zone: " << _sections[sectionIndex] << " with num executed agents: " << numExecutedAgents << " total agents: " << std::distance(_world->beginAgents(), _world->endAgents()) << " and overlap agents: " << _overlapAgents.size());
}

void SpacePartition::sendAgents( AgentsList & agentsToSend )
{
	if(_neighbors.size()==0)
	{
		return;
	}
	std::stringstream logName;
	logName << "MPI_agents_world_" << _id;
	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " sendAgent: " << agentsToSend.size() << " agents");

	// for each neighbor, we send the number of agents to send
	for(MpiFactory::TypesMap::iterator itType=MpiFactory::instance()->beginTypes(); itType!=MpiFactory::instance()->endTypes(); itType++)
	{
		log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " sendAgent - checking mpi type: " << itType->first );
		// add each agent to the list of the neighbour where it will be sent
		std::vector< AgentsList > agentsToNeighbors;
		agentsToNeighbors.resize(_neighbors.size());


		for(AgentsList::iterator it=agentsToSend.begin(); it!=agentsToSend.end(); it++)
		{
			AgentPtr agent = *it;
			if(agent->isType(itType->first))
			{
				int newID = getIdFromPosition(agent->getPosition());
				agentsToNeighbors[getNeighborIndex(newID)].push_back(agent);
			}
		}

		MPI_Datatype * agentType = itType->second;
		for(size_t i=0; i<_neighbors.size(); i++)
		{	
			int numAgents = agentsToNeighbors[i].size();
			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " sendAgent - sending num agents: " << numAgents << " to: " << _neighbors[i]);
			int error = MPI_Send(&numAgents, 1, MPI_INTEGER, _neighbors[i], eNumAgents, MPI_COMM_WORLD);
			if(error != MPI_SUCCESS)
			{
				std::stringstream oss;
				oss << "SpacePartition::sendAgents - " << _id << " error in MPI_Send : " << error;
				throw Exception(oss.str());
			}
			AgentsList::iterator it= agentsToNeighbors[i].begin();
			while(it!=agentsToNeighbors[i].end())
			{
				Agent * agent = it->get();
				void * package = agent->fillPackage();
				log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " sendAgent - sending agent: " << *it << " to: " << _neighbors[i] );
				error = MPI_Send(package, 1, *agentType, _neighbors[i], eAgent, MPI_COMM_WORLD);
				delete package;
				if(error != MPI_SUCCESS)
				{
					std::stringstream oss;
					oss << "SpacePartition::sendAgents - " << _id << " error in MPI_Send : " << error;
					throw Exception(oss.str());
				}
				agent->sendVectorAttributes(_neighbors[i]);
				// it is not deleted, as it is sent to overlap list
				it = agentsToNeighbors[i].erase(it);
			}
		}
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " sendAgent -  end checking agents to send: " << agentsToSend.size());
}

void SpacePartition::sendOverlapZones( const int & sectionIndex, const bool & entireOverlap )
{
	std::stringstream logName;
	logName << "MPI_raster_world_" << _id;
	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " sendOverlapZones");
	std::vector<int> neighborsToUpdate;
	
	for(size_t i=0; i<_neighbors.size(); i++)
	{
		if(needsToBeUpdated(_neighbors[i], sectionIndex))
		{
			neighborsToUpdate.push_back(_neighbors[i]);
		}
	}

	for(size_t d=0; d<_world->getNumberOfRasters(); d++)
	{	
		if(!_world->rasterExists(d) || !_world->isRasterDynamic(d))
		{
			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " index: " << d << " not sending");
			continue;
		}
		log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " sending raster: " << d);
		for(size_t i=0; i<neighborsToUpdate.size(); i++)
		{
			MpiOverlap * send = new MpiOverlap;
			if(entireOverlap)
			{
				send->_overlap= getOverlap(neighborsToUpdate[i], sectionIndex);
				log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " send entire overlap: " << send->_overlap << " to: " << neighborsToUpdate[i]);
			}
			else
			{
				send->_overlap = getInternalOverlap(neighborsToUpdate[i]);
				log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " send partial overlap: " << send->_overlap << " to: " << neighborsToUpdate[i]);
			}
			const Rectangle<int> & overlapZone = send->_overlap;
			send->_data.resize(overlapZone._size._width * overlapZone._size._height);
			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " will send overlap to: " << neighborsToUpdate[i] << " with size: " << send->_data.size() << " and zone: " << overlapZone);
			for(size_t n=0; n<send->_data.size(); n++)
			{
				Point2D<int> index(overlapZone._origin._x+n%overlapZone._size._width, overlapZone._origin._y+n/overlapZone._size._width);
				send->_data.at(n) = _world->getDynamicRaster(d).getValue(index);
				log_EDEBUG(logName.str(), "\t" << getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " send index: " << index << " in global pos: " << index+_boundaries._origin << " value: " << send->_data.at(n));
			}
			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " raster: " << d << " will be sent");
			MPI_Isend(&send->_data[0], send->_data.size(), MPI_INTEGER, neighborsToUpdate[i], eRasterData, MPI_COMM_WORLD, &send->_request);
			_sendRequests.push_back(send);
			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " raster: " << d << " data sent to: " << _neighbors[i]);
		}
		log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " raster: " << d << " sent");
	}
	log_DEBUG(logName.str(), getWallTime() << " step: "  << "/" << sectionIndex << _world->getCurrentStep() << " sendOverlapZones ended");
}

void SpacePartition::sendMaxOverlapZones()
{
	std::stringstream logName;
	logName << "MPI_raster_world_" << _id;
	log_DEBUG(logName.str(), getWallTime() << " step: "  << _world->getCurrentStep() << " sendMaxOverlapZones");
	for(size_t d=0; d<_world->getNumberOfRasters(); d++)
	{	
		if(!_world->rasterExists(d) || !_world->isRasterDynamic(d))
		{
			continue;
		}

		log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " sending max raster: " << d);
		for(size_t i=0; i<_neighbors.size(); i++)
		{
			MpiOverlap * send = new MpiOverlap;
			send->_overlap = getInternalOverlap(_neighbors[i]);
			send->_data.resize(send->_overlap._size._width * send->_overlap._size._height);
			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " will send max overlap of: " << d << " to: " << _neighbors[i] << " with size: " << send->_data.size() << " and zone: " << send->_overlap << " to " << _neighbors[i]);
			const Rectangle<int> & overlapZone = send->_overlap;
			for(size_t n=0; n<send->_data.size(); n++)
			{
				Point2D<int> index(overlapZone._origin._x+n%overlapZone._size._width, overlapZone._origin._y+n/overlapZone._size._width);
				send->_data.at(n) = _world->getDynamicRaster(d).getMaxValue(index);
				log_EDEBUG(logName.str(), "\t" << getWallTime() << " step: " << _world->getCurrentStep() << " send index: " << index << " in global pos: " << index+_boundaries._origin << " max value: " << send->_data.at(n));
			}
			MPI_Isend(&send->_data[0], send->_data.size(), MPI_INTEGER, _neighbors[i], eRasterMaxData, MPI_COMM_WORLD, &send->_request);
			_sendRequests.push_back(send);
			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " raster: " << d << " max data sent to: " << _neighbors[i]);
		}
		log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " raster: " << d << " max data sent");
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " sendMaxOverlapZones ended");
}

void SpacePartition::sendGhostAgents( const int & sectionIndex )
{
	std::stringstream logName;
	logName << "MPI_agents_world_" << _id;
	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " send ghost agents for section index: " << sectionIndex );

	std::vector<int> neighborsToUpdate;
	for(size_t i=0; i<_neighbors.size(); i++)
	{
		if(needsToBeUpdated(_neighbors[i], sectionIndex))
		{
			neighborsToUpdate.push_back(_neighbors[i]);
			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " section index: " << sectionIndex << " will send overlap to: " << _neighbors[i]);
		}
	}

	// for each type of agent we will send the collection of agents of the particular type to neighbors
	for(MpiFactory::TypesMap::iterator itType=MpiFactory::instance()->beginTypes(); itType!=MpiFactory::instance()->endTypes(); itType++)
	{
		log_DEBUG(logName.str(),  getWallTime() << " step: " << _world->getCurrentStep() << " section index: " << sectionIndex << " checking type: " << itType->first );
		MPI_Datatype * agentType = itType->second;

		std::vector< AgentsList > agentsToNeighbors;
		agentsToNeighbors.resize(neighborsToUpdate.size());

		for(size_t i=0; i<neighborsToUpdate.size(); i++)
		{
			Rectangle<int> overlapZone = getOverlap(neighborsToUpdate[i], sectionIndex);
			for(AgentsList::iterator it=_world->beginAgents(); it!=_world->endAgents(); it++)
			{
				AgentPtr agent = *it;
				// we check the type. TODO register the type in another string
				// TODO refactor!!!
				log_DEBUG(logName.str(),  getWallTime() << " step: " << _world->getCurrentStep() << " agent: " << agent << " of type: " << itType->first << " test will be removed: " << willBeRemoved(agent->getId()) << " checking overlap zone: " << overlapZone << " overlap boundaries: " << _boundaries << " - test is inside zone: " << overlapZone.contains(agent->getPosition()-_boundaries._origin));
				if(agent->isType(itType->first))
				{
					if((!willBeRemoved(agent->getId())) && (overlapZone.contains(agent->getPosition()-_boundaries._origin)))
					{
						agentsToNeighbors[i].push_back(agent);
						log_DEBUG(logName.str(),  getWallTime() << " step: " << _world->getCurrentStep() << " sending ghost agent: " << agent << " to: " << neighborsToUpdate[i] << " in section index: " << sectionIndex);
					}
				}
			}
			for(AgentsList::iterator it=_overlapAgents.begin(); it!=_overlapAgents.end(); it++)
			{
				AgentPtr agent = *it;	
				if(agent->isType(itType->first))
				{
					if((!willBeRemoved(agent->getId())) && (overlapZone.contains(agent->getPosition()-_boundaries._origin)))
					{
						agentsToNeighbors[i].push_back(agent);
						log_DEBUG(logName.str(),  getWallTime() << " step: " << _world->getCurrentStep() << " will send modified ghost agent: " << agent << " to: " << neighborsToUpdate[i] << " in section index: " << sectionIndex << " and step: " << _world->getCurrentStep());
					}
				}
			}

			int numAgents = agentsToNeighbors[i].size();
			log_DEBUG(logName.str(),  getWallTime() << " step: " << _world->getCurrentStep() << " sending num ghost agents: " << numAgents << " to : " << neighborsToUpdate[i] << " in step: " << _world->getCurrentStep() << " and section index: " << sectionIndex );
			int error = MPI_Send(&numAgents, 1, MPI_INTEGER, neighborsToUpdate[i], eNumGhostAgents, MPI_COMM_WORLD);
			if(error != MPI_SUCCESS)
			{
				std::stringstream oss;
				oss << "SpacePartition::sendGhostAgents - " << _id << " error in MPI_Send : " << error;
				throw Exception(oss.str());
			}
			for(AgentsList::iterator it=agentsToNeighbors[i].begin(); it!=agentsToNeighbors[i].end(); it++)
			{
				Agent * agent = it->get();
				void * package = agent->fillPackage();
				log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " sending ghost agent: " << *it << " from: " << _id << " to: " << neighborsToUpdate[i]);
				error = MPI_Send(package, 1, *agentType, neighborsToUpdate[i], eGhostAgent, MPI_COMM_WORLD);
				delete package;
				if(error != MPI_SUCCESS)
				{
					std::stringstream oss;
					oss << "SpacePartition::sendGhostAgents - " << _id << " error in MPI_Send : " << error;
					throw Exception(oss.str());
				}
				agent->sendVectorAttributes(neighborsToUpdate[i]);
			}
		}
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " send ghost agents for section index: " << sectionIndex << " finished");
}

void SpacePartition::receiveGhostAgents( const int & sectionIndex )
{
	std::stringstream logName;
	logName << "MPI_agents_world_" << _id;
	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " receive ghost agents for section index: " << sectionIndex );


	// we need to calculate how many neighbors will send data to this id
	std::vector<int> neighborsToUpdate;
	for(size_t i=0; i<_neighbors.size(); i++)
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

		for(size_t i=0; i<neighborsToUpdate.size(); i++)
		{
            log_DEBUG(logName.str(), getWallTime() << " AA neigh to update: " << neighborsToUpdate[i] << " type: " << itType->first);
			AgentsList newGhostAgents;
			int numAgentsToReceive;
			MPI_Status status;

			int error = MPI_Recv(&numAgentsToReceive, 1, MPI_INTEGER, neighborsToUpdate[i], eNumGhostAgents, MPI_COMM_WORLD, &status);			
			if(error!=MPI_SUCCESS)
			{
				std::stringstream oss;
				oss << "SpacePartition::receiveGhostAgents - " << _id << " error in MPI_Recv: " << error;
				throw Exception(oss.str());
			}
			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " has received message from " << neighborsToUpdate[i] << ", num ghost agents: " << numAgentsToReceive );
			for(int j=0; j<numAgentsToReceive; j++)
			{
				void * package = MpiFactory::instance()->createDefaultPackage(itType->first);
				error = MPI_Recv(package, 1, *agentType, neighborsToUpdate[i], eGhostAgent, MPI_COMM_WORLD, &status);					
				if(error!=MPI_SUCCESS)
				{
					std::stringstream oss;
					oss << "SpacePartition::receiveGhostAgents - " << _id << " error in MPI_Recv: " << error;
					throw Exception(oss.str());
				}
				Agent * agent = MpiFactory::instance()->createAndFillAgent(itType->first, package);
				log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " has received ghost agent: " << agent << " number: " << j << " from: " << neighborsToUpdate[i] << " in section index: " << sectionIndex << " and step: " << _world->getCurrentStep() );
				delete package;
				agent->receiveVectorAttributes(neighborsToUpdate[i]);

				// we must check if it is an update of an agent, or a ghost agent
				bool worldOwnsAgent = false;
				AgentsList::iterator it = getOwnedAgent(agent->getId());
				if(it!=_world->endAgents())
				{
					log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " has received update of own agent: " << *it << " in step: " << _world->getCurrentStep() );
					_world->eraseAgent(it);
					_world->addAgent(agent, false);
					worldOwnsAgent = true;
				}
				if(!worldOwnsAgent)
				{
					newGhostAgents.push_back(std::shared_ptr<Agent>(agent));
				}
			}
			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " num ghost agents sent for neighbor: " << neighborsToUpdate[i] << " in section index: " << sectionIndex << ": " << newGhostAgents.size());
			// if the agent is in the zone to be updated, remove it
			Rectangle<int> overlapZone = getOverlap(neighborsToUpdate[i], sectionIndex);
			AgentsList::iterator it=_overlapAgents.begin();
			while(it!=_overlapAgents.end())
			{
				Agent * agent = it->get();
				if(agent->isType(itType->first))
				{
					// si l'agent no està en zona que s'ha d'actualitzar, continuar
					if(overlapZone.contains((*it)->getPosition()-_boundaries._origin))
					{
						log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " in section index: " << sectionIndex << " with overlap zone: " << overlapZone << " erasing agent: " << *it);
						it = _overlapAgents.erase(it);
					}
					else
					{
						log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " in section index: " << sectionIndex <<  " with overlap zone: " << overlapZone << " maintaining agent: " << *it );
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
                AgentPtr agent = *it;
				log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " in section index: " << sectionIndex << " adding ghost agent: " << agent);
				_overlapAgents.push_back(agent);
			}
		}
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " receive ghost agents for section index: " << sectionIndex << " finished");

}

void SpacePartition::receiveAgents( const int & sectionIndex )
{
	std::stringstream logName;
	logName << "MPI_agents_world_" << _id;
	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " receiving agents for section index: " << sectionIndex);
	for(MpiFactory::TypesMap::iterator itType=MpiFactory::instance()->beginTypes(); itType!=MpiFactory::instance()->endTypes(); itType++)
	{
		log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " receiveAgent - checking mpi type: " << itType->first);
		MPI_Datatype * agentType = itType->second;

		for(size_t i=0; i<_neighbors.size(); i++)
		{
			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " receiveAgent - checking mpi type: " << itType->first << " for neighbor: " << _neighbors[i] );

			int numAgentsToReceive;
			MPI_Status status;
			int error = MPI_Recv(&numAgentsToReceive, 1, MPI_INTEGER, _neighbors[i], eNumAgents, MPI_COMM_WORLD, &status);			
			if(error!=MPI_SUCCESS)
			{
				std::stringstream oss;
				oss << "SpacePartition::receiveAgents - " << _id << " error in MPI_Recv: " << error;
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
					oss << "SpacePartition::receiveAgents - " << _id << " error in MPI_Recv: " << error;
					throw Exception(oss.str());
				}
				Agent * agent = MpiFactory::instance()->createAndFillAgent(itType->first, package);
				delete package;
				agent->receiveVectorAttributes(_neighbors[i]);
				_world->addAgent(agent, true);
				log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " receiveAgents - received agent: " << agent << " number: " << j << " from: " << _neighbors[i]);
			}
		}
	}
}

void SpacePartition::receiveOverlapData( const int & sectionIndex, const bool & entireOverlap )
{
	std::stringstream logName;
	logName << "MPI_raster_world_" << _id;
	log_DEBUG(logName.str(), getWallTime() << " step: "  << "/" << sectionIndex << _world->getCurrentStep() << " receiveOverlapData");

	// we need to calculate how many neighbors will send data to this id
	std::vector<int> neighborsToUpdate;
	for(size_t i=0; i<_neighbors.size(); i++)
	{
		if(needsToReceiveData(_neighbors[i], sectionIndex))
		{
			neighborsToUpdate.push_back(_neighbors[i]);
		}
	}

	// for each raster, we receive data from all the active neighbors
	for(size_t d=0; d<_world->getNumberOfRasters(); d++)
	{	
		if(!_world->rasterExists(d) || !_world->isRasterDynamic(d))
		{
			continue;
		}
		
		log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " receiving raster: " << d);
		for(size_t i=0; i<neighborsToUpdate.size(); i++)
		{
			MpiOverlap* receive = new MpiOverlap;
			// TODO move to index
			receive->_rasterName = _world->getRasterName(d);
			if(entireOverlap)
			{
				receive->_overlap = getOverlap(neighborsToUpdate[i], sectionIndex);
				log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " receive entire overlap: " << receive->_overlap << " from " << neighborsToUpdate[i]);
			}
			else
			{
				receive->_overlap = getExternalOverlap(neighborsToUpdate[i]);
				log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " receive external overlap: " << receive->_overlap << " from " << neighborsToUpdate[i]);
			}
			receive->_data.resize(receive->_overlap._size._width*receive->_overlap._size._height);
			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " will receive overlap from: " << neighborsToUpdate[i] << " with size: " << receive->_data.size() << " and zone: " << receive->_overlap );
			MPI_Irecv(&receive->_data[0], receive->_data.size(), MPI_INTEGER, neighborsToUpdate[i], eRasterData, MPI_COMM_WORLD, &receive->_request);
			_receiveRequests.push_back(receive);
		}
		log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " raster: " << d << " received");
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << "/" << sectionIndex << " receiveOverlapData ended");
}

void SpacePartition::receiveMaxOverlapData()
{
	std::stringstream logName;
	logName << "MPI_raster_world_" << _id;
	log_DEBUG(logName.str(), getWallTime() << " step: "  << _world->getCurrentStep() << " receiveMaxOverlapData");
	// for each raster, we receive data from all the active neighbors
	for(size_t d=0; d<_world->getNumberOfRasters(); d++)
	{
		if(!_world->rasterExists(d) || !_world->isRasterDynamic(d))
		{
			continue;
		}
		log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " receiving max raster: " << d);
		for(size_t i=0; i<_neighbors.size(); i++)			
		{
			MpiOverlap* receive = new MpiOverlap;
			receive->_rasterName = _world->getRasterName(d);
			receive->_overlap = getExternalOverlap(_neighbors[i]);
			receive->_data.resize(receive->_overlap._size._width*receive->_overlap._size._height);

			log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " will receive max overlap to: " << _neighbors[i] << " with size: " << receive->_data.size() << " and zone: " << receive->_overlap << " from " << _neighbors[i]);
			MPI_Irecv(&receive->_data[0], receive->_data.size(), MPI_INTEGER, _neighbors[i], eRasterMaxData, MPI_COMM_WORLD, &receive->_request);
			_receiveRequests.push_back(receive);
		}
		log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep()  << " raster: " << d << " max data received");
	}
	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " receiveMaxOverlapData ended");
}

void SpacePartition::clearRequests( bool updateMaxValues )
{	
	std::stringstream logName;
	logName << "MPI_raster_world_" << _id;
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
				for(size_t i=0; i<receive->_data.size(); i++)
				{
					Point2D<int> index(overlapZone._origin._x+i%overlapZone._size._width, overlapZone._origin._y+i/overlapZone._size._width);
					if(updateMaxValues)
					{
						log_EDEBUG(logName.str(), "\t" << getWallTime() << " step: " << _world->getCurrentStep() << " receive index: " << index << " max value: " << receive->_data.at(i));
						_world->getDynamicRaster(receive->_rasterName).setMaxValue(index, receive->_data.at(i));
					}
					else
					{
						log_EDEBUG(logName.str(), "\t" << getWallTime() << " step: " << _world->getCurrentStep() << " receive index: " << index << " current value: " << receive->_data.at(i));
						_world->getDynamicRaster(receive->_rasterName).setValue(index, receive->_data.at(i));
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

int SpacePartition::getIdFromPosition( const Point2D<int> & position )
{
	Point2D<int> nodePosition(position._x/_ownedArea._size._width, position._y/_ownedArea._size._height);
	return nodePosition._y*sqrt(_numTasks)+nodePosition._x;
}

Point2D<int> SpacePartition::getPositionFromId( const int & id ) const
{
	int worldsPerRow = sqrt(_numTasks);
	Point2D<int> worldPos(id%worldsPerRow, id/worldsPerRow);
	return worldPos;
}

int SpacePartition::getNeighborIndex( const int & id )
{
	for(size_t i=0; i<_neighbors.size(); i++)
	{
		if(_neighbors[i] == id)
		{
			return i;
		}
	}
	std::stringstream oss;
	oss << "SpacePartition::getNeighborIndex- " << _id << " looking for neighbor with id: " << id << " not found";
	throw Exception(oss.str());
}

void SpacePartition::executeAgents()
{
	for(int sectionIndex=0; sectionIndex<4; sectionIndex++)
	{
		// section index doesn't matter if is the entire overlap
		// TODO refactor? we are sending 4 times all the info
		sendOverlapZones(sectionIndex, false);
		receiveOverlapData(sectionIndex, false);
	}
	
	std::stringstream logName;
	logName << "simulation_" << getId();

	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " has executed update overlap");
	_executedAgentsHash.clear();

	std::stringstream logNameMpi;
	logNameMpi << "simulation_" << _id;

	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " executing sections");
	for(int sectionIndex=0; sectionIndex<4; sectionIndex++)
	{
		stepSection(sectionIndex);
		log_DEBUG(logNameMpi.str(), getWallTime() << " executing step: " << _world->getCurrentStep() << " and section: " << sectionIndex << " has been executed");
		receiveAgents(sectionIndex);
		log_DEBUG(logNameMpi.str(), getWallTime() << " executing step: " << _world->getCurrentStep() << " and section: " << sectionIndex << " has received agents");

		sendGhostAgents(sectionIndex);
		log_DEBUG(logNameMpi.str(), getWallTime() << " executing step: " << _world->getCurrentStep() << " and section: " << sectionIndex << " sent ghosts");
		receiveGhostAgents(sectionIndex);
		log_DEBUG(logNameMpi.str(), getWallTime() << " executing step: " << _world->getCurrentStep() << " and section: " << sectionIndex << " received ghosts");

		sendOverlapZones(sectionIndex);
		log_DEBUG(logNameMpi.str(), getWallTime() << " executing step: " << _world->getCurrentStep() << " and section: " << sectionIndex << " sent overlap");
		receiveOverlapData(sectionIndex);
		log_DEBUG(logNameMpi.str(), getWallTime() << " executing step: " << _world->getCurrentStep() << " and section: " << sectionIndex << " received overlap" );
		MPI_Barrier(MPI_COMM_WORLD);

		clearRequests(false);		
	}
}

void SpacePartition::initOverlappingData()
{
	// we need to send the agents and data in overlap zone to adjacent computer nodes	
	sendMaxOverlapZones();
	receiveMaxOverlapData();
	// all nodes must finish receiving max values before receiving current values
	MPI_Barrier(MPI_COMM_WORLD);
	clearRequests(true);

	for(int sectionIndex=0; sectionIndex<4; sectionIndex++)
	{

		// section index doesn't matter if is the entire overlap
		// TODO refactor? we are sending 4 times all the info
		sendOverlapZones(sectionIndex, false);
		receiveOverlapData(sectionIndex, false);
		// all nodes must finish receiving max values before receiving current values
		MPI_Barrier(MPI_COMM_WORLD);
		clearRequests(false);
		

		sendGhostAgents(sectionIndex);
		receiveGhostAgents(sectionIndex);
	}

}

void SpacePartition::finish()
{	
	std::stringstream logName;
	logName << "simulation_" << _id;

	_serializer.finish();

	log_INFO(logName.str(), getWallTime() << " simulation finished");
	if(_finalize)
	{
		MpiFactory::instance()->cleanTypes();
		MPI_Finalize();
	}
}

const Rectangle<int> & SpacePartition::getBoundaries() const
{
	return _boundaries;
}

bool SpacePartition::isCorner( const int & neighbor ) const
{
	Point2D<int> worldPos = getPositionFromId(neighbor);
	Point2D<int> diff = worldPos - _worldPos;
	if(std::abs(diff._x)==1 && std::abs(diff._y)==1)
	{
		return true;
	}
	return false;
}

Rectangle<int> SpacePartition::getInternalOverlap( const int & id ) const
{
	Point2D<int> diff = getPositionFromId(id)-_worldPos;
	// left
	
	Rectangle<int> result;

	// origin
	if(diff._x==-1)
	{
		if(_ownedArea._origin._x==0)
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
		if(_ownedArea._origin._x==0)
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
		if(_ownedArea._origin._x==0)
		{
			result._origin._x = _ownedArea._size._width - _overlap;
		}
		// else sum and remove an overlap
		else
		{
			result._origin._x = _ownedArea._size._width;
		}
	}
	
	if(diff._y==-1)
	{
		if(_ownedArea._origin._y==0)
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
		if( _ownedArea._origin._y==0)
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
		if(_ownedArea._origin._y==0)
		{
			result._origin._y = _ownedArea._size._height - _overlap;
		}
		// else sum and remove an overlap
		else
		{
			result._origin._y = _ownedArea._size._height;
		}
	}

	// size
	if(diff._x!=0)
	{
		result._size._width = _overlap;
	}
	else
	{
		result._size._width = _ownedArea._size._width;
	}
	
	if(diff._y!=0)
	{
		result._size._height = _overlap;
	}
	else
	{
		result._size._height = _ownedArea._size._height;
	}
	std::stringstream logName;
	logName << "MPI_raster_world_" << _id;
	log_DEBUG(logName.str(), getWallTime() << " internal overlap with: " << id << " and diff: " << diff << " - " << result);
	return result;
}

Rectangle<int> SpacePartition::getExternalOverlap( const int & id) const
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
		if(_ownedArea._origin._x == 0)
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
		if(_ownedArea._origin._x==0)
		{
			result._origin._x = _ownedArea._size._width;
		}
		// else sum an overlap
		else
		{
			result._origin._x = _ownedArea._size._width + _overlap;
		}
	}
	
	if(diff._y==-1)
	{	
		result._origin._y = 0;
	}
	else if(diff._y==0)
	{
		if(_ownedArea._origin._y == 0)
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
		// if lower border it doesn't have a lower overlap		
		if(_ownedArea._origin._y==0)
		{
			result._origin._y = _ownedArea._size._height;
		}
		// else sum an overlap
		else
		{
			result._origin._y = _ownedArea._size._height + _overlap;
		}
	}

	// size
	if(diff._x!=0)
	{
		result._size._width = _overlap;
	}
	else
	{
		result._size._width = _ownedArea._size._width;
	}
	
	if(diff._y!=0)
	{
		result._size._height = _overlap;
	}
	else
	{	
		result._size._height = _ownedArea._size._height;
	}
	
	std::stringstream logName;
	logName << "MPI_raster_world_" << _id;
	log_DEBUG(logName.str(), getWallTime() << " external overlap with: " << id << " and diff: " << diff << " - " << result);
	return result;
}

Rectangle<int> SpacePartition::getOverlap( const int & id, const int & sectionIndex) const	
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
			result._origin._x = _ownedArea._size._width/2;	
			if(_ownedArea._origin._x+_ownedArea._size._width!=_world->getConfig().getSize()._width)
			{
				result._origin._x -= _overlap;
			}
		}
	}
	else
	{
		// if left border just remove an overlap
		if(_ownedArea._origin._x==0)
		{
			result._origin._x = _ownedArea._size._width - _overlap;
		}
		// else sum and remove an overlap
		else
		{
			result._origin._x = _ownedArea._size._width;
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
			result._origin._y = _ownedArea._size._height/2;
			if(_ownedArea._origin._y+_ownedArea._size._height!=_world->getConfig().getSize()._height)
			{
				result._origin._y -= _overlap;
			}
		}
	}
	else
	{
		// if top border just remove an overlap
		if(_ownedArea._origin._y==0)
		{
			result._origin._y = _ownedArea._size._height - _overlap;
		}
		// else sum and remove an overlap
		else
		{
			result._origin._y = _ownedArea._size._height;
		}
	}

	// size
	if(diff._x!=0)
	{
		result._size._width = _overlap*2;
	}
	else
	{
		// this result is = to boundaries + 1 overlap
		result._size._width = _ownedArea._size._width/2;
		// border
		if(_ownedArea._origin._x!=0)
		{
			result._size._width += _overlap;
		}
		else
		{
			if(sectionIndex!=0 && sectionIndex!=2)
			{
				result._size._width += _overlap;
			}
		}

		if(_ownedArea._origin._x+_ownedArea._size._width!=_world->getConfig().getSize()._width)
		{
			result._size._width += _overlap;
		}
		else
		{
			if(sectionIndex!=1 && sectionIndex!=3)
			{
				result._size._width += _overlap;
			}
		}
	}
	
	if(diff._y!=0)
	{
		result._size._height = _overlap*2;
	}
	else
	{
		result._size._height = _ownedArea._size._height/2;
		if(_ownedArea._origin._y!=0)
		{
			result._size._height += _overlap;
		}
		else
		{
			if(sectionIndex!=0 && sectionIndex!=1)
			{
				result._size._height += _overlap;
			}
		}

		if(_ownedArea._origin._y+_ownedArea._size._height!=_world->getConfig().getSize()._height)
		{
			result._size._height += _overlap;
		}
		else
		{
			if(sectionIndex!=2 && sectionIndex!=3)
			{
				result._size._height += _overlap;
			}
		}
	}
	std::stringstream logName;
	logName << "MPI_raster_world_" << _id;
	log_DEBUG(logName.str(), getWallTime() << " overlap with: " << id << " and diff: " << diff << " in section Index: " << sectionIndex << " - " << result);
	return result;
}

bool SpacePartition::needsToBeUpdated( const int & id, const int & sectionIndex )
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
			oss << "SpacePartition::needsToBeUpdated - wrong section index: " << sectionIndex;
			throw Engine::Exception(oss.str());
	}
	return false;
}

bool SpacePartition::needsToReceiveData( const int & id, const int & sectionIndex )
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
			oss << "SpacePartition::needsToReceiveData - wrong section index: " << sectionIndex;
			throw Engine::Exception(oss.str());
	}
	return false;
}

const int & SpacePartition::getOverlap() const
{
	return _overlap;
}

bool SpacePartition::hasBeenExecuted( const std::string & id ) const
{
	if(_executedAgentsHash.find(id)==_executedAgentsHash.end())
	{
		return false;
	}
	return true;
}

void SpacePartition::agentAdded( AgentPtr agent, bool executedAgent )
{
	_executedAgentsHash.insert(make_pair(agent->getId(), agent));
	AgentsList::iterator it = getGhostAgent(agent->getId());
	if(it!=_overlapAgents.end())
	{
		_overlapAgents.erase(it);
	}

	if(!executedAgent)
	{
		return;
	}
}

AgentsList::iterator SpacePartition::getGhostAgent( const std::string & id )
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

Agent * SpacePartition::getAgent( const std::string & id )
{
	AgentsList::iterator it = getOwnedAgent(id);
	if(it!=_world->endAgents())
	{
		Agent * agent = it->get();
		if(agent->exists())
		{
			return agent;
		}
	}
	it = getGhostAgent(id);
	if(it!=_overlapAgents.end())
	{
		Agent * agent = it->get();
		if(agent->exists())
		{
			return agent;
		}
	}
	return 0;
}

void SpacePartition::removeAgent( Agent * agent )
{
	AgentsList::iterator it = getOwnedAgent(agent->getId());
	if(it==_world->endAgents())
	{
		std::stringstream oss;
		oss << "SpacePartition::removeAgent - agent: " << agent << " not found";
		throw Exception(oss.str());
	}
	// TODO it is not needed if it has modified position, as it is already done after the executed of a given agent step
	//sendDeleteOverlapAgent(it, agent->getPosition());
	_removedAgents.push_back(*it);
}

void SpacePartition::removeAgents()
{
	AgentsList::iterator it=_removedAgents.begin();
	while(it!=_removedAgents.end())
	{
		Agent * agent = it->get();
		AgentsList::iterator itAg = getOwnedAgent(agent->getId());
		if(itAg==_world->endAgents())
		{
			std::stringstream oss;
			oss << "SpacePartition::removeAgents - agent: " << agent << " not found";
			throw Exception(oss.str());
			return;
		}
		_world->eraseAgent(itAg);
		it = _removedAgents.erase(it);
	}
	_removedAgents.clear();
}

AgentsVector SpacePartition::getAgent( const Point2D<int> & position, const std::string & type )
{
	AgentsVector result;
	for(AgentsList::iterator it=_world->beginAgents(); it!=_world->endAgents(); it++)
	{
        AgentPtr agent = *it;
		if(agent->getPosition().isEqual(position))
		{
			if(type.compare("all")==0 || agent->isType(type))
			{
				result.push_back(agent);
			}
		}
	}
	for(AgentsList::iterator it=_overlapAgents.begin(); it!=_overlapAgents.end(); it++)		
	{
        AgentPtr agent = *it;
		if(agent->getPosition().isEqual(position))
		{	
			if(type.compare("all")==0 || agent->isType(type))
			{
				result.push_back(agent);
			}
		}
	}
	return result;
}

AgentsList::iterator SpacePartition::getOwnedAgent( const std::string & id )
{
	for(AgentsList::iterator it=_world->beginAgents(); it!=_world->endAgents(); it++)
	{
		if((*it)->getId().compare(id)==0)
		{
			return it;
		}
	}
	return _world->endAgents();
}

bool SpacePartition::willBeRemoved( const std::string & id )
{
	for(AgentsList::iterator it=_removedAgents.begin(); it!=_removedAgents.end(); it++)
	{	
		if((*it)->getId().compare(id)==0)
		{
			return true;
		}
	}
	return false;
}

const Rectangle<int> & SpacePartition::getOwnedArea() const
{
	return _ownedArea;
}
	
Point2D<int> SpacePartition::getRealPosition( const Point2D<int> & globalPosition ) const
{
	return globalPosition-_boundaries._origin;
}

Point2D<int> SpacePartition::getRandomPosition() const
{
	Engine::Point2D<int> pos(GeneralState::statistics().getUniformDistValue(0,_ownedArea._size._width-1), GeneralState::statistics().getUniformDistValue(0,_ownedArea._size._height-1));
	pos += _ownedArea._origin;
	return pos;
}

double SpacePartition::getWallTime() const
{
	return MPI_Wtime() - _initialTime;
}

void SpacePartition::addStringAttribute( const std::string & type, const std::string & key, const std::string & value )
{
	_serializer.addStringAttribute(type, key, value);
}

void SpacePartition::addIntAttribute( const std::string & type, const std::string & key, int value )
{
	_serializer.addIntAttribute(type, key, value);
}

void SpacePartition::addFloatAttribute( const std::string & type, const std::string & key, float value )
{
	_serializer.addFloatAttribute(type, key, value);
}

void SpacePartition::serializeAgents( const int & step )
{
	_serializer.serializeAgents(step, _world->beginAgents(), _world->endAgents());
}

void SpacePartition::serializeRasters( const int & step )
{
	_serializer.serializeRasters(step);
}

int SpacePartition::countNeighbours( Agent * target, const double & radius, const std::string & type )
{
	int numAgents = for_each(_world->beginAgents(), _world->endAgents(), aggregatorCount<std::shared_ptr<Agent> >(radius,*target, type))._count;
	int numOverlapAgents = for_each(_overlapAgents.begin(), _overlapAgents.end(), aggregatorCount<std::shared_ptr<Agent> >(radius,*target, type))._count;
	return numAgents+numOverlapAgents;
}

AgentsVector SpacePartition::getNeighbours( Agent * target, const double & radius, const std::string & type )
{
	AgentsVector agentsVector = for_each(_world->beginAgents(), _world->endAgents(), aggregatorGet<std::shared_ptr<Agent> >(radius,*target, type))._neighbors;
	AgentsVector overlapAgentsVector =  for_each(_overlapAgents.begin(), _overlapAgents.end(), aggregatorGet<std::shared_ptr<Agent> >(radius,*target, type))._neighbors;
	std::copy(overlapAgentsVector.begin(), overlapAgentsVector.end(), std::back_inserter(agentsVector));
	std::random_shuffle(agentsVector.begin(), agentsVector.end());
	return agentsVector;
}
	
void SpacePartition::setValue( DynamicRaster & raster, const Point2D<int> & position, int value )
{
	raster.setValue(getRealPosition(position), value);
}

int SpacePartition::getValue( const DynamicRaster & raster, const Point2D<int> & position ) const
{
	return raster.getValue(getRealPosition(position));
}

void SpacePartition::setMaxValue( DynamicRaster & raster, const Point2D<int> & position, int value )
{
	raster.setMaxValue(getRealPosition(position), value);
}

int SpacePartition::getMaxValue( const DynamicRaster & raster, const Point2D<int> & position ) const
{
	return raster.getMaxValue(getRealPosition(position));
}

} // namespace Engine

