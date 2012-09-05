#ifndef __FIXED_AGENT_INITIALIZER__
#define __FIXED_AGENT_INITIALIZER__

#include <AgentInitializer.hxx>
#include <vector>

class TiXmlElement;

namespace Gujarat
{

class GujaratAgent;

class FixedAgentInitializer : public AgentInitializer
{
public:

	FixedAgentInitializer(TiXmlElement* xmlConfig);
	virtual 	~FixedAgentInitializer();
	virtual void	initialize( GujaratAgent* agent );
protected:
	std::vector<int>	_individualsAges;
};

};

#endif
