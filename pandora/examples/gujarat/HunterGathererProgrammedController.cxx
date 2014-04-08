
#include <HunterGathererProgrammedController.hxx>

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

HunterGathererProgrammedController::HunterGathererProgrammedController()
{
//	_DoNothingDaysCovered = 1; 
}

HunterGathererProgrammedController::~HunterGathererProgrammedController()
{
}


void getSectors(HunterGatherer & agent, int & LRIdx, int & HRIdx)
{
	LRIdx = Engine::GeneralState::statistics().getUniformDistValue(0,agent.getLRSectors().size()-1);
	
	unsigned int i=0;
	int numS = agent.getHRSectors().size();
	while(i < numS && agent.getHRSectors()[i]->_direction != agent.getLRSectors()[LRIdx]->_direction)
	{
		++i;
	}
	
	HRIdx = i;
	assert(agent.getHRSectors()[HRIdx]->_direction == agent.getLRSectors()[LRIdx]->_direction);
}


MDPAction* HunterGathererProgrammedController::shouldForage( HunterGatherer & agent )
{		
	int LRIdx;
	int HRIdx;
	getSectors(agent, LRIdx, HRIdx);	
	return new ForageAction(agent.getHRSectors()[HRIdx],agent.getLRSectors()[LRIdx], false);	
}

MDPAction* HunterGathererProgrammedController::shouldMoveHome( HunterGatherer & agent )
{		
	int LRIdx;
	int HRIdx;
	getSectors(agent, LRIdx, HRIdx);	
	
	Sector * s = agent.getLRSectors()[LRIdx];
	int cellIdx = Engine::GeneralState::statistics().getUniformDistValue(0,s->numCells()-1);
	Engine::Point2D<int> newPosLR = *(s->cells()[cellIdx]);
	Engine::Point2D<int> newHome;
	GujaratWorld * gw = ((GujaratWorld*)agent.getWorld());
	gw->getHRFreeCell(newPosLR,newHome);
	
	Engine::Point2D<int> posLR;
	gw->worldCell2LowResCell(agent.getPosition(),posLR);
	
	int lowResHomeRange = agent.getLowResHomeRange();
	MoveHomeAction * mha = new MoveHomeAction( newHome
					, newPosLR	
					, agent.getHRSectors()[HRIdx]
					, agent.getLRSectors()[LRIdx]
					, false );
	
	return mha;	
}

void HunterGathererProgrammedController::selectActions( GujaratAgent & agent, std::list<Engine::Action*> & actions )
{		
	agent.updateKnowledge();
	
	HunterGatherer & agentConcrete = dynamic_cast<HunterGatherer&>( agent );
    
	int select = Engine::GeneralState::statistics().getUniformDistValue(0,1);
		
	MDPAction * selectedAction = 0;
	if(select)
	{
		selectedAction = shouldForage(agentConcrete);
	}
	else
	{
		selectedAction = shouldMoveHome(agentConcrete);	
	}
	
	assert(selectedAction!=0);	
	
	actions.push_back(selectedAction);
	
}

}

