
#include <MoveHomeAction.hxx>

#include <Agent.hxx>
#include <GujaratAgent.hxx>
#include <HunterGatherer.hxx>

#include <GujaratWorld.hxx>
#include <HunterGathererMDPState.hxx>
#include <algorithm>
#include <vector>

#include <Statistics.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <Sector.hxx>
#include <ForageAction.hxx>

#include <GujaratState.hxx>

namespace Gujarat
{


	
MoveHomeAction::MoveHomeAction( const Engine::Point2D<int>& p, const Engine::Point2D<int>& pLR, Sector * HRSectorToForage, Sector * LRSectorToForage, bool ownsSectorPointer ) 
: _newHomeLoc( p ), _newHomeLocLR(pLR), _forageAction(0)
{
	assert(LRSectorToForage->cells().size() > 0);
	
	_forageAction = new ForageAction(HRSectorToForage, LRSectorToForage, false);
	_forageAction->setFullPopulation(false);
}

MoveHomeAction::MoveHomeAction( const Engine::Point2D<int>& p, const Engine::Point2D<int>& pLR, ForageAction * forageAction ) 
: _newHomeLoc( p ), _newHomeLocLR(pLR), _forageAction(forageAction)
{
}

MoveHomeAction::~MoveHomeAction()
{
	if(_forageAction)
	{
		delete _forageAction;
	}
}

MDPAction * MoveHomeAction::copy() const
{
	MoveHomeAction * mha = new MoveHomeAction( _newHomeLoc, _newHomeLocLR,(ForageAction*)_forageAction->copy() );
	
	mha->_newHomeLocLR = _newHomeLocLR;
	
	return (mha);
}

std::string MoveHomeAction::describe() const
{
	std::stringstream logMove;
	logMove << "move_home( " << _newHomeLoc._x << ", " << _newHomeLoc._y << "|" << _newHomeLocLR._x << ", " << _newHomeLocLR._y << ")";
	return logMove.str();
}



void	MoveHomeAction::generatePossibleActions( const GujaratAgent & agent
												, const  std::vector< Sector* > & HRActionSectors
												, const std::vector< Sector* > & LRActionSectors
												, std::vector< MoveHomeAction* > & actions )
												
{
	generatePossibleActions( agent, agent.getPosition(), HRActionSectors, LRActionSectors, actions );
}

void MoveHomeAction::generatePossibleActions( const GujaratAgent & agent
				, const Engine::Point2D<int> & agentPos
				, const std::vector< Sector* > & HRActionSectors
				, const std::vector< Sector* > & LRActionSectors
				, std::vector< MoveHomeAction* > & actions )
{
	std::stringstream logName;
	logName << "agents_" << agent.getWorld()->getId() << "_" << agent.getId();

	
	//log_DEBUG(logName.str(), "generate possible actions for pos: " << agentPos);

	int scoreBestCell = 0;
	std::vector< Engine::Point2D<int>* > candidateCells;
	std::vector< Engine::Point2D<int>* > candidateCellsFromSector;
   
	const HunterGatherer & agentConcrete = dynamic_cast< const HunterGatherer & >( agent );
	GujaratWorld * gw = (GujaratWorld *)agentConcrete.getWorld();
	
	//log_INFO(logName.str(),"agent has many sectors:" << agentConcrete.getSectors().size());
   
	// Find candidate cells
	std::vector<int> chosenSects;	
	//for(std::vector<Sector *>::const_iterator it=agentConcrete.getLRSectors().begin(); it!=agentConcrete.getLRSectors().end(); it++)
	
	std::vector<bool> visitedSector(LRActionSectors.size(),false);
	
	for(unsigned int sectIdx=0;sectIdx < LRActionSectors.size(); sectIdx++)
	{
        //if(!(*it)->isEmpty())
		if(!(LRActionSectors[sectIdx]->isEmpty()))
		{
				const std::vector< Engine::Point2D<int>* > & sectCells = LRActionSectors[sectIdx]->cells();//(*it)->cells();	
				for(unsigned int cellsIdx = 0; cellsIdx < sectCells.size(); cellsIdx++)
				{
					int numDunes      = gw->getValueLR(LRCounterSoilDUNE,*sectCells[cellsIdx]);
					
					int resourcesCell = gw->getValueLR(eLRResources,*sectCells[cellsIdx]);
					//TODO test a new heuristic based on visitable cells
					//int resourcesCell = sectCells[cellsIdx]->getBiomassAmount();
					
					//int numInterDunes = gw->getValueLR(LRCounterSoilINTERDUNE, 
					//								   sectCells[cellsIdx]);
					//int visitable = agent.getAvailableTime() / agent.getForageTimeCost();
					
					int scoreCell = resourcesCell;
					
					if ( numDunes - gw->getValueLR(eLRPopulation,*sectCells[cellsIdx]) > 0 
						&& scoreBestCell <= scoreCell )
					{
						if ( scoreBestCell < scoreCell)
						{           
							scoreBestCell = scoreCell;
							candidateCellsFromSector.clear();	
							candidateCells.clear();
							chosenSects.clear();			candidateCellsFromSector.push_back(sectCells[cellsIdx]);
							visitedSector[sectIdx] = true;							
							//chosenSects.push_back(sectIdx);
						}
						else if (!visitedSector[sectIdx])
						{								candidateCellsFromSector.push_back(sectCells[cellsIdx]);
							visitedSector[sectIdx] = true;
							//chosenSects.push_back(sectIdx);
						}						
					}
				}
		}
    //sectIdx++;    
    
		for(std::vector< Engine::Point2D<int> * >::iterator it = candidateCellsFromSector.begin(); it != candidateCellsFromSector.end(); ++it)
		{
			candidateCells.push_back(*it);
			chosenSects.push_back(sectIdx);
			*it = 0;
		}
		candidateCellsFromSector.clear();
    
	}
    // Select candidate
   
	assert(!candidateCells.empty());   
   
	/*
	if(candidateCells.size() >1)
	{
		std::cout << " MoveHome choices : " << candidateCells.size() << std::endl;
		log_INFO(agent.getId()+"_controller",  "\tMoveHome choices=" << candidateCells.size());
	}*/
	
	if(!candidateCells.empty())
	{   
		int sectorIdx=0;
		for(std::vector< Engine::Point2D<int> * >::iterator it = candidateCells.begin(); it != candidateCells.end(); ++it)
		{
			Engine::Point2D<int> newPosLR = **it;
			Engine::Point2D<int> newHome;
			gw->getHRFreeCell(newPosLR,newHome);

//#ifdef REDUCC		
		
			Engine::Point2D<int> posLR;
			gw->worldCell2LowResCell(agentPos,posLR);
			int lowResHomeRange = ((GujaratConfig)((GujaratWorld*)agent.getWorld())->getConfig()).getLowResHomeRange();	
			/*int s = GujaratState::sectorsMask( newPos._x-posLR._x+lowResHomeRange, newPos._y-posLR._y+lowResHomeRange, GujaratState::getLRSectorsMask() );*/
		
			MoveHomeAction * mha = new MoveHomeAction( newHome
							, newPosLR
							, HRActionSectors[chosenSects[sectorIdx]]
							, LRActionSectors[chosenSects[sectorIdx]]
							, false );
			
			actions.push_back( mha );
			++sectorIdx;
		}
		
//#endif		

/*		
#ifndef REDUCC
		for( int i = 0; i < LRActionSectors.size(); i++)
		{           
			if(!LRActionSectors[i]->isEmpty())
			{
				MoveHomeAction * mha = new MoveHomeAction( newHome, HRActionSectors[i],LRActionSectors[i],false );
				mha->_newHomeLocLR = newPos;
				actions.push_back( mha );
			}
		}	
#endif
*/
	}
    
    for(unsigned int i = 0; i < candidateCells.size(); i++)
	{
		candidateCells[i] = (Engine::Point2D<int>*)0;
	}    
	candidateCells.clear();
	chosenSects.clear();
	
	//assert( actions.size()==1 );
	
	//log_DEBUG(logName.str(), "generate possible actions for pos: " << agentPos << " finished");
	//std::cout << "possible actions for MoveHome: " << actions.size() << std::endl;	
}

void MoveHomeAction::execute( Engine::Agent & agent )
{
	if(_forageAction)
	{
		_forageAction->execute(agent);
	}
	
	// Decrease population counter in old LR home cell
	Engine::Point2D<int> LRposOld;
	GujaratWorld * gw = (GujaratWorld *)agent.getWorld();
	gw->worldCell2LowResCell(agent.getPosition(),LRposOld);
	gw->setValueLR(eLRPopulation, LRposOld, gw->getValueLR(eLRPopulation,LRposOld) - 1);	
	// Change position
	agent.setPosition( _newHomeLoc );
	// Increase population counter in new LR home cell
	Engine::Point2D<int> LRpos;
	gw->worldCell2LowResCell(agent.getPosition(),LRpos);
	gw->setValueLR(eLRPopulation, LRpos, 1 + gw->getValueLR(eLRPopulation,LRpos));	
	
	GujaratAgent & gujaratAgent = (GujaratAgent&)agent;
	gujaratAgent.incrementMoveHomeActionsExecuted();
	
	// TODO
	// here? or at HunterGatherer::executeActions()?
	//gujaratAgent.clearSectorKnowledge(LRposOld,LRpos);
	gujaratAgent.clearSectorKnowledge();
	
	/*
	std::cout << "MSG> MOVE: "
	<< "t=" << agent.getWorld()->getCurrentTimeStep() << ","
	<< "pos=" << agent.getPosition() << ","
	<< std::endl;*/

}

void MoveHomeAction::executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const
{
	
	_forageAction->executeMDP(agent, s, sp);
	sp.setLocation( _newHomeLoc );
	//std::cout << "executeMDP- time index: " << s.getTimeIndex() << " MoveHomeAction from loc->resources: " << s.getLocation() << " -> " << s.getOnHandResources() << " to: " << sp.getLocation() << " -> " << sp.getOnHandResources() << " days starving: " << s.getDaysStarving() << std::endl;
	/*
	std::cout << "MSG> MDPMOVE: "
	<< "t=" << s.getTimeIndex() << ","
	<< "pos=" << s.getLocation() 
	<< std::endl;*/
}

int MoveHomeAction::getTimeNeeded() const
{
	return 1;//issue: set it in configuration file
}

int MoveHomeAction::getPotentialBiomass() const
{
	if(!_forageAction)
	{
		return 0;
	}
	return _forageAction->getLRSector().getBiomassAmount();
}

Sector * MoveHomeAction::getVisitedSector() 
{ 
	return _forageAction->getVisitedSector(); 	
}


} // namespace Gujarat

