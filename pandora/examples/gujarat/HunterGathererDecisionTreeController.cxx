
#include <HunterGathererDecisionTreeController.hxx>

#include <vector>
#include <iostream>

#include <MoveHomeAction.hxx>
#include <ForageAction.hxx>
#include <DoNothingAction.hxx>
#include <HunterGatherer.hxx>
#include <SettlementAreas.hxx>
#include <GujaratWorld.hxx>
#include <Sector.hxx>
#include <Logger.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>

namespace Gujarat
{

HunterGathererDecisionTreeController::HunterGathererDecisionTreeController()
{
//	_DoNothingDaysCovered = 1; 
}

HunterGathererDecisionTreeController::~HunterGathererDecisionTreeController()
{
}


int HunterGathererDecisionTreeController::getMaxBiomassSector(  HunterGatherer & agent  )
{
	// Make Forage actions
	std::vector< int > validActionSectorsIdx;
	//agent.updateKnowledge( agent.getPosition(), agent.getWorld()->getDynamicRaster(eResources), actionSectors );
	
	//agent.updateKnowledge();	
	assert(!agent.getLRSectors().empty());
	if(agent.getLRSectors().empty())
	{
		agent.updateKnowledge();
		//return -1;
	}
	
	// MRJ: Remove empty sectors if any
	int maxBiomass    = -1;
	for ( int i = 0; i < agent.getLRSectors().size(); i++ )
	{
		if( !(agent.getLRSectors()[i]->isEmpty()) )
		{
			if( agent.getLRSectors()[i]->getBiomassAmount() > maxBiomass )
			{
				maxBiomass = agent.getLRSectors()[i]->getBiomassAmount();
				validActionSectorsIdx.clear();
			}
			else if(agent.getLRSectors()[i]->getBiomassAmount() < maxBiomass )
			{
				continue;
			}
			validActionSectorsIdx.push_back( i );
		}
	}	
	assert(maxBiomass != -1);
	//std::random_shuffle(validActionSectorsIdx.begin(), validActionSectorsIdx.end());
	int result = validActionSectorsIdx[Engine::GeneralState::statistics().getUniformDistValue(0,validActionSectorsIdx.size()-1)];
	validActionSectorsIdx.clear();
	return result;
}

/*
MDPAction* HunterGathererDecisionTreeController::shouldDoNothing( HunterGatherer & agent )
{	
	// CollectedResources > ConsumedResourcesByAgent * #days
	if( agent.getOnHandResources() > agent.computeConsumedResources( getDoNothingDaysCovered() ) )
	{
		return new DoNothingAction();
	}
	else
	{
		return NULL;
	}
}
*/
MDPAction* HunterGathererDecisionTreeController::shouldForage( HunterGatherer & agent )
{	
	//Sector * maxSector = getMaxBiomassSector(agent);
	int maxSectorIdx = getMaxBiomassSector(agent);
	if(maxSectorIdx < 0) 
	{
		return 0;
	}

	int biomass = agent.getLRSectors()[maxSectorIdx]->getBiomassAmount();

	// thinking that the agent will forage at most 9 cells
	int numCells = agent.getHRSectors()[maxSectorIdx]->numCells();

	float maxNumCells = agent.getNrAvailableAdults()*agent.getAvailableTime()/agent.getForageTimeCost();
	float percentageOfCells = maxNumCells/numCells;

	std::cout 	<< "avail time:" 		<< agent.getAvailableTime()
				<< ",forage time cost:"	<< agent.getForageTimeCost()
				<< ",maxNumCells:" 		<< maxNumCells 
				<< std::endl;
	
	/*
	std::cout << agent << " required needs: " << agent.computeConsumedResources(1) << " max biomass: " << biomass << " potential calories: " << agent.convertBiomassToCalories(biomass) << " adults: " << agent.getNrAvailableAdults() << " max num cells: " << maxNumCells << " of: " << numCells  << " percentage: " << percentageOfCells << " estimation: " <<  0.5*percentageOfCells*(agent.convertBiomassToCalories(biomass)) << std::endl;	
	*/
	// we check if, collecting 50% of real biomass, needs will be arrived.
	// ISSUE : the condition say : everybody eats or I do not go out to forage
	// You should go and forage something, at least for the couple.
	if( 0.5*percentageOfCells*(agent.convertBiomassToCalories(biomass)) >= agent.computeConsumedResources(1) )
	{
		return new ForageAction(agent.getHRSectors()[maxSectorIdx],agent.getLRSectors()[maxSectorIdx], true);
	}
	//std::cout << "maxSector: " << maxSector << std::endl;
	//delete maxSector;
	return 0;
}

MDPAction* HunterGathererDecisionTreeController::shouldMoveHome( HunterGatherer & agent )
{	
	std::vector< MoveHomeAction* > possibleActions;
	
	agent.updateKnowledge();
	
	// look for an action that collects most calories before moving
	//Sector * bestSector = getMaxBiomassSector(agent);
	int bestSectorIdx = getMaxBiomassSector(agent);
	if(bestSectorIdx < 0) 
	{
		return 0;
	}
	MoveHomeAction::generatePossibleActions( agent, possibleActions );
	
	// generatePossibleAction generates potential MoveHome actions ordered by the score of potential SettlementAreas
	// thus, the first possible action with the best potential biomass is the optimal move
	MDPAction * chosenAction = 0; 
	// LEAK arranged
	//std::random_shuffle(possibleActions.begin(), possibleActions.end());
	
	int idx = Engine::GeneralState::statistics().getUniformDistValue(0,possibleActions.size()-1);
	chosenAction = possibleActions.at(idx);
	possibleActions[idx] = 0;
	for(int i=0; i<possibleActions.size(); i++)
	{
		delete possibleActions.at(i);
	}		
	possibleActions.clear();
	return chosenAction;
}

void HunterGathererDecisionTreeController::selectActions( GujaratAgent & agent, std::list<MDPAction*> & actions )
{	
	HunterGatherer & agentConcrete = dynamic_cast<HunterGatherer&>( agent );
        //Decission Tree: DoNothing --> Forage --> MoveHome

	/*
	MDPAction * selectedAction = 0;
	selectedAction = shouldDoNothing(agentConcrete);
	if(selectedAction)
	{
		actions.push_back(selectedAction);
		log_INFO(logName.str(), "timestep: " << agent.getWorld()->getCurrentTimeStep() << " agent: " << agent << " will do nothing");
		return actions;
	}
	*/

	MDPAction * selectedAction = shouldForage(agentConcrete);
	if(selectedAction)
	{
		actions.push_back(selectedAction);
		return;
	}
	selectedAction = shouldMoveHome(agentConcrete);
	if(selectedAction)
	{
		actions.push_back(selectedAction);
	}
}

}

