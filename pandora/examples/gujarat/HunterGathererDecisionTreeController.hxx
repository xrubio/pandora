#ifndef __HUNTER_GATHERER_DECISION_TREE_CONTROLLER__
#define __HUNTER_GATHERER_DECISION_TREE_CONTROLLER__

#include <AgentController.hxx>

namespace Gujarat
{

class HunterGatherer;
class Sector;

class HunterGathererDecisionTreeController : public AgentController
{
public:
	HunterGathererDecisionTreeController();
	virtual ~HunterGathererDecisionTreeController();

	virtual	MDPAction*	selectAction( GujaratAgent & agent );
	
	MDPAction*         shouldDoNothing(  HunterGatherer & agent  );
	MDPAction*         shouldForage(  HunterGatherer & agent  );
	MDPAction*         shouldMoveHome(  HunterGatherer & agent  );
	
	Sector*         getMaxBiomassSector( HunterGatherer & agent );
	unsigned        getDoNothingDaysCovered() const { return _DoNothingDaysCovered; } 

private:
	unsigned                _DoNothingDaysCovered;
};

}

#endif // HunterGathererDecisionTreeController.hxx
