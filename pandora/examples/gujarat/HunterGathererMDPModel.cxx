
#include <HunterGathererMDPModel.hxx>
#include <HunterGatherer.hxx>
#include <HunterGathererMDPConfig.hxx>
#include <GujaratWorld.hxx>
#include <ForageAction.hxx>
#include <MoveHomeAction.hxx>
#include <DoNothingAction.hxx>
#include <Exception.hxx>
#include <typeinfo>

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
		delete _initial;
}

void	HunterGathererMDPModel::setup( const HunterGathererMDPConfig& cfg )
{
	_config = cfg;
	setHorizon( cfg.getHorizon() );
}

void	HunterGathererMDPModel::reset( GujaratAgent & agent )
{
	if ( _initial != NULL )
		delete _initial;


	_simAgent = dynamic_cast<HunterGatherer *>(&agent);

	// Build initial state from current state in the simulation
	_initial = new HunterGathererMDPState(	agentRef().getPosition(), agentRef().getOnHandResources(), agentRef().getWorld()->getDynamicRaster(eResources), _config.getHorizon(), agentRef().computeConsumedResources(1));
	makeActionsForState( *_initial );
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
	HunterGathererMDPState sp;
	s.initializeSuccessor(sp);
	const MDPAction* act = s.availableActions(a);
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
	//std::cout << "creating actions for state with time index: " << s.getTimeIndex() << " and resources: " << s.getOnHandResources() << std::endl;
	assert( s.numAvailableActions() == 0 );
	// Make Do Nothing
	//if ( _config.isDoNothingAllowed() )
	//	s.addAction( new DoNothingAction() );	
	
	// Make Forage actions
	std::vector< Sector* > validActionSectors;
	std::vector< Sector* > actionSectors;

	agentRef().updateKnowledge( s.getLocation(), s.getResourcesRaster(), actionSectors );

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
	std::random_shuffle( validActionSectors.begin(), validActionSectors.end() );
	std::sort( validActionSectors.begin(), validActionSectors.end(), SectorBestFirstSortPtrVecPredicate() );
	int forageActions = _config.getNumberForageActions();
	if ( forageActions >= validActionSectors.size() )
	{
		for ( unsigned i = 0; i < validActionSectors.size(); i++ )
		{
			s.addAction( new ForageAction( validActionSectors[i], true ) );	
		}
	}
	else
	{
		for ( unsigned i = 0; i < forageActions; i++ )
			s.addAction( new ForageAction( validActionSectors[i], true ) );
		for ( unsigned i = forageActions; i < validActionSectors.size(); i++ )
			delete validActionSectors[i];
	}
	//std::cout << "number of valid forage actions: " << s.numAvailableActions() << " for number of valid sectors: " << validActionSectors.size() << std::endl;

	// Make Move Home
	std::vector< MoveHomeAction* > possibleMoveHomeActions;
	MoveHomeAction::generatePossibleActions( agentRef(), s.getLocation(), possibleMoveHomeActions );
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
	//std::cout << "finished creating actions for state with time index: " << s.getTimeIndex() << " and resources: " << s.getOnHandResources() << std::endl;
} 

}
