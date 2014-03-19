
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


void HunterGathererMDPController::selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions )
{
	log_DEBUG(agent.getId()+"_controller",  "timestep=" << agent.getWorld()->getCurrentTimeStep());
	log_DEBUG(agent.getId()+"_controller",  "\tagent.position=" << agent.getPosition());


//try{	
	
	_model->reset(agent);
/*
}catch(const std::exception & e){
	std::cerr << "EXC 1" << std::endl;
	assert(0==1);
	}
	*/
UCT*	uctPolicy;

//try{	

	//UCT*	uctPolicy = new UCT( *_uctBasePolicy, 	(unsigned)_mdpConfig.getWidth(), (unsigned)_mdpConfig.getHorizon(), _mdpConfig.getExplorationBonus(), false );
	
	uctPolicy = new UCT( *_uctBasePolicy, 	(unsigned)_mdpConfig.getWidth(), (unsigned)_mdpConfig.getHorizon(), _mdpConfig.getExplorationBonus(), false );
/*	
}catch(const std::exception & e){
	std::cerr << "EXC 2" << std::endl;
	assert(0==1);
	}
*/
	Problem::action_t aIndex;

try{	

	//Problem::action_t aIndex = (*uctPolicy)( _model->init() );
	aIndex = (*uctPolicy)( _model->init() );
	
}catch(const std::exception & e){
	std::cerr << "EXC 3" << std::endl;
	assert(0==1);
	}
	
	MDPAction* a;
	
try{	
	//MDPAction* a = _model->init().availableActions(aIndex)->copy();
	a = _model->init().availableActions(aIndex)->copy();
}catch(const std::exception & e){
	std::cerr << "EXC 4" << std::endl;
	assert(0==1);
	}

try{	

	delete uctPolicy;
}catch(const std::exception & e){
	std::cerr << "EXC 5" << std::endl;
	assert(0==1);
	}

try{

	_model->clean();        
}catch(const std::exception & e){
        std::cerr << "EXC 6" << std::endl;
        assert(0==1);
        }

	
	//log_DEBUG(agent.getId()+"_controller",  "\taction_selected=" << a->describe());

	//log_INFO(agent.getId()+"_controller",  "\taction_selected=" << a->describe());
try{	
	
	actions.push_back(a);	
}catch(const std::exception & e){
	std::cerr << "EXC 7" << std::endl;
	assert(0==1);
	}
	
	
	
}

}

