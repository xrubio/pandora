#ifndef __ScenarioConfig_hxx__
#define __ScenarioConfig_hxx__

#include <Config.hxx>
#include <Point2D.hxx>
#include <map>

namespace Panic 
{

class ScenarioConfig : public Engine::Config
{
	typedef std::map<std::string, std::string> SupportRastersMap;

	int _size;
	std::string _obstacleFile;

	// agents
	int _numAgents;

	float _agentCompressionWeight;
	float _wallCompressionWeight;
	float _compressionThreshold;

	// place where panic begins
	Engine::Point2D<int> _initPanic;
	int _initPanicRadius;
	int _contagion;

	// rasters map to show in the final outcome
	// they are not used during the execution
	SupportRastersMap _supportMaps;

	// type of distribution (random, file and then a file="foo"
	std::string _initAgentsDistributionType;
	std::string _initAgentsDistributionFile;

public:
	ScenarioConfig();
	virtual ~ScenarioConfig();
	void extractParticularAttribs(TiXmlElement *pRoot);
	int getSize() const;
	
	friend class Scenario;
	friend class PanicAgent;
};

} // namespace Panic 

#endif 
