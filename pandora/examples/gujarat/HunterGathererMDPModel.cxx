
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
	//*?
	//setWidth( cfg.getWidth() );
}

void	HunterGathererMDPModel::reset( GujaratAgent & agent )
{
	std::stringstream logName;
	logName << "HGMDPModel";
	
	if ( _initial != NULL )
	{
		_initial->_info = -_initial->_info;
		delete _initial;		
	}

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
	std::vector< Sector* > * HRActionSectors = &(_simAgent->getHRSectorsNoConst());
	std::vector< Sector* > * LRActionSectors = new std::vector< Sector* >(0);
	//&(_simAgent->getLRSectorsNoConst());//new std::vector< Sector* >(0);
	std::vector< Engine::Point2D<int> > * HRCellPool = &(_simAgent->getHRCellPoolNoConst());
	std::vector< Engine::Point2D<int> > * LRCellPool = new std::vector< Engine::Point2D<int> >(0);
	//&(_simAgent->getLRCellPoolNoConst());//new std::vector< Engine::Point2D<int> >(0);

	
	std::vector< bool > ownsItems(4);
	ownsItems[0]=false;	
	ownsItems[1]=false;
	ownsItems[2]=false;	
	ownsItems[3]=false;		
		
	// Build initial state from current state in the simulation
	
	omp_lock_t * mapLock = _simAgent->getMapLock();
	//new omp_lock_t();
	//omp_init_lock(mapLock);

	std::vector<MDPAction *>  actionList;	

	makeActionsForState(_simAgent->getLRResourcesRaster()      
				, _simAgent->getPosition()
				, HRActionSectors
				, LRActionSectors
				, HRCellPool
				, LRCellPool
				, actionList);	
	
	_initial = new HunterGathererMDPState( _simAgent
						, &_config
						, _simAgent->getPosition()
						, _simAgent->getOnHandResources()
						, _simAgent->getLRResourcesRaster() //*? incremental?
						, _config.getHorizon()
						, _simAgent->computeConsumedResources(1)
						, HRActionSectors
						, LRActionSectors 
						, HRCellPool
						, LRCellPool
						, ownsItems
						, _simAgent->getObjectUseCounter()
						, mapLock
						, actionList);
	
	_initial->computeHash();
	
	//*?
	_initial->_creator = -_initial->_creator;
}

action_t HunterGathererMDPModel::number_actions( const HunterGathererMDPState& s ) const
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
					OutcomeVector& outcomes,int foo ) const
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
	
	HRActionSectors = s.getHRActionSectors();
	LRActionSectors = s.getLRActionSectors();
	HRCellPool = s.getHRCellPool();
	LRCellPool = s.getLRCellPool();		
	
	ownership[0]=false;	
	ownership[1]=false;
	ownership[2]=false;	
	ownership[3]=false;		
	
	std::vector<MDPAction *>  actionList;

	Engine::Point2D<int> center;
	if(dynamic_cast<const MoveHomeAction*>(act))
	{
		center = ((MoveHomeAction*)act)->getNewHomeLoc();
	}
	else
	{
		center = s.getLocation();
	}
	
	
	makeActionsForState(s, center,HRActionSectors, LRActionSectors, HRCellPool, LRCellPool, actionList);
	
	//s.initializeSuccessor(sp,ownership);
	HunterGathererMDPState sp(s, center,HRActionSectors, LRActionSectors, HRCellPool, LRCellPool, ownership, actionList);
	
	act->executeMDP( agentRef(), s, sp );
	applyFrameEffects( s, sp );
	sp.computeHash();	
	
	outcomes.push_back( std::make_pair(sp, 1.0) ); //*? TODO why 1.0, justify it
	
}


