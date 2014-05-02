
#include <HunterGathererDecisionTreeController.hxx>

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
{}

HunterGathererDecisionTreeController::~HunterGathererDecisionTreeController()
{}

int HunterGathererDecisionTreeController::getSectorIdxMatchingDirection( const HunterGatherer & agent, const std::vector<Sector *> & sectors, const int direction  ) const
{
	for(unsigned i = 0; i < sectors.size(); ++i) {
		if (sectors[i]->_direction == direction) {
			return i;
		}
	}
	return -1;
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
	int maxBiomass = -1;
	for ( unsigned int i = 0; i < agent.getLRSectors().size(); i++ )
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


MDPAction* HunterGathererDecisionTreeController::shouldForage( HunterGatherer & agent )
{	
	//Sector * maxSector = getMaxBiomassSector(agent);
	int maxSectorLRIdx = getMaxBiomassSector(agent);
	if(maxSectorLRIdx < 0) return 0;
	
	int maxSectorHRIdx = getSectorIdxMatchingDirection( agent, agent.getHRSectors(), agent.getLRSectors()[maxSectorLRIdx]->_direction );
	assert(agent.getHRSectors()[maxSectorHRIdx]->_direction == agent.getLRSectors()[maxSectorLRIdx]->_direction);
	
	ForageAction * f = new ForageAction(agent.getHRSectors()[maxSectorHRIdx],agent.getLRSectors()[maxSectorLRIdx], false);
	
	double  maxDist= agent.computeMaxForagingDistance(true); // we are not in a half-forage
	unsigned long sum = 0;
	for(unsigned i = 0; i < BIOMASS_ESTIMATION_ITERATIONS; ++i) {
		sum += f->doWalkForRewardEstimation(agent, agent.getPosition(), maxDist, agent.getWorld()->getDynamicRaster(eResources));
	}
	float meanReward = agent.convertBiomassToCalories( sum / (float) BIOMASS_ESTIMATION_ITERATIONS );	
	
	float alpha = ((GujaratConfig)((GujaratWorld*)agent.getWorld())->getConfig()).getAlphaDecTree();
	if(  alpha*meanReward >= agent.computeConsumedResources(1) ) {
		return f;
	}
	
	delete f;
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
	MoveHomeAction::generatePossibleActions( agent, agent.getHRSectors(), agent.getLRSectors(), possibleActions );
	
	// generatePossibleAction generates potential MoveHome actions ordered by the score of potential SettlementAreas
	// thus, the first possible action with the best potential biomass is the optimal move
	MDPAction * chosenAction = 0; 
	// LEAK arranged
	//std::random_shuffle(possibleActions.begin(), possibleActions.end());
	
	int idx = Engine::GeneralState::statistics().getUniformDistValue(0,possibleActions.size()-1);
	chosenAction = possibleActions.at(idx);
	possibleActions[idx] = 0;
	for(unsigned int i=0; i<possibleActions.size(); i++)
	{
		delete possibleActions.at(i);
	}		
	possibleActions.clear();
	return chosenAction;
}

void HunterGathererDecisionTreeController::selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions )
{	
	HunterGatherer & agentConcrete = dynamic_cast<HunterGatherer&>( agent );

	MDPAction * selectedAction = shouldForage(agentConcrete);
	
	if(selectedAction!=0)
	{
		actions.push_back(selectedAction);
		return;
	}
	selectedAction = shouldMoveHome(agentConcrete);
	if(selectedAction!=0)
	{
		actions.push_back(selectedAction);
	}
}

}

