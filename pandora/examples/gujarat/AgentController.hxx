#ifndef __AGENT_CONTROLLER__
#define __AGENT_CONTROLLER__

namespace Gujarat
{

class MDPAction;
class GujaratAgent;

class AgentController
{
public:
	
	AgentController();
	virtual ~AgentController();

	// MRJ: returned object ownership is caller's
	virtual MDPAction* selectAction( GujaratAgent & agent ) = 0;
};

}

#endif // AgentController.hxx
