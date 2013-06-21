
#include <HunterGathererMDPModel.hxx>
#include <HunterGatherer.hxx>
#include <HunterGathererMDPConfig.hxx>
#include <GujaratWorld.hxx>
#include <ForageAction.hxx>
#include <MoveHomeAction.hxx>
#include <DoNothingAction.hxx>
#include <Exceptions.hxx>
#include <typeinfo>

#include <sstream>
#include <Logger.hxx>

using Problem::action_t;

namespace Gujarat
{

HunterGathererMDPModel::HunterGathererMDPModel()
	: _simAgent(0), _initial( NULL )
{
}

HunterGathererMDPModel::~HunterGathererMDPModel()
{	
	if ( _initial != NULL )
	{
		delete _initial;
	}
}

void	HunterGathererMDPModel::setup( const HunterGathererMDPConfig& cfg )
{
	_config = cfg;
	setHorizon( cfg.getHorizon() );
}

void	HunterGathererMDPModel::reset( GujaratAgent & agent )
{
	std::stringstream logName;
	logName << "infoshar";
	
	if ( _initial != NULL )
		delete _initial;

	_simAgent = dynamic_cast<HunterGatherer *>(&agent);

	// Copy of the Sector & Pool structures.
	// If we pass ones owned by the agent, even if we protect them
	// from being altered in cell contents, the utility shall be
	// modified. So, to preserve the same state before and after
	// reasoning a duplication is needed.
	// Only altered utility : LRSectors
	// HRSectors, HRCellPool, LRCellPool, shall not be altered.
	// A state associated to a MoveHomeAction shall see its sector
	// structures created with new items
	
	// The main matter : do not alter Sector's utility -->
	// --> Sector duplication
	std::vector< bool > ownsItems(4);
	ownsItems[0]=false;
	ownsItems[1]=true;
	ownsItems[2]=false;
	ownsItems[3]=false;
	const std::vector< Sector* > & sourceLRSectors = agentRef().getLRSectors();
	std::vector< Sector* > * LRActionSectors = 
			new std::vector< Sector* >(sourceLRSectors.size());
	
	std::vector< Sector* >::const_iterator it = sourceLRSectors.begin();
	int i = 0;
	while(it!=sourceLRSectors.end())
	{
		Sector * s = (Sector*)*it;
		Sector * r = new Sector(s);
		//*?
		r->setMetaInfo(1);
		// Shallow Copy;
		(*LRActionSectors)[i++] = r;
		it++;
	}
	// We have copied the Sectors, but still referencing the same LR cells!!!
	// that's good, reusing Point2D pool	
	
	// Build initial state from current state in the simulation
	
	//log_INFO(logName.str(),"CREA INITIAL");
	
	//assert(agentRef().getHRSectorsNoConst().size()>0 && agentRef().getHRCellPoolNoConst().size()>0);
	
	//*?
	//TODO
	// passing as paramater this : &agentRef().getHRSectorsNoConst()
	// could be dangerous	
	
	_initial = new HunterGathererMDPState(	agentRef().getPosition()
											, agentRef().getOnHandResources()
											, agentRef().getLRResourcesRaster()
											, _config.getHorizon()
											, agentRef().computeConsumedResources(1)
					
											, &agentRef().getHRSectorsNoConst()
											, LRActionSectors 
											, &agentRef().getHRCellPoolNoConst()
											, &agentRef().getLRCellPoolNoConst()
											, ownsItems);
											
	//std::cout << "creat MDPState:" << _initial->_dni << std::endl;
	
	
	//TODO refactor it, instead of passing HR and LR structures pass a HGMind
	
	//log_INFO(logName.str(),"MAKE ACTIONS INITIAL");
	
	makeActionsForState( *_initial );
	
	//log_INFO(logName.str(),"MAKE ACTIONS INITIAL AFTER");
	
	
	//std::cout << "Initial state: " << *_initial << std::endl;	
}

action_t	HunterGathererMDPModel::number_actions( const HunterGathererMDPState& s ) const
{
	return s.numAvailableActions();
}

const HunterGathererMDPState& HunterGathererMDPModel::init() const
{
	return *_initial;
}

bool HunterGathererMDPModel::terminal( const HunterGathererMDPState& s ) const
{
	return s.getTimeIndex() == getHorizon();
}

bool HunterGathererMDPModel::applicable( const HunterGathererMDPState& s,
						action_t a ) const
{
	return true;
}

float HunterGathererMDPModel::cost( const HunterGathererMDPState& s,
					action_t a ) const
{
	// TODO XRC: what is that 10??
	//float cost = s.getDaysStarving()*10;
	float cost = s.getDaysStarving()*10.0f + s.availableActions(a)->getTimeNeeded();
	//float cost = 1000000 - s.getOnHandResources();//s.availableActions(a)->getTimeNeeded();
	//std::cout << "cost for action: " << s.availableActions(a)->describe() << " is: " << cost << " days starving: " << s.getDaysStarving() << " x10: " << s.getDaysStarving()*10.0f << " get time needed: " << s.availableActions(a)->getTimeNeeded() << std::endl;
	return cost;
}


void HunterGathererMDPModel::next( 	const HunterGathererMDPState &s, 
					action_t a, 
					OutcomeVector& outcomes ) const
{
	std::stringstream logName;
	logName << "infoshar";
	
	
	const MDPAction* act = s.availableActions(a);
	/*
	 * Here, where I know whether "act" is ForageAction or MoveHomeAction,
	 * according to these, what I pass to sp depends on it... copies,refs,
	 * or new adhoc creation... etc...
	 */
	
	std::vector<bool> ownership(4);
	//TODO An action should know nothing about ownerships. Refactor the thing.
	
	std::vector< Sector* > * HRActionSectors;
	std::vector< Sector* > * LRActionSectors;
	std::vector< Engine::Point2D<int> > * HRCellPool;
	std::vector< Engine::Point2D<int> > * LRCellPool;
	
	if(dynamic_cast<const MoveHomeAction*>(act))
	{
		// Move implies a new set of cells around the home.
		// New containers are created to be filled by updateKnowledge(...) const
		// HR information remains untouched, not used along MDP
		HRActionSectors = s.getHRActionSectors();	
		//HRActionSectors = new std::vector< Sector* >(0);
		LRActionSectors = new std::vector< Sector* >(0);
		HRCellPool = s.getHRCellPool();
		//HRCellPool = new std::vector< Engine::Point2D<int> >;
		LRCellPool = new std::vector< Engine::Point2D<int> >(0);
		ownership[0]=false;
		ownership[1]=true;
		ownership[2]=false;
		ownership[3]=true;
		
		assert(HRActionSectors->size()>0 && HRCellPool->size()>0);
	}	
	else if(dynamic_cast<const ForageAction*>(act))
	{
		// New Sectors are created to preserve utility markers of parent state.
		// But the same cell pools are used.
		const std::vector< Sector* > & sourceLRSectors = agentRef().getLRSectors();
		LRActionSectors = new std::vector< Sector* >(sourceLRSectors.size());
		
		/*std::cout << "source has " ;
		for(int i=0; i <sourceLRSectors.size(); i++)
			std::cout << " " << sourceLRSectors[i]->_dni;
		
		std::cout << std::endl;*/
		
		
		std::vector< Sector* >::const_iterator it = sourceLRSectors.begin();
		int i = 0;
		while(it!=sourceLRSectors.end())
		{
			Sector * se = (Sector*)*it;
			Sector * r = new Sector(se);
			r->setMetaInfo(2);
			// Shallow Copy;
			(*LRActionSectors)[i] = r;
			i++;
			it++;
		}
		
		HRActionSectors = s.getHRActionSectors();
		HRCellPool = s.getHRCellPool();
		LRCellPool = s.getLRCellPool();
		
		ownership[0]=false;
		ownership[1]=true;
		ownership[2]=false;
		ownership[3]=s.getOwnerShip()[3];
		
		assert(HRActionSectors->size()>0 && HRCellPool->size()>0);
		
		/*std::cout << "AFTER COPY source has " ;
		for(int i=0; i <sourceLRSectors.size(); i++)
			std::cout << " " << sourceLRSectors[i]->_dni;
		
		std::cout << std::endl;*/
		
		
		
	}	
	else if(dynamic_cast<const DoNothingAction*>(act))
	{
		HRActionSectors = s.getHRActionSectors();
		LRActionSectors = s.getLRActionSectors();
		HRCellPool = s.getHRCellPool();
		LRCellPool = s.getLRCellPool();
		
		//ownership = s.getOwnerShip();
		ownership[0]=false;
		ownership[1]=s.getOwnerShip()[1];
		ownership[2]=false;
		ownership[3]=s.getOwnerShip()[3];
		
		assert(HRActionSectors->size()>0 && HRCellPool->size()>0);
	}
	else{
		/* Should be left this case to a default initialization of sectors and pools?
		 * It could produce a backdoor effect that would lead to errors when a new
		 * action is added to the model.
		 */		
		
		std::stringstream oss;
		oss << "HunterGathererMDPModel::next() - Action is of not recognized type.";
		throw Engine::Exception(oss.str());
	}
		
	
	//s.initializeSuccessor(sp,ownership);
	HunterGathererMDPState sp(s, HRActionSectors, LRActionSectors, HRCellPool, LRCellPool, ownership);
	
	//std::cout << "creat MDPState:" << sp._dni << std::endl;
	
	//assert(LRActionSectors->size() > 0);
	
	/*for(int i=0;i<s.getLRActionSectors()->size();i++)
	{
		if((*s.getLRActionSectors())[i]->cells().size() == 0)
		{
			std::cout << "FOUND SECTOR ZERO CELLS, sector:" << (*s.getLRActionSectors())[i]->_dni << " state:" << s._dni << std::endl;
		}
		assert((*s.getLRActionSectors())[i]->cells().size() >0);
	}*/
	
	act->executeMDP( agentRef(), s, sp );
	applyFrameEffects( s, sp );
	sp.computeHash();	

	makeActionsForState( sp );
	
	outcomes.push_back( std::make_pair(sp, 1.0) );
}

void	HunterGathererMDPModel::applyFrameEffects( const HunterGathererMDPState& s,  HunterGathererMDPState& sp ) const
{
	sp.consume();
	//sp.spoilage( agentRef().getSurplusSpoilageFactor() );
	sp.increaseTimeIndex();
}

void	HunterGathererMDPModel::makeActionsForState( HunterGathererMDPState& s ) const
{
	// Map from "sector memory address" to "sector integer identifier".
	// After sorting validActionSectors I need to access both the HR and the LR sector
	std::map<Sector*,int> sectorIdxMap;

	//std::cout << "creating actions for state with time index: " << s.getTimeIndex() << " and resources: " << s.getOnHandResources() << std::endl;
	assert( s.numAvailableActions() == 0 );
	// Make Do Nothing
	//if ( _config.isDoNothingAllowed() )
	//	s.addAction( new DoNothingAction() );	
	
	std::vector< Sector* > validActionSectors;
	// Low Resolution
	std::vector< Sector* > * LRActionSectors = s.getLRActionSectors();
	// High Resolution
	std::vector< Sector* > * HRActionSectors = s.getHRActionSectors();	
	// It is not needed to recalculate each time the HR cells per sector.
	std::vector< Engine::Point2D<int> > * HRCellPool = s.getHRCellPool();
	std::vector< Engine::Point2D<int> > * LRCellPool = s.getLRCellPool();	
	
	//for(int i = 0; i < LRActionSectors.size(); i++)
		//assert(LRActionSectors[i]->cells().size() >0);
	
	
	//TODO watch HRSectors update : BOTTLENECK
	agentRef().updateKnowledge( s.getLocation(), s.getResourcesRaster(), *HRActionSectors, *LRActionSectors, *HRCellPool, *LRCellPool );
	
	// MRJ: Remove empty sectors if any
	for ( unsigned i = 0; i < LRActionSectors->size(); i++ )
	{
		if ( (*LRActionSectors)[i]->isEmpty() )
		{
			// You can't do that if you do not own it.
			// Any delete is postponed at the end of lifecycle of the MDPState
			
			// delete (*LRActionSectors)[i];
			// delete (*HRActionSectors)[i];
			continue;
		}
		validActionSectors.push_back( (*LRActionSectors)[i] );
		sectorIdxMap[(*LRActionSectors)[i]] = i;
	}	
	//TODO why 2 reorderings??? first random, then according a predicate
	//std::random_shuffle( validActionSectors.begin(), validActionSectors.end() );
	std::sort( validActionSectors.begin(), validActionSectors.end(), SectorBestFirstSortPtrVecPredicate() );
	
	// Make Forage actions
	
	int forageActions = _config.getNumberForageActions();
	if ( forageActions >= validActionSectors.size() )
	{
		for ( unsigned i = 0; i < validActionSectors.size(); i++ )
		{
			int sectorIdx = sectorIdxMap[validActionSectors[i]];
			//s.addAction( new ForageAction( HRActionSectors[sectorIdx], validActionSectors[i], true ) );
			s.addAction( new ForageAction( (*HRActionSectors)[sectorIdx], validActionSectors[i], false ) );	
		}
	}
	else
	{
		for ( unsigned i = 0; i < forageActions; i++ )
			{
			int sectorIdx = sectorIdxMap[validActionSectors[i]];
			//s.addAction( new ForageAction( HRActionSectors[sectorIdx],validActionSectors[i], true ) );
			s.addAction( new ForageAction( (*HRActionSectors)[sectorIdx],validActionSectors[i], false ) );
			}
		for ( unsigned i = forageActions; i < validActionSectors.size(); i++ )
			{
			//int sectorIdx = sectorIdxMap[validActionSectors[i]];
			
			// Structures from a MDPState cannot be destroyed till the State disappears
			// delete validActionSectors[i];
			// delete HRActionSectors[sectorIdx];
			// delete LRActionSectors[sectorIdx]; redundancy because validActionSectors[i]==LRActionSectors[sectorIdx]
			}
	}
	//std::cout << "number of valid forage actions: " << s.numAvailableActions() << " for number of valid sectors: " << validActionSectors.size() << std::endl;

	// Make Move Home
	std::vector< MoveHomeAction* > possibleMoveHomeActions;
	MoveHomeAction::generatePossibleActions( agentRef(), s.getLocation(), *HRActionSectors, validActionSectors, possibleMoveHomeActions );
	int moveHomeActions =  _config.getNumberMoveHomeActions();
	if (  moveHomeActions >=  possibleMoveHomeActions.size() )
	{
		for ( unsigned i = 0; i < possibleMoveHomeActions.size(); i++ )
			s.addAction( possibleMoveHomeActions[i] );
	}
	else
	{
		for ( unsigned i = 0; i <  moveHomeActions; i++ )
			s.addAction( possibleMoveHomeActions[i] );
		for ( unsigned i =  moveHomeActions; i < possibleMoveHomeActions.size(); i++ )
			delete possibleMoveHomeActions[i];
	}
	assert( s.numAvailableActions() > 0 );
	sectorIdxMap.clear();
	possibleMoveHomeActions.clear();
	validActionSectors.clear();
	
	// Reference to structures that could reference structures from a MDPState cannot be destroyed. The MDPState's will destroy the ones created and are owned.	
	//HRActionSectors.clear();
	//LRActionSectors.clear();
	//std::cout << "finished creating actions for state with time index: " << s.getTimeIndex() << " and resources: " << s.getOnHandResources() << std::endl;
} 

}
