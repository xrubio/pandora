
#include "DecisionModel.hxx"
#include "Herder.hxx"
#include "HerderState.hxx"
#include "HerderWorld.hxx"
#include "MoveAction.hxx"

#include <Exception.hxx>

using Problem::action_t;

namespace GujaratCellphones
{

DecisionModel::DecisionModel() : _agent(0), _initial(0), _daysUntilWetSeason(0)
{
}

DecisionModel::~DecisionModel()
{
	if(_initial)
	{
		delete _initial;
	}
}

void DecisionModel::reset( Herder & agent, int daysUntilWetSeason, int horizon)
{
	_daysUntilWetSeason = daysUntilWetSeason;
	_horizon = horizon;
	if(_initial)
	{
		delete _initial;
		_initial = 0;
	}
	_agent= &agent;
	_initial = new HerderState(agent.getPosition(), agent.getWorld()->getDynamicRaster(agent.getKnowledgeMap()), agent.getWorld()->getDynamicRaster(agent.getResourcesMap()), agent.getNeededResources());
	makeActionsForState(*_initial);
}

action_t DecisionModel::number_actions( const HerderState & state ) const
{
	return state.getNumAvailableActions();
}

const HerderState & DecisionModel::init() const
{
	return *_initial;
}

bool DecisionModel::terminal( const HerderState & state ) const
{
    return false;
}
	
bool DecisionModel::dead_end( const HerderState & state ) const
{
	return false;
}

bool DecisionModel::applicable( const HerderState & state, action_t action) const
{
	return true;
}

float DecisionModel::cost( const HerderState & state, action_t action ) const
{
	float resourcesToCollect = state.getResourcesMap().getValue(state.getAvailableAction(action).getNewPosition());
    float cost =  0.0f - resourcesToCollect;
	return cost;
}

void DecisionModel::next( const HerderState & state, action_t index, OutcomeVector & outcomes ) const
{
	HerderState stateNext(state);
	const MoveAction & moveAction = state.getAvailableAction(index);
	moveAction.executeMDP( *_agent, state, stateNext );
	applyFrameEffects(state, stateNext);
	stateNext.computeHash();	
	makeActionsForState(stateNext);
	outcomes.push_back(std::make_pair(stateNext, 1.0));
}

void DecisionModel::applyFrameEffects( const HerderState & state,  HerderState & stateNext ) const
{
}

void DecisionModel::makeActionsForState( HerderState & state ) const
{
	state.clearActions();
	assert(state.getNumAvailableActions()==0);
	for(int i=state.getPosition()._x-1; i<=state.getPosition()._x+1; i++)
	{
		for(int j=state.getPosition()._y-1; j<=state.getPosition()._y+1; j++)
		{
			Engine::Point2D<int> newPosition(i,j);
			if(!_agent->getWorld()->checkPosition(newPosition))
			{
				continue;
			}
			MoveAction * newAction = new MoveAction(newPosition);
			state.addAction(newAction);
		}
	}
	assert(state.getNumAvailableActions()>0);
	state.randomizeActions();
}

} // namespace GujaratCellphones

