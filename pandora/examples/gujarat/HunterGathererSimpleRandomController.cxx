
#include <HunterGathererSimpleRandomController.hxx>

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

#include <GujaratWorld.hxx>

namespace Gujarat
{

HunterGathererSimpleRandomController::HunterGathererSimpleRandomController()
{
//	_DoNothingDaysCovered = 1; 
}

HunterGathererSimpleRandomController::~HunterGathererSimpleRandomController()
{
}

int HunterGathererSimpleRandomController::getSectorIdxMatchingDirection( const HunterGatherer & agent, const std::vector<Sector *> & sectors, const int direction  ) const
{
	int i = 0;
	while(i < sectors.size())
	{
		if (sectors[i]->_direction == direction)
		{
			return i;
		}
		i++;
	}
	return -1;
}


void HunterGathererSimpleRandomController::selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions )
{	
	HunterGatherer & agentConcrete = dynamic_cast<HunterGatherer&>( agent );
        //Decission Tree: DoNothing --> Forage --> MoveHome

	MDPAction * selectedAction;
	int LRSectorIdx = Engine::GeneralState::statistics().getUniformDistValue(0,agentConcrete.getLRSectors().size()-1);
	int HRSectorIdx = getSectorIdxMatchingDirection( agentConcrete, agentConcrete.getHRSectors(), agentConcrete.getLRSectors()[LRSectorIdx]->_direction );
		
	assert(agentConcrete.getHRSectors()[HRSectorIdx]->_direction == agentConcrete.getLRSectors()[LRSectorIdx]->_direction);

	if (Engine::GeneralState::statistics().getUniformDistValue(0,1))
	{
		selectedAction = new ForageAction(agentConcrete.getHRSectors()[HRSectorIdx],agentConcrete.getLRSectors()[LRSectorIdx], false);
	}
	else
	{
		const std::vector< Engine::Point2D<int> * > & cellsLR = agentConcrete.getLRSectors()[LRSectorIdx]->cells();
		int idxCellLR = Engine::GeneralState::statistics().getUniformDistValue(0,cellsLR.size()-1);
		Engine::Point2D<int> newPosLR = *(cellsLR[idxCellLR]);
		Engine::Point2D<int> newHome;
		((GujaratWorld*)agentConcrete.getWorld())->getHRFreeCell(newPosLR,newHome);
		
		selectedAction = new MoveHomeAction( newHome
										, newPosLR
										, agentConcrete.getHRSectors()[HRSectorIdx]
										, agentConcrete.getLRSectors()[LRSectorIdx]
										, false );
	}	
	
	actions.push_back(selectedAction);
	
}

}

