
#include <DoNothingAction.hxx>
#include <Agent.hxx>
#include <GujaratAgent.hxx>
#include <HunterGathererMDPState.hxx>
#include <iostream>

#include <Logger.hxx>
#include <GeneralState.hxx>

namespace Gujarat
{

DoNothingAction::DoNothingAction()
{
}

DoNothingAction::~DoNothingAction()
{
}

MDPAction*	DoNothingAction::copy() const
{
	return new DoNothingAction();
}

void DoNothingAction::execute( Engine::Agent& agent )
{
	std::stringstream logName;
	logName << agent.getWorld()->getId() << "_" << agent.getId() << "_actions";
	log_DEBUG(logName.str(), " executing DoNothing action"); 
}

void DoNothingAction::generatePossibleActions( GujaratAgent& agent, std::vector< DoNothingAction* >& actions )
{
	actions.push_back( new DoNothingAction() );
}

void DoNothingAction::executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const
{
	//std::cout << "DO NOTHING" << std::endl;
}

std::string DoNothingAction::describe() const
{
	return "do_nothing()";
}


}
