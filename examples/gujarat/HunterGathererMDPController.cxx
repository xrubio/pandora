
#include <HunterGathererMDPController.hxx>
#include <HunterGatherer.hxx>

#include <Logger.hxx>
#include <GeneralState.hxx>

namespace Gujarat
{

HunterGathererMDPController::HunterGathererMDPController( const HunterGathererMDPConfig& cfg ) : _mdpConfig( cfg ), _model(0), _uctBasePolicy(0)
{
	_model = new HunterGathererMDPModel();
	_model->setup( cfg );
	_uctBasePolicy = new BasePolicy( *_model ); 
}

HunterGathererMDPController::~HunterGathererMDPController()
{
	delete _uctBasePolicy;
	delete _model;
}


void HunterGathererMDPController::selectActions( GujaratAgent & agent, std::list<MDPAction*> & actions )
{
	log_DEBUG(agent.getId()+"_controller",  "timestep=" << agent.getWorld()->getCurrentTimeStep());
	log_DEBUG(agent.getId()+"_controller",  "\tagent.position=" << agent.getPosition());

	_model->reset(agent);

	UCT*	uctPolicy = new UCT( *_uctBasePolicy, 
	(unsigned)_mdpConfig.getWidth(), (unsigned)_mdpConfig.getHorizon(), _mdpConfig.getExplorationBonus(), false );

	Problem::action_t aIndex = (*uctPolicy)( _model->init() );

	MDPAction* a = _model->init().availableActions(aIndex)->copy();
	
	delete uctPolicy;

	log_DEBUG(agent.getId()+"_controller",  "\taction_selected=" << a->describe());


	actions.push_back(a);
}

}
