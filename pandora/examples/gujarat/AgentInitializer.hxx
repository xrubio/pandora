#ifndef __AGENT_INITIALIZER__
#define __AGENT_INITIALIZER__

class TiXmlElement;

namespace Gujarat
{

class GujaratAgent;

class AgentInitializer
{
public:
	AgentInitializer( TiXmlElement* xmlConfig );
	virtual ~AgentInitializer();

	virtual void	initialize( GujaratAgent* agent ) = 0;
};

}

#endif // AgentInitializer.hxx
