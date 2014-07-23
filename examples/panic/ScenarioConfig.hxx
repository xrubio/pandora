#ifndef __ScenarioConfig_hxx__
#define __ScenarioConfig_hxx__

#include <Config.hxx>
#include <Size.hxx>
#include <Point2D.hxx>
#include <map>
#include <list>

namespace Panic 
{

struct PanicEvent
{
	Engine::Point2D<int> _position;	
	int _radius;
	// if != 0 is defines the a zone that will be transformed to obstacle and all the agents there will die
	int _obstacleRadius;
	int _step;
};

class ScenarioConfig : public Engine::Config
{
	typedef std::map<std::string, std::string> SupportRastersMap;
	typedef std::list<PanicEvent> PanicEventsList;

	std::string _obstacleFile;

	// agents
	int _numAgents;
	int _contagion;
	float _agentCompressionWeight;
	float _wallCompressionWeight;
	int _compressionThreshold;
	int _bodiesToObstacle;
	int _knowledge;

	// rasters map to show in the final outcome
	// they are not used during the execution
	SupportRastersMap _supportMaps;

	// list of panic events
	PanicEventsList _panicEvents;

	// type of distribution (random, file and then a file="foo"
	std::string _initAgentsDistributionType;
	std::string _initAgentsDistributionFile;

public:
	ScenarioConfig( const std::string & xmlFile );
	virtual ~ScenarioConfig();
	void loadParams(); 
	
	friend class Scenario;
	friend class PanicAgent;
};

} // namespace Panic 

#endif 
