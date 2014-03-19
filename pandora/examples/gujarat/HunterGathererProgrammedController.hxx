#ifndef __HUNTER_GATHERER_PROGRAMMED_CONTROLLER__
#define __HUNTER_GATHERER_PROGRAMMED_CONTROLLER__

#include <AgentController.hxx>


//*?
#include <string>

namespace Gujarat
{

class HunterGathererProgrammedController : public AgentController
{
public:
	HunterGathererProgrammedController();
	virtual ~HunterGathererProgrammedController();

	void selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions );

//*?
	int getWidth() {return 0;}

};

}

#endif // HunterGathererProgrammedController.hxx
