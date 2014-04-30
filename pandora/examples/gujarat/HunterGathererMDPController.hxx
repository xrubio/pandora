#ifndef __HUNTER_GATHERER_MDP_CONTROLLER__
#define __HUNTER_GATHERER_MDP_CONTROLLER__

#include <AgentController.hxx>
#include <HunterGathererMDPConfig.hxx>
#include <HunterGathererMDPModel.hxx>
#include <HunterGathererMDPState.hxx>
#include <engine/policy.h>
#include <engine/uct.h>

namespace Gujarat
{

/**
 * A controller that makes decisions by casting the decision problem as an MDP
 * and computing the optimal decision to a certain horizon by applying the UCT algorithm.
 */
class HunterGathererMDPController : public AgentController
{

public:

	HunterGathererMDPController( const HunterGathererMDPConfig& cfg );
	virtual ~HunterGathererMDPController();

	void selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions );

private:
	typedef		Online::Policy::random_t< HunterGathererMDPState >	BasePolicy;
	typedef		Online::Policy::UCT::uct_t< HunterGathererMDPState >	UCT;

	HunterGathererMDPConfig						_mdpConfig;
	HunterGathererMDPModel*						_model;
	Online::Policy::random_t< HunterGathererMDPState >*		_uctBasePolicy;	
};

}

#endif // HunterGathererMDPController.hxx
