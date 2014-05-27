
#ifndef __HUNTER_GATHERER_MDP_RANDOM_CONTROLLER__
#define __HUNTER_GATHERER_MDP_RANDOM_CONTROLLER__

#include <AgentController.hxx>
#include <HunterGathererMDPConfig.hxx>
#include <HunterGathererMDPModel.hxx>
#include <HunterGathererMDPState.hxx>
#include <engine/policy.h>
#include <engine/uct.h>

namespace Gujarat
{

class GujaratAgent;

/**
 * A RandomMDPPolicy selects an action randomly in exactly the same manner as the UCT random base policy.
 * Useful for comparison with the UCT policy itself.
 */
class HunterGathererRandomMDPPolicyController : public AgentController
{
public:
	HunterGathererRandomMDPPolicyController(const HunterGathererMDPConfig& cfg);
	virtual ~HunterGathererRandomMDPPolicyController();

	void selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions );
	
	int getWidth(){return 0;}
	
private:
	typedef Online::Policy::random_t< HunterGathererMDPState > RandomPolicy;
	HunterGathererMDPModel _model;
};

}

#endif 