void HunterGathererMDPModel::next( 	const HunterGathererMDPState &s, 
					action_t a, 
					OutcomeVector& outcomes
					 ) const
{
	std::stringstream logName;
	logName << "HGMDPModel";
	
	
	const MDPAction* act = s.availableActions(a);
	/*
	 * Here, where I know whether "act" is ForageAction or MoveHomeAction,
	 * according to these, what I pass to sp depends on it... copies,refs,
	 * or new adhoc creation... etc...
	 */
	
	Engine::Point2D<int> center(s.getLocation());	
	
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
		LRActionSectors = new std::vector< Sector* >(0);		
		HRCellPool = s.getHRCellPool();		
		LRCellPool = new std::vector< Engine::Point2D<int> >(0);	
		ownership[0]=false;	
		ownership[1]=true;
		ownership[2]=false;	
		ownership[3]=true;		
	
		center = ((MoveHomeAction*)act)->getNewHomeLoc();
		
	}	
	else if(dynamic_cast<const ForageAction*>(act))
	{
		
		HRActionSectors = s.getHRActionSectors();	
		LRActionSectors = new std::vector< Sector* >(0);		
		HRCellPool = s.getHRCellPool();		
		LRCellPool = new std::vector< Engine::Point2D<int> >(0);	
		ownership[0]=false;	
		ownership[1]=true;
		ownership[2]=false;	
		ownership[3]=true;	
		

	}
	/*
	else if(dynamic_cast<const ForageAction*>(act))
	{
		// New Sectors are created to preserve utility markers of parent state.
		// But the same cell pools are used.
		const std::vector< Sector* > & sourceLRSectors = agentRef().getLRSectors();
		LRActionSectors = new std::vector< Sector* >(sourceLRSectors.size());

		for(unsigned int i=0; i<sourceLRSectors.size(); i++)
		{
			Sector * se = sourceLRSectors[i];
			Sector * r = new Sector(se);
			(*LRActionSectors)[i] = r;
		}
		
		HRActionSectors = s.getHRActionSectors();
		HRCellPool = s.getHRCellPool();
		LRCellPool = s.getLRCellPool();
		
		ownership[0]=false;
		ownership[1]=true;
		ownership[2]=false;
		ownership[3]=s.getOwnerShip()[3];		
		

	}	*/
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
		
	}
	else
	{
		/* Should be left this case to a default initialization of sectors and pools?
		 * It could produce a backdoor effect that would lead to errors when a new
		 * action is added to the model.
		 */		
		
		std::stringstream oss;
		oss << "HunterGathererMDPModel::next() - Action is of not recognized type.";
		throw Engine::Exception(oss.str());
	}
		
	
	std::vector<MDPAction *>  actionList;

	makeActionsForState(s, center, HRActionSectors, LRActionSectors, HRCellPool, LRCellPool, actionList);
	
	HunterGathererMDPState sp(s, center, HRActionSectors, LRActionSectors, HRCellPool, LRCellPool, ownership, actionList);
	
	/*
	std::cout << "NET: edge "		
		<< s._dni 
		<< " "		
		<< sp._dni
		<< " n"
		<< std::endl;
	*/
	//assert(LRActionSectors->size() > 0);
	
	act->executeMDP( agentRef(), s, sp );
	applyFrameEffects( s, sp );
	sp.computeHash();	
	
	outcomes.push_back( std::make_pair(sp, 1.0) );
}

void	HunterGathererMDPModel::applyFrameEffects( const HunterGathererMDPState& s,  HunterGathererMDPState& sp ) const
{
	sp.consume();
	//sp.spoilage( agentRef().getSurplusSpoilageFactor() );
	sp.increaseTimeIndex();
}

/**
 * @param actionList List of executable actions
 *//*
void HunterGathererMDPModel::makeActionsForState( HunterGatherer& parent, const Engine::Point2D<int> &loc, std::vector<MDPAction *>&  actionList) const
{
	makeActionsForState(
			     parent.getLRResourcesRaster(),
			     loc,
			     &parent.getHRSectorsNoConst(),
			     &parent.getLRSectorsNoConst(),
			     &parent.getHRCellPoolNoConst(),
			     &parent.getLRCellPoolNoConst(),
			     actionList);
}*/

/**
 * @param actionList List of executable actions
 */
void HunterGathererMDPModel::makeActionsForState( 
				const HunterGathererMDPState& parent
				, const Engine::Point2D<int> &loc
				, std::vector< Sector* > * HRActionSectors
				, std::vector< Sector* > * LRActionSectors
				, std::vector< Engine::Point2D<int> > * HRCellPool
				, std::vector< Engine::Point2D<int> > * LRCellPool
				, std::vector<MDPAction *>&  actionList) const
			     
{
	makeActionsForState( 
			     parent.getResourcesRaster(),
			     loc,
			     HRActionSectors,
			     LRActionSectors,
			     HRCellPool,
			     LRCellPool,
			     actionList);
} 



/**
 * @param actionList List of executable actions
 */
