
#ifndef __Scheduler_hxx__
#define __Scheduler_hxx__

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
	World * _world;
	
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
	virtual const Rectangle<int> & getBoundaries() const = 0;
	virtual Point2D<int> getRandomPosition() const = 0;

	// ids
	//! id will always be 0 unless the execution is distributed in some way
	const int & getId() const { return _id; }
	//! num tasks will always be 1 unless the execution is distributed in some way
	const int & getNumTasks() const { return _numTasks; }
	//! MPI version of wall time
	virtual double getWallTime() const = 0;

	// agent addition, removal and getters
	virtual void agentAdded( Agent * agent, bool executedAgent ) = 0;
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
	virtual void serializeAgents( const int & step) = 0;
	virtual void serializeRasters( const int & step) = 0;

	virtual void setValue( Raster & raster, const Point2D<int> & position, int value ) = 0;
	virtual int getValue( const Raster & raster, const Point2D<int> & position ) const = 0;
	virtual void setMaxValueAt( Raster & raster, const Point2D<int> & position, int value ) = 0;
	virtual int getMaxValueAt( const Raster & raster, const Point2D<int> & position ) const = 0;

};

} // namespace Engine

#endif // __Scheduler_hxx__

