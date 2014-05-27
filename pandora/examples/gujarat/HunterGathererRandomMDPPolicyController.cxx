
#include <HunterGathererRandomMDPPolicyController.hxx>

#include <HunterGatherer.hxx>

#include <Logger.hxx>
#include <GeneralState.hxx>

namespace Gujarat
{

// 	HunterGathererMDPController::HunterGathererMDPController( const HunterGathererMDPConfig& cfg ) : _mdpConfig( cfg ), _model(0), _uctBasePolicy(0)
	
HunterGathererRandomMDPPolicyController::HunterGathererRandomMDPPolicyController(const HunterGathererMDPConfig& cfg) :
	_model()
{
	_model.setup( cfg );
}

HunterGathererRandomMDPPolicyController::~HunterGathererRandomMDPPolicyController() { }
	
	
void HunterGathererRandomMDPPolicyController::selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions )
{	
	_model.reset(agent);
	//auto s = _model.init();
	Gujarat::HunterGathererMDPState s = _model.init();
	
// 	std::cout << "Starting Random action selection on root state: " << s << std::endl;

	RandomPolicy randomPolicy(_model);
	Problem::action_t aIndex = randomPolicy(s);
	MDPAction* a = s.availableActions(aIndex)->copy();
	actions.push_back(a);

// 	log_DEBUG(agent.getId() + "_controller",  "\taction_selected=" << a->describe());
// 	std::cout << "Selected action: " << a->describe() << std::endl << std::endl	;
}

	
	


}

