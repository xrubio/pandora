#ifndef __HUNTER_GATHERER_DECISION_TREE_CONTROLLER__
#define __HUNTER_GATHERER_DECISION_TREE_CONTROLLER__

#include <vector>

#include <AgentController.hxx>
#include <GujaratConfig.hxx>

//*?
#include <string>

namespace Gujarat
{

class MDPAction;
class HunterGatherer;
class GujaratAgent;
class Sector;


class HunterGathererDecisionTreeController : public AgentController
{
public:
	HunterGathererDecisionTreeController();
	virtual ~HunterGathererDecisionTreeController();

	void selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions );
	
	//MDPAction*         shouldDoNothing(  HunterGatherer & agent  );
	MDPAction*	shouldForage(  HunterGatherer & agent  );
	MDPAction*	shouldForageWithWalkEstimation( HunterGatherer & agent );
	MDPAction*	shouldMoveHome(  HunterGatherer & agent  );
	
	int         getMaxBiomassSector( HunterGatherer & agent );
//	unsigned        getDoNothingDaysCovered() const { return _DoNothingDaysCovered; } 

//*?
	int getWidth(){return 0;}
	
	int getSectorIdxMatchingDirection( const HunterGatherer & agent, const std::vector<Sector *> & sectors, const int direction  ) const;


private:
	unsigned                _DoNothingDaysCovered;
};

}

#endif // HunterGathererDecisionTreeController.hxx
