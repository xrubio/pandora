
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
		std::vector< MoveHomeAction* > possibleActions;
		MoveHomeAction::generatePossibleActions( agent, possibleActions );

		// MRJ: Select Move Home action on a random basis
		dice = Engine::GeneralState::statistics().getUniformDistValue( 0, possibleActions.size() - 1 );

		MoveHomeAction* selectedAction = possibleActions.at(dice);
		possibleActions[dice] = 0;
		for ( unsigned i = 0; i < possibleActions.size(); i++ )
		{
			if (possibleActions.at(i))
			{
				delete possibleActions[i];
			}
		}
	
		do
		{
			dice = Engine::GeneralState::statistics().getUniformDistValue( 0, agentConcrete.getSectors().size()-1 );
		} while ( agentConcrete.getSectors()[dice]->isEmpty() );
		actions.push_back(new ForageAction( agentConcrete.getSectors()[dice], false));
		actions.push_back(selectedAction);
		return;
	}

	do
	{
		dice = Engine::GeneralState::statistics().getUniformDistValue( 0, agentConcrete.getSectors().size()-1 );
	
	} while ( agentConcrete.getSectors()[dice]->isEmpty() );

	actions.push_back(new ForageAction( agentConcrete.getSectors()[dice] ));
}

} // namespace Gujarat

