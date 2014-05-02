
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

HunterGathererMDPModel::HunterGathererMDPModel(const HunterGathererMDPConfig& cfg)
	: _agent(0), _initial( NULL ), _config(cfg)
{}

HunterGathererMDPModel::~HunterGathererMDPModel()
{	
	if ( _initial != NULL )
	{
		delete _initial;
	}
}

void	HunterGathererMDPModel::reset( GujaratAgent & agent )
{
	delete _initial;

	_agent = dynamic_cast<HunterGatherer *>(&agent);

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
	std::vector< Sector* > * HRActionSectors = &(_agent->getHRSectorsNoConst());
	std::vector< Sector* > * LRActionSectors = new std::vector< Sector* >(0);
	//&(_agent->getLRSectorsNoConst());//new std::vector< Sector* >(0);
	std::vector< Engine::Point2D<int> > * HRCellPool = &(_agent->getHRCellPoolNoConst());
	std::vector< Engine::Point2D<int> > * LRCellPool = new std::vector< Engine::Point2D<int> >(0);
	//&(_agent->getLRCellPoolNoConst());//new std::vector< Engine::Point2D<int> >(0);

	// Build initial state from current state in the simulation
	_initial = new HunterGathererMDPState(_agent, _config, HRActionSectors, LRActionSectors , HRCellPool, LRCellPool);
	_initial->computeHash();
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
	return s.getTimeIndex() == (unsigned) _config.getHorizon();
}

bool HunterGathererMDPModel::applicable( const HunterGathererMDPState& s, action_t a ) const
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


void HunterGathererMDPModel::next( 	const HunterGathererMDPState &s,  action_t a, OutcomeVector& outcomes) const
{
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
	else {
		throw Engine::Exception("Unrecognized action type");
	}

	HunterGathererMDPState sp(s, center, HRActionSectors, LRActionSectors, HRCellPool, LRCellPool, ownership);
	
	act->executeMDP( *_agent, s, sp );
	applyFrameEffects( s, sp );
	sp.computeHash();	
	
	outcomes.push_back( std::make_pair(sp, 1.0) );
}

void	HunterGathererMDPModel::applyFrameEffects( const HunterGathererMDPState& s,  HunterGathererMDPState& sp ) const
{
	sp.consume();
	sp.increaseTimeIndex();
}
  
}
