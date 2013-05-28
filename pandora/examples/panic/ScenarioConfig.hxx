#ifndef __ScenarioConfig_hxx__
#define __ScenarioConfig_hxx__

#include <Config.hxx>
#include <Point2D.hxx>

namespace Panic 
{

class ScenarioConfig : public Engine::Config
{
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
