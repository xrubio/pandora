
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


Sector* HunterGathererDecisionTreeController::getMaxBiomassSector(  HunterGatherer & agent  )
{
	// Make Forage actions
	std::vector< Sector* > validActionSectors;
	std::vector< Sector* > actionSectors;

	agent.updateKnowledge( agent.getPosition(), agent.getWorld()->getDynamicRaster(eResources), actionSectors );

	// MRJ: Remove empty sectors if any
	for ( unsigned i = 0; i < actionSectors.size(); i++ )
	{
		if ( actionSectors[i]->isEmpty() )
		{
			delete actionSectors[i];
			continue;
		}
		validActionSectors.push_back( actionSectors[i] );
	}	
	
	if(validActionSectors.empty())
	{
		return 0;
	}

	// Find Sector with Maximum Biomass to Forage 
	int maxBiomass = 0;
	unsigned maxBiomassIdx = 0;
	for ( unsigned i = 0; i < validActionSectors.size(); i++ )
	{
		if( validActionSectors[i]->getBiomassAmount() > maxBiomass )
		{
			maxBiomass = validActionSectors[i]->getBiomassAmount();
			maxBiomassIdx = i;
		}
	}

	for ( unsigned i = 0; i < validActionSectors.size(); i++ )
		if( i != maxBiomassIdx)
			delete validActionSectors[i];

	return validActionSectors.at(maxBiomassIdx);

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
	Sector* maxSector = getMaxBiomassSector(agent);	

	if(!maxSector) 
	{
		return 0;
	}

	int biomass = maxSector->getBiomassAmount();

	// thinking that the agent will forage at most 9 cells
	int numCells = maxSector->numCells();

	float maxNumCells = agent.getNrAvailableAdults()*agent.getAvailableTime()/agent.getForageTimeCost();
	float percentageOfCells = maxNumCells/numCells;
//	std::cout << agent << " required needs: " << agent.computeConsumedResources(1) << " max biomass: " << biomass << " potential calories: " << agent.convertBiomassToCalories(biomass) << " adults: " << agent.getNrAvailableAdults() << " max num cells: " << maxNumCells << " of: " << numCells  << " percentage: " << percentageOfCells << " estimation: " <<  0.5*percentageOfCells*(agent.convertBiomassToCalories(biomass)) << std::endl;	

	// we check if, collecting 50% of real biomass, needs will be arrived
	if( 0.5*percentageOfCells*(agent.convertBiomassToCalories(biomass)) >= agent.computeConsumedResources(1) )
	{
		return new ForageAction(maxSector, true);
	}
	//std::cout << "maxSector: " << maxSector << std::endl;
	delete maxSector;
	return 0;
}

MDPAction* HunterGathererDecisionTreeController::shouldMoveHome( HunterGatherer & agent )
{	
	std::vector< MoveHomeAction* > possibleActions;
	MoveHomeAction::generatePossibleActions( agent, possibleActions );

	// look for an action that collects most calories before moving
	Sector * bestSector = getMaxBiomassSector(agent);
	// generatePossibleAction generates potential MoveHome actions ordered by the score of potential SettlementAreas
	// thus, the first possible action with the best potential biomass is the optimal move
	MDPAction * chosenAction = 0; 
	for(int i=0; i<possibleActions.size(); i++)
	{
		if(!chosenAction && (possibleActions.at(i)->getPotentialBiomass()==bestSector->getBiomassAmount()))
		{
			chosenAction = possibleActions.at(i);
		}
		else
		{
			delete possibleActions.at(i);
		}
	}
	delete bestSector;
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

