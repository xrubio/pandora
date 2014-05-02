#ifndef __HUNTER_GATHERER_DECISION_TREE_CONTROLLER__
#define __HUNTER_GATHERER_DECISION_TREE_CONTROLLER__

#include <vector>

#include <AgentController.hxx>
#include <GujaratConfig.hxx>

namespace Gujarat
{

class MDPAction; class HunterGatherer; class GujaratAgent; class Sector;

class HunterGathererDecisionTreeController : public AgentController
{
public:
	
	//! Number of iterations used to estimate the (stochastic) reward that can be obtained 
	const unsigned BIOMASS_ESTIMATION_ITERATIONS = 10;
	
	HunterGathererDecisionTreeController();
	virtual ~HunterGathererDecisionTreeController();

	void selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions );

	//! Decides whether the agent should forage on the current sector based on an estimation of the possible biomass
	//! that can be obtained by doing so, and on the caloric needs of the agent.
	MDPAction*	shouldForage(  HunterGatherer & agent  );
	
	MDPAction*	shouldMoveHome(  HunterGatherer & agent  );
	
	int         getMaxBiomassSector( HunterGatherer & agent );

	int getSectorIdxMatchingDirection( const HunterGatherer & agent, const std::vector<Sector *> & sectors, const int direction  ) const;
};

}

#endif // HunterGathererDecisionTreeController.hxx
