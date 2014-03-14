
#ifndef __Scheduler_hxx__
#define __Scheduler_hxx__

namespace Engine
{

/** Scheduler is the base class to create simulation schedulers that control the flow of World and Agents execution
  */
class Scheduler
{
public:
	Scheduler();
	virtual ~Scheduler();

	// main exeecution methods
	void init( int argc, char *argv[] );
	// initialize data processes
	void initData( std::vector<StaticRaster * > rasters, std::vector<bool> & dynamicRasters, std::vector<bool> serializeRasters );

	//! responsible for executing the agents and update world 
	void executeAgents();
	void finish();

	// geometry 
	const Rectangle<int> & getBoundaries() const;
	const Rectangle<int> & getOwnedArea() const;
	//! transform from global coordinates to real coordinates (in terms of world position)
	Point2D<int> getRealPosition( const Point2D<int> & globalPosition ) const;
	Point2D<int> getRandomPosition() const;

	// ids
	//! id will always be 0 unless the execution is distributed in some way
	const int & getId() const;
	//! num tasks will always be 0 unless the execution is distributed in some way
	const int & getNumTasks() const;
	//! MPI version of wall time
	double getWallTime() const;

	// agent addition, removal and getters
	void agentAdded( Agent * agent, bool executedAgent );
	void removeAgents();
	void removeAgent(Agent * agent);
	//! this method will return an agent, both looking at owned and ghost agents
	Agent * getAgent( const std::string & id );
	AgentsVector getAgent( const Point2D<int> & position, const std::string & type="all" );
	int countNeighbours( Agent * target, const double & radius, const std::string & type);
	AgentsVector getNeighbours( Agent * target, const double & radius, const std::string & type);

	// serialization
	void addStringAttribute( const std::string & type, const std::string & key, const std::string & value );
	void addIntAttribute( const std::string & type, const std::string & key, int value );
	void serializeAgents( const int & step);
	void serializeRasters( const int & step);

};

} // namespace Engine

#endif // __Scheduler_hxx__

