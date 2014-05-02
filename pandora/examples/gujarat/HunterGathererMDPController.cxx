
#include <HunterGathererMDPController.hxx>
#include <HunterGatherer.hxx>

#include <Logger.hxx>

namespace Gujarat
{

HunterGathererMDPController::HunterGathererMDPController( const HunterGathererMDPConfig& cfg )
	: _mdpConfig( cfg ), _model(cfg), _uctBasePolicy(_model)
{}

HunterGathererMDPController::~HunterGathererMDPController()
{}


void HunterGathererMDPController::selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions )
{
	_model.reset(agent);
	auto state = _model.init();
	log_DEBUG(agent.getId() + "_controller",  "Starting UCT run on seed state: " << state);

	UCT uctPolicy( _uctBasePolicy, (unsigned)_mdpConfig.getWidth(), (unsigned)_mdpConfig.getHorizon(), _mdpConfig.getExplorationBonus(), false );
	Problem::action_t aIndex = uctPolicy(state);

	MDPAction* a = state.availableActions(aIndex)->copy();
	actions.push_back(a);

	log_DEBUG(agent.getId() + "_controller",  "Action selected:" << a->describe());
}

}

