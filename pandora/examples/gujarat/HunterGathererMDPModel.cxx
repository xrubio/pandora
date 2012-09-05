#include <HunterGathererMDPModel.hxx>
#include <HunterGatherer.hxx>
#include <HunterGathererMDPConfig.hxx>
#include <ForageAction.hxx>
#include <MoveHomeAction.hxx>
#include <DoNothingAction.hxx>
#include <Exceptions.hxx>
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
	_initial = new HunterGathererMDPState(	agentRef().getPosition(),
						agentRef().getOnHandResources(),
						agentRef().getWorld()->getDynamicRaster( "resources" ),
						_config.getHorizon(),
						agentRef().computeConsumedResources(1) );
	makeActionsForState( *_initial );
	std::cout << "Initial state: " << *_initial << std::endl;	
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
	float cost = s.getDaysStarving()*10;
	cost += s.availableActions(a)->getTimeNeeded();
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
	assert( s.numAvailableActions() == 0 );
	// Make Do Nothing
	if ( _config.isDoNothingAllowed() )
		s.addAction( new DoNothingAction() );	
	
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
	if ( _config.getNumberForageActions() >= validActionSectors.size() )
	{
		for ( unsigned i = 0; i < validActionSectors.size(); i++ )
			s.addAction( new ForageAction( validActionSectors[i], true ) );	
	}
	else
	{
		for ( unsigned i = 0; i < _config.getNumberForageActions(); i++ )
			s.addAction( new ForageAction( validActionSectors[i], true ) );
		for ( unsigned i = _config.getNumberForageActions(); i < validActionSectors.size(); i++ )
			delete validActionSectors[i];
	}
	
	// Make Move Home
	std::vector< MoveHomeAction* > possibleMoveHomeActions;
	MoveHomeAction::generatePossibleActions( agentRef(), s.getLocation(), possibleMoveHomeActions );

	if ( _config.getNumberMoveHomeActions() >=  possibleMoveHomeActions.size() )
	{
		for ( unsigned i = 0; i < possibleMoveHomeActions.size(); i++ )
			s.addAction( possibleMoveHomeActions[i] );
	}
	else
	{
		for ( unsigned i = 0; i < _config.getNumberMoveHomeActions(); i++ )
			s.addAction( possibleMoveHomeActions[i] );
		for ( unsigned i = _config.getNumberMoveHomeActions(); i < possibleMoveHomeActions.size(); i++ )
			delete possibleMoveHomeActions[i];
	}
	assert( s.numAvailableActions() > 0 );
} 

}
