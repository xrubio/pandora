#ifndef __HUNTER_GATHERER_RANDOM_CONTROLLER__
#define __HUNTER_GATHERER_RANDOM_CONTROLLER__

#include <AgentController.hxx>

//*?
#include <string>

namespace Gujarat
{

class MDPAction;
class HunterGatherer;
class GujaratAgent;
class Sector;

class HunterGathererProgrammedController : public AgentController
{
public:
	HunterGathererProgrammedController();
	virtual ~HunterGathererProgrammedController();

	void selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions );
	
	MDPAction*         shouldForage(  HunterGatherer & agent  );
	MDPAction*         shouldMoveHome(  HunterGatherer & agent  );
	



//*?
	int getWidth(){return 0;}

};

}

#endif // HunterGathererProgrammedController.hxx
