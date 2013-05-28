#ifndef __Scenario_hxx
#define __Scenario_hxx

#include <World.hxx>
#include <GeneralState.hxx>
#include "ScenarioConfig.hxx"

namespace Engine
{
	class Simulation;
}

namespace Panic 
{
class PanicAgent;

enum Rasters
{
	// obstacle, 1 if true, 0 if false
	eObstacles,
	// exits, 1 if true, 0 if false
	eExits,
	// number of agents in each cell
	eNumAgents,
	// number of adjacent walls
	eWalls,
	// compression
	eCompression,
	eDeaths,
	ePanic
};

class Scenario: public Engine::World
{
	typedef std::list<Engine::Point2D<int> > ExitsList;
	ExitsList _exits;
	ScenarioConfig _config;

	void createAgents();
	void createRasters();

	void fillExitList();
	void computeShortestExit(PanicAgent & agent );
	void stepEnvironment();
	bool maxCapacity( const Engine::Point2D<int> & position );
public:
	Scenario( Engine::Simulation &simulation, ScenarioConfig &config );
	virtual ~Scenario();
};

} // namespace Panic

#endif // __Scenario_hxx
