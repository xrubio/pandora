
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

#include <GeneralState.hxx>
#include <Statistics.hxx>

namespace Gujarat
{

HunterGathererDecisionTreeController::HunterGathererDecisionTreeController()
{
	_DoNothingDaysCovered = 1; 
}

HunterGathererDecisionTreeController::~HunterGathererDecisionTreeController()
{
}


Sector* HunterGathererDecisionTreeController::getMaxBiomassSector(  HunterGatherer & agent  )
{
	// Make Forage actions
	std::vector< Sector* > validActionSectors;
	std::vector< Sector* > actionSectors;

	agent.updateKnowledge( agent.getPosition(), agent.getWorld()->getDynamicRaster( "resources" ), actionSectors );

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
	
	if( validActionSectors.empty() ) return NULL;

	// Find Sector with Maximum Biomass to Forage 
	int maxBiomass = validActionSectors[0]->getBiomassAmount();
	unsigned maxBiomassIdx = 0;
	for ( unsigned i = 1; i < validActionSectors.size(); i++ )
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

	return validActionSectors[ maxBiomassIdx ];

}

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

MDPAction* HunterGathererDecisionTreeController::shouldForage( HunterGatherer & agent )
{	
	Sector* maxSector = getMaxBiomassSector(agent);	

	if( maxSector == NULL) 
		return NULL;
	
	MDPAction* forage = NULL;
	if( maxSector->getBiomassAmount() >=  agent.computeConsumedResources(1) )
		forage = new ForageAction( maxSector, true );

	return forage;
	

}

MDPAction* HunterGathererDecisionTreeController::shouldMoveHome( HunterGatherer & agent )
{
	const Engine::Point2D<int>& agentPos = agent.getPosition();
	
	// Put a BOX around the home range.	
	int boxOriginX = agentPos._x - agent.getHomeMobilityRange();
	int boxOriginY = agentPos._y - agent.getHomeMobilityRange();
	int boxSizeX = 2*agent.getHomeMobilityRange()+1;
	int boxSizeY = 2*agent.getHomeMobilityRange()+1;
	Engine::Point2D<int> boxOrigin(boxOriginX, boxOriginY);	
	Engine::Point2D<int> boxSize(boxSizeX,boxSizeY);	
	Engine::Rectangle<int> unTrimmedHomeBox(boxOrigin,boxSize);
	Engine::Rectangle<int> homeBox;
	
	unTrimmedHomeBox.intersection(agent.getWorld()->getOverlapBoundaries(),homeBox);
	
	// Retrieve the areas that have intersection non zero with homeBox
	const GujaratWorld * world              = (GujaratWorld *)agent.getWorld();
	const SettlementAreas * settlementAreas = world->getSettlementAreas();
	std::vector<int> candidates(0);
	settlementAreas->intersectionFilter(homeBox,candidates);

	assert( !candidates.empty() );
	
	// Select Areas with maximum score.
	// Sort candidates following scores	
	std::make_heap(candidates.begin(),candidates.end(),Gujarat::compareSettlementAreas(settlementAreas));
	
	
	MDPAction* moveHome = NULL;
	Engine::Point2D<int> newHomeLocation;
	
	assert( !candidates.empty() );
	std::vector< Engine::Point2D<int> > dunes;

	for ( unsigned i = 0; i < candidates.size(); i++ )
	{
		Engine::Rectangle<int> selectedArea = settlementAreas->getAreaById(candidates[i]);
		Engine::Rectangle<int> intersection;
		// MRJ: If the selected area is outside of the "homeBox" then why caring about it all?
		// ATM: above you will find settlementAreas->intersectionFilter method call. This ensures any
		// item inside candidates vector has intersection with homeBox. So the 'if' is not
		// needed, we just recompute the intersection to use it.
		homeBox.intersection(selectedArea, intersection);
		
		// Extract one random dune cell which is inside the homeRange and inside the selected area.
		
		// count dunes from candidate area "i", 'selectedArea' variable
		int countDunes = 0;
		int maxDist    = 0;
		Engine::Point2D<int> index;

		for (index._x = intersection._origin._x; index._x < intersection._origin._x+intersection._size._x; index._x++)			
		{
			for (index._y = intersection._origin._y; index._y < intersection._origin._y+intersection._size._y; index._y++)			
			{
				int d = ceil(agentPos.distance(index));
				if ((world->getValue("soils",index) == DUNE) 
				    && (d <= (double)agent.getHomeMobilityRange()))
				{
			  
					if ( d > maxDist )
					{
						maxDist = d;
						dunes.clear();
						countDunes = 1;
						dunes.push_back(index);
					}
					else if ( d == maxDist )
					{			  
						countDunes++;
						dunes.push_back(index);
					}
				}
			}
		}		
	}
	
	if ( dunes.empty() ) 
	{
		return NULL;
	}
	uint32_t diceSelectOneRandomDune = Engine::GeneralState::statistics().getUniformDistValue(0, dunes.size()-1);
	newHomeLocation = dunes[ diceSelectOneRandomDune ];
	moveHome = new MoveHomeAction( dunes[ diceSelectOneRandomDune ] );

	assert( moveHome != NULL );
	candidates.clear();

	return moveHome;
		


}

MDPAction*	HunterGathererDecisionTreeController::selectAction(	GujaratAgent & agent )
{
	HunterGatherer & agentConcrete = dynamic_cast<HunterGatherer&>( agent );
        //Decission Tree: DoNothing --> Forage --> MoveHome

	MDPAction* selectedAction = NULL;
	
	selectedAction = shouldDoNothing(agentConcrete);	
	selectedAction = (!selectedAction) ?  shouldForage(agentConcrete) : selectedAction;
	selectedAction = (!selectedAction) ?  shouldMoveHome(agentConcrete) : selectedAction;
	
	return selectedAction;
	
}

}
