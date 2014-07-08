
#include <HunterGathererProgrammedController.hxx>

#include <vector>
#include <iostream>

#include <MoveHomeAction.hxx>
#include <ForageAction.hxx>
#include <HunterGatherer.hxx>
#include <DoNothingAction.hxx>

#include <Statistics.hxx>
#include <GeneralState.hxx>

namespace Gujarat
{

HunterGathererProgrammedController::HunterGathererProgrammedController()
{
}

HunterGathererProgrammedController::~HunterGathererProgrammedController()
{
}

void HunterGathererProgrammedController::selectActions( GujaratAgent & agent, std::list<MDPAction*> & actions )
{
	// TODO: which order must follow the actions? random?
	// now random
	HunterGatherer & agentConcrete = dynamic_cast<HunterGatherer&>( agent );

	// action pack : move Home, hunting, gathering
	int dice = Engine::GeneralState::statistics().getUniformDistValue(1,10);
	if ( dice > 8 ) // p=0.2 agent chooses to move its home
	{
		do
		{
			dice = Engine::GeneralState::statistics().getUniformDistValue( 0, agentConcrete.getSectors().size()-1 );
		}
		while ( agentConcrete.getSectors().at(dice)->isEmpty() );

		actions.push_back(new ForageAction( agentConcrete.getSectors().at(dice), false));
	}
	else
	{
		std::vector< MoveHomeAction* > possibleActions;
		MoveHomeAction::generatePossibleActions( agent, possibleActions );
		dice = Engine::GeneralState::statistics().getUniformDistValue( 0, possibleActions.size() - 1 );
		MoveHomeAction * selectedAction = possibleActions.at(dice);
		possibleActions.at(dice) = 0;
		
		for ( unsigned i = 0; i < possibleActions.size(); i++ )
		{
			if (possibleActions.at(i))
			{
				delete possibleActions[i];
			}
		}
		actions.push_back(selectedAction);
		// TODO move/forage
	}
}

} // namespace Gujarat

