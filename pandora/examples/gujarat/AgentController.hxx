#ifndef __AGENT_CONTROLLER__
#define __AGENT_CONTROLLER__

#include <list>

namespace Gujarat
{

class MDPAction;
class GujaratAgent;

class AgentController
{
public:
	
	AgentController();
	virtual ~AgentController();
	
	virtual void selectActions( GujaratAgent & agent, std::list<MDPAction*> & actions ) = 0;
};

}

#endif // AgentController.hxx
