#ifndef __AGENT_CONTROLLER__
#define __AGENT_CONTROLLER__

#include <list>
//*?
#include <string>

namespace Engine { class Action; }  // forward declaration

namespace Gujarat
{
class GujaratAgent;

class AgentController
{
public:
	
	AgentController();
	virtual ~AgentController();
	
	virtual void selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions ) = 0;

	//*?
	virtual  int getWidth() = 0;
};

}

#endif // AgentController.hxx
