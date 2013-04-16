#ifndef __ScenarioConfig_hxx__
#define __ScenarioConfig_hxx__

#include <Config.hxx>

namespace Panic 
{

class ScenarioConfig : public Engine::Config
{
	int _size;

	// agents
	int _numAgents;

public:
	ScenarioConfig();
	virtual ~ScenarioConfig();
	void extractParticularAttribs(TiXmlElement *pRoot);
	int getSize() const;
	
	friend class Scenario;
};

} // namespace Panic 

#endif 
