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

#ifndef __SpacePartition_hxx__
#define __SpacePartition_hxx__

#include <mpi.h>
#include <World.hxx>
#include <typedefs.hxx>
#include <Serializer.hxx>
#include <list>
#include <vector>
#include <Scheduler.hxx>

namespace Engine
{
class Agent;

//! struct containing raster information sent between nodes
struct MpiOverlap
{
	// TODO move from string to index
	std::string _rasterName;
	std::vector<int> _data;
	Rectangle<int> _overlap;
	MPI_Request _request;
};

/** SpacePartition is a execution scheduler
  * It distributes a Pandora execution in different nodes using spatial partition
  * Each node contains the same amount of space and the agents inside
  * It is efficient for models where a homogeneous density of agents is expected around the whole world
  */
class SpacePartition : public Scheduler
{
	Serializer _serializer;	
	
	Point2D<int> _localRasterSize;

	//! map of already executed agents
	std::map<std::string, std::weak_ptr<Agent> > _executedAgentsHash;	

	//! position of World inside global limits 
	Point2D<int> _worldPos;

	std::list<MpiOverlap*> _sendRequests;
	std::list<MpiOverlap*> _receiveRequests;
	// this method checks whether all the requests in the pool created by MPI_Isend and MPI_Irecv are finished before continuing
	void clearRequests( bool updateMaxValues );
	
	// method to send a list of agents to their respective future world
	void sendAgents( AgentsList & agentsToSend );
	// Method to send overlap zones in the section we have executed 
	// if entire overlap is true, the node will send its owned zone in overlap as well as adjacents overlapped zones
	void sendOverlapZones( const int & sectionIndex, const bool & entireOverlap = true );
	void sendMaxOverlapZones();
	// method to copy of agents to neighbours
	void sendGhostAgents( const int & sectionIndex );

	// add the agent to overlap agents list, and remove previous instances if they exist
	//void updateOverlapAgent( Agent * agent );
	void receiveGhostAgents( const int & sectionIndex );
	// method to receive agents
	void receiveAgents( const int & sectionIndex );
	// method to receive overlap zones from neighbors that have executed adjacent sections 
	// if entire overlap is true, the node will send its owned zone in overlap as well as adjacents overlapped zones
	void receiveOverlapData( const int & sectionIndex, const bool & entireOverlap = true );
	void receiveMaxOverlapData();
	
	//! id's of neighboring computer nodes
	std::vector<int> _neighbors;
	//! area inside boundaries owned by the computer node without overlap
	Rectangle<int> _ownedArea;
	//! the four sections into a world is divided
	std::vector<Rectangle<int> > _sections;
	
	//! list of agents owned by other nodes in overlapping positions
	AgentsList _overlapAgents;
	
	//! this method returns true if neighbor is corner of _id
	bool isCorner(const int & neighbor) const;
	
	//! this method returns the general overlap zone between both worlds
	Rectangle<int> getOverlap(const int & id, const int & sectionIndex ) const;
	//! this method returns the external part of the strict overlap between World and id, 
	Rectangle<int> getExternalOverlap(const int & id) const;
	//! this method returns the internal part of the strict overlap between World and id, 
	Rectangle<int> getInternalOverlap(const int & id) const;
	//! returns true if neighbor id must be updated this section index execution
	bool needsToBeUpdated( const int & id, const int & sectionIndex );
	//! returns true if neighbor id will send data to _id, according to index execution
	bool needsToReceiveData( const int & id, const int & sectionIndex );
	
	//! amount of width around one boundary considering the side of the World object that owns _overlap
	int _overlap;
	
	//! check correct overlap/size relation
	void checkOverlapSize();
	//! compute _boundaries based on Size, number of nodes and _overlap
	void stablishBoundaries();
	//! define original position of world, given overlap, size and id.
	void stablishWorldPosition();
	//! applies next simulation step on the Section of the space identified by parameter 'sectionIndex'.
	void stepSection( const int & sectionIndex );

	//! returns the id of the section that contains the point 'position' 
	int getIdFromPosition( const Point2D<int> & position );
	//! given the id of a section returns that section position 
	Point2D<int> getPositionFromId( const int & id ) const;
	//! given the id of a neighbour world section, returns its index, the position in the vector _neighbors
	int getNeighborIndex( const int & id );

	//! if true will call MPI_Finalize at the end of run (default behavior)
	bool _finalize;

	//! this method returns true if the agent is already in executedAgents list
	bool hasBeenExecuted( const std::string & id ) const;
	//! return an agent, if it is in the list of ghosts 
	AgentsList::iterator getGhostAgent( const std::string & id );
	//! this list has the agents that need to be removed at the end of step.
	AgentsList _removedAgents;
	//! return an agent, if it is in the list of owned
	AgentsList::iterator getOwnedAgent( const std::string & id );
	
	//! true if the agent is in the list of agents to remove
	bool willBeRemoved( const std::string & id );
	double _initialTime;
	//! send overlapping data to neighbours before run
	void initOverlappingData();

	const Rectangle<int> & getOwnedArea() const;
	//! returns the attribute _overlap
	const int & getOverlap() const;
	//! transform from global coordinates to real coordinates (in terms of world position)
	Point2D<int> getRealPosition( const Point2D<int> & globalPosition ) const;

public:
	SpacePartition(const int & overlap, bool finalize );
	virtual ~SpacePartition();

	void finish();

	const Rectangle<int> & getBoundaries() const;
	//! initialization of the object World for the simulation. Required to be called before calling run.

	//! initializes everything needed before creation of agents and rasters (i.e. sizes)
	void init( int argc, char *argv[] );
	// initialize data processes after creation of agents and rasters
	void initData();
	//! responsible for executing the agents and update world 
	void executeAgents();

	void agentAdded( AgentPtr agent, bool executedAgent );
	void removeAgents();
	void removeAgent(Agent * agent);
	
	//! this method will return an agent, both looking at owned and ghost agents
	Agent * getAgent( const std::string & id );
	AgentsVector getAgent( const Point2D<int> & position, const std::string & type="all" );

	Point2D<int> getRandomPosition() const;

	//! MPI version of wall time
	double getWallTime() const;

	void addIntAttribute( const std::string & type, const std::string & key, int value );
	void addStringAttribute( const std::string & type, const std::string & key, const std::string & value );
	void addFloatAttribute( const std::string & type, const std::string & key, float value );
	void serializeAgents( const int & step);
	void serializeRasters( const int & step);
	int countNeighbours( Agent * target, const double & radius, const std::string & type);
	AgentsVector getNeighbours( Agent * target, const double & radius, const std::string & type);
	
	void setValue( DynamicRaster & raster, const Point2D<int> & position, int value );
	int getValue( const DynamicRaster & raster, const Point2D<int> & position ) const;
	void setMaxValue( DynamicRaster & raster, const Point2D<int> & position, int value );
	int getMaxValue( const DynamicRaster & raster, const Point2D<int> & position ) const;

	friend class Serializer;

    size_t getNumberOfOverlapAgents() const{return _overlapAgents.size(); }
    AgentsList::const_iterator beginOverlapAgents() const{ return _overlapAgents.begin(); }
    AgentsList::const_iterator endOverlapAgents() const{ return _overlapAgents.end(); }
};

} // namespace Engine

#endif // __SpacePartition_hxx__

