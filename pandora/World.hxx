
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

#ifndef __World_hxx__
#define __World_hxx__

enum MpiMessageType
{
	eNumAgents = 1,
	eAgent = 2,
	eNumGhostAgents = 3,
	eGhostAgent = 4,
	eRasterData = 5,
	eRasterMaxData = 6,
	eVectorAttribute = 7, 	
	eSizeVector = 8,
	eNumModifiedAgents = 9,
	eModifiedAgent = 10
};

#include <map>
#include <vector>
#include <list>
#include <Raster.hxx>
#include <StaticRaster.hxx>
#include <Rectangle.hxx>
#include <Point2D.hxx>
#include <Simulation.hxx>
#include <mpi.h>

#include <algorithm>

namespace Engine
{
class Agent;

struct MpiOverlap
{
	std::string _rasterName;
	std::vector<int> _data;
	Rectangle<int> _overlap;
	MPI_Request _request;
};

class World
{
public:
	typedef std::map< std::string, StaticRaster > StaticRastersMap;
	typedef std::map< std::string, Raster > RastersMap;
	typedef std::list< Agent* > AgentsList;
	typedef std::vector< Agent* > AgentsVector;
protected:		
	Simulation _simulation;

	int _localRasterSize;

	Point2D<int> _worldPos;
	Rectangle<int> _boundaries;
	Rectangle<int> _globalBoundaries;
	//! boundaries of world, with overlaps added
	Rectangle<int> _overlapBoundaries;
	//! the four sections into a world is divided
	std::vector<Rectangle<int> > _sections;
	std::vector<int> _neighbors;

	//! list of already executed agents
	//AgentsList _executedAgents;
	std::map<std::string, Agent *> _executedAgentsHash;	
	//! global list of agents
	AgentsList _agents;
	AgentsList _overlapAgents;
	//! this list has the agents that need to be removed at the end of step.
	AgentsList _removedAgents;

	//! this method returns true if the agent is already in executedAgents list
	bool hasBeenExecuted( Agent * agent );
	//! true if the agent is in the list of agents to remove
	bool willBeRemoved( Agent * agent );

	//! false if each cell can have just one agent
	bool _allowMultipleAgentsPerCell;

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


	//! current simulation step
	int _step;


	//! provides a random valid position inside boundaries
	Point2D<int> getRandomPosition();

	std::list<MpiOverlap*> _sendRequests;
	std::list<MpiOverlap*> _receiveRequests;
	// this method checks whether all the requests in the pool created by MPI_Isend and MPI_Irecv are finished before continuing
	void clearRequests( bool updateMaxValues );
private:
	//! PENDENT amount of width around one boundary considering the side of the World object that owns _overlap
	int _overlap;
	
	// if this variable is set to true, getNeighbours will look through the list of agents instead of searching by position. It is false by default
	bool _searchAgents;
	
	//std::vector<Environment * > _environments;

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
protected:
	double _initialTime;
	//! check correct overlap/size relation
	void checkOverlapSize();
	//! stablish position of world inside the space of the simulation
	void stablishPosition();

	// rasters that won't change values during the simulation
	StaticRastersMap _staticRasters;
	//! map of all the dynamic rasters used by the simulation
	RastersMap _dynamicRasters;
	std::list<std::string> _rastersToSerialize;

	//! stub method for grow resource to max of initialrasters, used by children of world at init time
	void updateRasterToMaxValues( const std::string & key );
	
	//! define original position of world, given overlap, size and id.
	void stablishWorldPosition();
	//! dumps current state of the simulation. Then applies next simulation step.
	void step();
	//! applies next simulation step on the Section of the space identified by parameter 'sectionIndex'.
	void stepSection( const int & sectionIndex );
	//! returns static raster identified by parameter 'key'.
	StaticRaster & getStaticRaster( const std::string & key );
	
	//! method interface for attribute _searchAgents. 
	void setSearchAgents( const bool & searchAgents );
	//! method interface for attribute _searchAgents. 
	bool getSearchAgents();

	//! return a raster from the entire set (dynamic and static)
	StaticRaster & getRasterTmp( const std::string & key );
	const StaticRaster & getRasterTmp( const std::string & key ) const;

public:
	//! constructor.
	/*!
	The World object is bounded to a Simulation configuration through the parameter 'simulation'.
	
	The parameter 'overlap' defines the width of overlapping are between neighbour Sections (world divisions).
	
	The parameter 'allowMultipleAgentsPerCell' defines if more than one agent can occupy a cell of the World.
	
	The parameter 'fileName' references the file where the simulation will be dumped to.
	*/
	World( const Simulation & simulation, const int & overlap, const bool & allowMultipleAgentsPerCell, const std::string & fileName );
	
	virtual ~World();
	//! initialization of the object World for the simulation. Required to be called before calling run.
	/*!
	The MPI is prepared. The simulation state is initialized, the rasters created and the
	World filled with agents.
	*/ 
	void init( int argc, char *argv[] );
	//! calls init without MPI initialization (used in pyPandora)
	void initialize();
	//! Runs the simulation. Performs each step and stores the states. Requires calling 'init' method a-priori.
	void run();
	
	//! add an agent to the world, and remove it from overlap agents if exist
	void addAgent( Agent * agent );
	
	void removeAgents();
	void removeAgent(Agent * agent);

	//! this method will return an agent, both looking at owned and ghost agents
	Agent * getAgent( const std::string & id );
	AgentsVector getAgent( const Point2D<int> & position, const std::string & type="all" );

	//! return an agent, if it is in the list of owned
	AgentsList::iterator getOwnedAgent( const std::string & id );
	//! return an agent, if it is in the list of ghosts 
	AgentsList::iterator getGhostAgent( const std::string & id );