void HunterGathererMDPModel::makeActionsForState(
			      const Engine::IncrementalRaster & resourcesRaster			      
			      , const Engine::Point2D<int> &position
			      , std::vector< Sector* >* HRActionSectors
			      , std::vector< Sector* >* LRActionSectors
			      , std::vector< Engine::Point2D<int> >* HRCellPool
			      , std::vector< Engine::Point2D<int> >* LRCellPool
			      , std::vector<MDPAction *>&  actionList) const
{
	std::stringstream logName;
	logName << "logMDPStates_"	<< _simAgent->getWorld()->getId() << "_" << _simAgent->getId();
	
	
	// Map from "sector memory address" to "sector integer identifier".
	// After sorting validActionSectors I need to access both the HR and the LR sector
	std::map<unsigned long,int> sectorIdxMap;

	//std::cout << "creating actions for state with time index: " << s.getTimeIndex() << " and resources: " << s.getOnHandResources() << std::endl;
	// Make Do Nothing	
	if ( _config.isDoNothingAllowed() )
		actionList.push_back( new DoNothingAction() );	

	std::vector< Sector* > validActionSectors;

	//for(int i = 0; i < LRActionSectors.size(); i++)
		//assert(LRActionSectors[i]->cells().size() >0);
	
	/*if(LRActionSectors->size()==0)
	{
		log_INFO(logName.str(),"PREUPDATE LRSectors at " << position << ", amount sectors " << LRActionSectors->size());	
	}
	else
	{
		log_INFO(logName.str(),"PREUPDATE LRSectors at " << position << ", amount sectors " << LRActionSectors->size());	
	}*/
		
	agentRef().updateKnowledge( position, resourcesRaster, HRActionSectors, LRActionSectors, HRCellPool, LRCellPool );
	
	
	//log_INFO(logName.str(),"UPDATED LRSectors at " << position << ", amount sectors " << LRActionSectors->size());	
	
	// MRJ: Remove empty sectors if any
	for ( unsigned i = 0; i < LRActionSectors->size(); i++ )
	{
		if ( (*LRActionSectors)[i]->isEmpty() )
		{
			continue;
		}
		validActionSectors.push_back( (*LRActionSectors)[i] );
		sectorIdxMap[(unsigned long)(*LRActionSectors)[i]] = i;
	}	
	std::sort( validActionSectors.begin(), validActionSectors.end(), SectorBestFirstSortPtrVecPredicate() );
	
	// Make Forage actions
	for ( unsigned i = 0; i < validActionSectors.size(); i++ )
	{
		int sectorIdx = sectorIdxMap[(unsigned long)(validActionSectors[i])];
		actionList.push_back( new ForageAction( (*HRActionSectors)[sectorIdx], validActionSectors[i], false ) );	
	}
	
	// Make Move Home
	std::vector< MoveHomeAction* > possibleMoveHomeActions;
	MoveHomeAction::generatePossibleActions( agentRef(), position, *HRActionSectors, validActionSectors, possibleMoveHomeActions );
	unsigned int moveHomeActions = _config.getNumberMoveHomeActions();
	if (  moveHomeActions >=  possibleMoveHomeActions.size() )
	{
		for ( unsigned i = 0; i < possibleMoveHomeActions.size(); i++ )
			actionList.push_back( possibleMoveHomeActions[i] );
	}
	else
	{
		for ( unsigned i = 0; i <  moveHomeActions; i++ )
			actionList.push_back( possibleMoveHomeActions[i] );
		for ( unsigned i =  moveHomeActions; i < possibleMoveHomeActions.size(); i++ )
			delete possibleMoveHomeActions[i];
	}
	
	assert( actionList.size() > 0 );
	sectorIdxMap.clear();
	possibleMoveHomeActions.clear();
	validActionSectors.clear();
	
	// Reference to structures that could reference structures from a MDPState cannot be destroyed. The MDPState's will destroy the ones created and are owned.	
	//HRActionSectors.clear();
	//LRActionSectors.clear();
	//std::cout << "finished creating actions for state with time index: " << parent.getTimeIndex() << " and resources: " << parent.getOnHandResources() << std::endl;
	
	/*log_INFO(logName.str(),"CREA LRSectors at " << position 
						<< ", amount sectors " << LRActionSectors->size()
						<< ", amount actions " << actionList.size()
				);	*/
	
} 


  
}
