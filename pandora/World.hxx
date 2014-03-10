
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


#include <map>
#include <vector>
#include <list>
#include <Raster.hxx>
#include <StaticRaster.hxx>
#include <Rectangle.hxx>
#include <Point2D.hxx>
#include <Simulation.hxx>
#include <SpacePartition.hxx>

#include <algorithm>

namespace Engine
{
class SpacePartition;
class Agent;

class World
{
	SpacePartition * _scheduler;
public:
	typedef std::map< std::string, int> RasterNameMap;
	typedef std::list< Agent* > AgentsList;
	typedef std::vector< Agent* > AgentsVector;
protected:		
	Simulation _simulation;

	Rectangle<int> _globalBoundaries;

	//! global list of agents
	AgentsList _agents;

	
	//! false if each cell can have just one agent
	bool _allowMultipleAgentsPerCell;



	//! current simulation step
	int _step;

public: // TODO for refactoring of Scheduler
	//! true if the agent is in the list of agents to remove
	bool willBeRemoved( Agent * agent );

private:
	//! if this variable is set to true, getNeighbours will look through the list of agents instead of searching by position. It is false by default
	bool _searchAgents;
	
protected:
	double _initialTime;
	// rasters that won't change values during the simulation
	std::map<std::string, int> _rasterNames;
	std::vector<StaticRaster * > _rasters;	
	// true if the raster is dynamic
	std::vector<bool> _dynamicRasters;
	std::vector<bool> _serializeRasters;

	//! stub method for grow resource to max of initialrasters, used by children of world at init time
	void updateRasterToMaxValues( const std::string & key );
	void updateRasterToMaxValues( const int & index );
	

	//! dumps current state of the simulation. Then applies next simulation step.
	void step();

		
	//! method interface for attribute _searchAgents. 
	void setSearchAgents( const bool & searchAgents );
	//! method interface for attribute _searchAgents. 
	bool getSearchAgents();

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

	//! calls init without MPI initialization (used in pyPandora)
	void initialize();
	//! Runs the simulation. Performs each step and stores the states. Requires calling 'init' method a-priori.
	void run();
	
	//! add an agent to the world, and remove it from overlap agents if exist
	void addAgent( Agent * agent, bool executedAgent = true );
	




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
	virtual void stepRaster( const int & index);
	//! the override of this method allows to modify agents between step executions
	virtual void stepAgents();

	//! returns raster identified by parameter 'key'.
	Raster & getDynamicRaster( const size_t & index );
	Raster & getDynamicRaster( const std::string & key );
	const Raster & getDynamicRaster( const size_t & index ) const;

	//! returns static raster identified by parameter 'key'.
	StaticRaster & getStaticRaster( const size_t & index );
	StaticRaster & getStaticRaster( const std::string & key );

	//! create a new static raster map with the stablished size and given key
	void registerStaticRaster( const std::string & key, const bool & serialize, int index = -1);
	//! create a new raster map with the stablished size and given key
	void registerDynamicRaster( const std::string & key, const bool & serialize, int index = -1);
	//! checks if position parameter 'newPosition' is free to occupy by an agent, 'newPosition' is inside of the world and the maximum of agent cell-occupancy is not exceeded.
	bool checkPosition( const Point2D<int> & newPosition );

	//! returns the simulation characterization of this world
	Simulation & getSimulation();

	//! sets the value of raster "key" to value "value" in global position "position"
	void setValue( const std::string & key, const Point2D<int> & position, int value );
	//! sets the value of raster "index" to value "value" in global position "position"
	void setValue( const int & index, const Point2D<int> & position, int value );
	//! returns the value of raster "key" in global position "position"
	int getValue( const std::string & key, const Point2D<int> & position ) const;
	//! returns the value of raster "index" in global position "position"
	int getValue( const int & index, const Point2D<int> & position ) const;

	//! sets the maximum allowed value of raster "key" to value "value" in global position "position"
	void setMaxValue( const std::string & key, const Point2D<int> & position, int value );
	//! sets the maximum allowed value of raster "index" to value "value" in global position "position"
	void setMaxValue( const int & index, const Point2D<int> & position, int value );

	//! gets the maximum allowed value of raster "key" in global position "position"
	int getMaxValueAt( const std::string & key, const Point2D<int> & position );
	//! gets the maximum allowed value of raster "index" in global position "position"
	int getMaxValueAt( const int & index, const Point2D<int> & position );

	//! returns a Rectangle<int> expressing the boundaries of the world
	const Rectangle<int> & getBoundaries() const;
	const Rectangle<int>& getGlobalBoundaries() const { return _globalBoundaries; }

	
	// get a raster name from its index
	const std::string & getRasterName( const int & index ) const;
public:
	//! Factory method design pattern for creating concrete agents and rasters. It is delegated to concrete Worlds. This method must be defined by children, it is the method where agents are created and addAgents must be called
	virtual void createAgents() = 0;
	//! to be redefined for subclasses
	virtual void createRasters() = 0;

	int	getCurrentTimeStep() const { return _step; }
	double getWallTime() const;
	//! provides a random valid position inside boundaries
	Point2D<int> getRandomPosition();


// stub methods to scheduler
public:
	int getId() const;
	const Rectangle<int> & getOverlapBoundaries() const;
	const int & getOverlap();
	const int & getNumTasks() const;
	const Point2D<int> & getLocalRasterSize() const;
// methods that need to be defined for current state of the code
	const Point2D<int> & getSize() const;
	AgentsList::iterator beginAgents() { return _agents.begin(); }
	AgentsList::iterator endAgents() { return _agents.end(); }
	size_t getNumberOfRasters() const { return _rasters.size(); }
	StaticRaster * getStaticRasterIndex( size_t index ) { return _rasters.at(index); }
	bool getDynamicRasterIndex( size_t index ) { return _dynamicRasters.at(index); }
	void eraseAgent( AgentsList::iterator & it ) { _agents.erase(it); }
	void removeAgent( Agent * agent );

};

} // namespace Engine

#endif //__World_hxx__