	// this method returns a list with the list of agents in manhattan distance radius of position. if include center is false, position is not checked
//	AgentsList getAgentsNear( const Point2D<int> & position, const int & radius, const bool & includeCenter );

	template<class T> struct aggregator : public std::unary_function<T,void>
	{
		aggregator(double radius, T &center, const std::string & type ) :  _radius(radius), _center(center), _type(type)
		{
			_particularType = _type.compare("all");
		}
		virtual ~aggregator(){}
		void operator()( T * neighbor )
		{
			if(neighbor==&_center || !neighbor->exists())
			{
				return;
			}
			if(_particularType && !neighbor->isType(_type))
			{
				return;
			}
			// if we use epsilon the evaluation will fail for equal double numbers
			//if(_center.getPosition().distance(neighbor->getPosition())-_radius<= std::numeric_limits<double>::epsilon())
			if(_center.getPosition().distance(neighbor->getPosition())-_radius<= 0.0001)
			{
					execute( *neighbor );
			}
		}
		virtual void execute( T & neighbor )=0;
		bool _particularType;
		double _radius;
		T & _center;
		std::string _type;
	};

	template<class T> struct aggregatorCount : public aggregator<T>
	{
		aggregatorCount( double radius, T & center, const std::string & type ) : aggregator<T>(radius,center,type), _count(0) {}
		void execute( T & neighbor )
		{
			_count++;
		}
		int _count;
	};
	template<class T> struct aggregatorGet : public aggregator<T>
	{
		aggregatorGet( double radius, T & center, const std::string & type ) : aggregator<T>(radius,center,type) {}
		void execute( T & neighbor )
		{
			_neighbors.push_back(&neighbor);
		}
		AgentsVector _neighbors;
	};

	//! returns the number of neighbours of agent 'target' within the radius 'radius' using Euclidean Distance.
	int countNeighbours( Agent * target, const double & radius, const std::string & type="all" );
	//! returns a list with the neighbours of agent 'target' within the radius 'radius' using Euclidean Distance.
	AgentsVector getNeighbours( Agent * target, const double & radius, const std::string & type="all" );
	//! returns an integer identifying the current step where the simulation is. The identifiers denote an order from older to newer steps.
	int getCurrentStep() const;
	//! this method can be redefined by the children in order to modify the execution of each step on a given resource field. Default is grow 1 until max
	virtual void stepEnvironment();
	//! this method is executed for each section during @stepSection. It is useful in the cases where a Raster is modified following data in adjacent cells.
	virtual void stepRastersSection(  const int & indexSection, const Rectangle<int> & section );
	//! dump the rasters through a serializer.
	void serializeRasters();
	//! dump the static rasters through a serializer.
	void serializeStaticRasters();
	//! dump the agents through a serializer.
	void serializeAgents();
	//! the override of this method allows to modify rasters between step executions
	virtual void stepRaster( const std::string & key);
	//! the override of this method allows to modify agents between step executions
	virtual void stepAgents();

	//! returns raster identified by parameter 'key'.
	Raster & getDynamicRaster( const std::string & key );
	const Raster& getConstDynamicRaster( const std::string& key ) const;

	//! create a new static raster map with the stablished size and given key
	void registerStaticRaster( const std::string & key, const bool & serialize );
	//! create a new raster map with the stablished size and given key
	void registerDynamicRaster( const std::string & key, const bool & serialize );
	//! checks if position parameter 'newPosition' is free to occupy by an agent, 'newPosition' is inside of the world and the maximum of agent cell-occupancy is not exceeded.
	bool checkPosition( const Point2D<int> & newPosition );

	//! returns the simulation characterization of this world
	Simulation & getSimulation();

	//! sets the value of raster "key" to value "value" in global position "position"
	void setValue( const std::string & key, const Point2D<int> & position, int value );
	//! returns the value of raster "key" in global position "position"
	int getValue( const std::string & key, const Point2D<int> & position ) const;
	//! sets the maximum allowed value of raster "key" to value "value" in global position "position"
	void setMaxValue( const std::string & key, const Point2D<int> & position, int value );
	//! gets the maximum allowed value of raster "key" in global position "position"
	int getMaxValueAt( const std::string & key, const Point2D<int> & position );

	//! returns a Rectangle<int> expressing the boundaries of the world
	const Rectangle<int> & getBoundaries() const;
	const Rectangle<int>& getGlobalBoundaries() const { return _globalBoundaries; }
	//! returns the Rectangle the contains the world section boundaries plus the overlap area around that boundaries
	const Rectangle<int> & getOverlapBoundaries() const;
	//! returns the id of the section that contains the point 'position' 
	int getIdFromPosition( const Point2D<int> & position );
	//! given the id of a section returns that section position 
	Point2D<int> getPositionFromId( const int & id ) const;
	//! given the id of a neighbour world section, returns its index, the position in the vector _neighbors
	int getNeighborIndex( const int & id );
	//! returns the simulation id where that World is in (??? TODO verifica això)
	int getId() const;

	bool rasterToSerialize( const std::string & key );
	//! returns the attribute _overlap
	const int & getOverlap();
public:
	//! Factory method design pattern for creating concrete agents and rasters. It is delegated to concrete Worlds. This method must be defined by children, it is the method where agents are created and addAgents must be called
	virtual void createAgents() = 0;
	//! to be redefined for subclasses
	virtual void createRasters() = 0;

	int	getCurrentTimeStep() const { return _step; }
	double getMpiTime() const;
};

} // namespace Engine

#endif //__World_hxx__

