
#ifndef __Scheduler_hxx__
#define __Scheduler_hxx__

#include <Agent.hxx>

namespace Engine
{

/** Scheduler is the base class to create simulation schedulers that control the flow of World and Agents execution
  * This is an implementation of the Bridge pattern, decoupling agent and position management from World class
  */
class Scheduler
{
protected:
	int _id;
	int _numTasks;
	Engine::Rectangle<int> _boundaries;
	World * _world;

	// this method returns a list with the list of agents in euclidean distance radius of position. if include center is false, position is not checked
	template<class T> struct aggregator : public std::unary_function<T,void>
	{
		aggregator(double radius, Agent &center, const std::string & type ) :  _radius(radius), _center(center), _type(type)
		{
			_particularType = _type.compare("all");
		}
		virtual ~aggregator(){}
		void operator()( T neighbor )
		{
			if(neighbor->getId()==_center.getId() || !neighbor->exists())
			{
				return;
			}
			if(_particularType && !neighbor->isType(_type))
			{
				return;
			}
			if(_center.getPosition().distance(neighbor->getPosition())-_radius<= 0.0001)
			{
					execute( neighbor );
			}
		}
		virtual void execute( T neighbor )=0;
		bool _particularType;
		double _radius;
		Agent & _center;
		std::string _type;
	};

	template<class T> struct aggregatorCount : public aggregator<T>
	{
		aggregatorCount( double radius, Agent & center, const std::string & type ) : aggregator<T>(radius,center,type), _count(0) {}
		void execute( T neighbor )
		{
			_count++;
		}
		int _count;
	};
	template<class T> struct aggregatorGet : public aggregator<T>
	{
		aggregatorGet( double radius, Agent & center, const std::string & type ) : aggregator<T>(radius,center,type) {}
		void execute( T neighbor )
		{
			_neighbors.push_back(neighbor);
		}
		AgentsVector _neighbors;
	};


public:
	Scheduler() : _id(0), _numTasks(1), _world(0) { }
	void setWorld( World * world ) { _world = world;}
	virtual ~Scheduler() {}

	//! any initializing procedure BEFORE creating agents/rasters (i.e. init communications and define boundaries for parallel nodes) 
	virtual void init( int argc, char *argv[] ) = 0; 
	//! initializing procedures AFTER creating agents/rasters (i.e. send initial data to other nodes in parallel schedulers)
	virtual void initData() = 0;

	//! responsible for executing the agents and update world 
	virtual void executeAgents() = 0;
	//! procedures that need to be executed after simulation (i.e. finish communications for parallel nodes) 
	virtual void finish() = 0;

	//! basic method to use while exploring boundaries of World 
	virtual const Rectangle<int> & getBoundaries() const { return _boundaries; };
	virtual Point2D<int> getRandomPosition() const = 0;

	// ids
	//! id will always be 0 unless the execution is distributed in some way
	const int & getId() const { return _id; }
	//! num tasks will always be 1 unless the execution is distributed in some way
	const int & getNumTasks() const { return _numTasks; }
	//! MPI version of wall time
	virtual double getWallTime() const = 0;

	// agent addition, removal and getters
	//! do anything needed after adding agent to the list of World _agents
	virtual void agentAdded( AgentPtr agent, bool executedAgent ){};
	virtual void removeAgents() = 0;
	virtual void removeAgent(Agent * agent) = 0;
	//! this method will return an agent, both looking at owned and ghost agents
	virtual Agent * getAgent( const std::string & id ) = 0;
	virtual AgentsVector getAgent( const Point2D<int> & position, const std::string & type="all" ) = 0;
	virtual int countNeighbours( Agent * target, const double & radius, const std::string & type) = 0;
	virtual AgentsVector getNeighbours( Agent * target, const double & radius, const std::string & type) = 0;

	// serialization
	virtual void addStringAttribute( const std::string & type, const std::string & key, const std::string & value ) = 0;
	virtual void addIntAttribute( const std::string & type, const std::string & key, int value ) = 0;
	virtual void addFloatAttribute( const std::string & type, const std::string & key, float value ) = 0;
	virtual void serializeAgents( const int & step) = 0;
	virtual void serializeRasters( const int & step) = 0;

	virtual void setValue( DynamicRaster & raster, const Point2D<int> & position, int value ) = 0;
	virtual int getValue( const DynamicRaster & raster, const Point2D<int> & position ) const = 0;
	virtual void setMaxValue( DynamicRaster & raster, const Point2D<int> & position, int value ) = 0;
	virtual int getMaxValue( const DynamicRaster & raster, const Point2D<int> & position ) const = 0;

};

} // namespace Engine

#endif // __Scheduler_hxx__

