#ifndef __HUNTER_GATHERER_PROGRAMMED_CONTROLLER__
#define __HUNTER_GATHERER_PROGRAMMED_CONTROLLER__

#include <AgentController.hxx>

namespace Gujarat
{

class HunterGathererProgrammedController : public AgentController
{
public:
	HunterGathererProgrammedController();
	virtual ~HunterGathererProgrammedController();

	void selectActions( GujaratAgent & agent, std::list<MDPAction*> & actions );
};

}

#endif // HunterGathererProgrammedController.hxx
