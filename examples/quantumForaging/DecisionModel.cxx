
#include "DecisionModel.hxx"
#include "Forager.hxx"
#include "ForagerState.hxx"

#include "BaseAction.hxx"
#include "MoveAction.hxx"
#include "ForageAction.hxx"

#include <Exception.hxx>

using Problem::action_t;

namespace QuantumExperiment
{

DecisionModel::DecisionModel( Forager & agent ) : _agent(agent), _initial(0)
{
}

DecisionModel::~DecisionModel()
{
	if(_initial)
	{
		delete _initial;
	}
}

void DecisionModel::reset()
{
	if(_initial)
	{
		delete _initial;
		_initial = 0;
	}
	_initial = new ForagerState(_agent.getPosition(), _agent.getNeededResources(), _agent.getWorld()->getDynamicRaster(_agent.getKnowledgeMap()), _agent.getWorld()->getDynamicRaster(_agent.getResourcesMap()), 0);
	makeActionsForState(*_initial);
}

action_t DecisionModel::number_actions( const ForagerState & state ) const
{
	return state.getNumAvailableActions();
}

const ForagerState & DecisionModel::init() const
{
	return *_initial;
}

bool DecisionModel::terminal( const ForagerState & state ) const
{
	if(state.getTimeStep()>_agent.getHorizon())
	{
		return true;
	}
	return false;
}
	
bool DecisionModel::dead_end( const ForagerState & state ) const
{
	return false;
}

bool DecisionModel::applicable( const ForagerState & state, action_t action) const
{
	return true;
}

float DecisionModel::cost( const ForagerState & state, action_t action ) const
{
	float starvationCost = state.getAvailableAction(action).getStarvationCost();

	const Engine::DynamicRaster & knowledge = state.getKnowledgeMap();

	float localKnowledgeIncrease = 0.0f;
	if(state.getAvailableAction(action).getPosition()==state.getPosition() && knowledge.getValue(state.getPosition())<knowledge.getMaxValue(state.getPosition()))
	{
		localKnowledgeIncrease += 1.0f;	 
	}

	float neighborKnowledgeIncrease = 0.0f;
	if(state.getAvailableAction(action).getPosition()!=state.getPosition() && knowledge.getValue(state.getAvailableAction(action).getPosition())<knowledge.getMaxValue(state.getAvailableAction(action).getPosition()))
	{
		neighborKnowledgeIncrease += 1.0f;
	}
	
	float knowledgeCost = (1.0f-localKnowledgeIncrease)*_agent.getAmbiguityAversion() + (1.0f-neighborKnowledgeIncrease)*_agent.getRiskAversion();
//	std::cout << "knowledge costs: " << knowledgeCost << " with ambiguity: " << localKnowledgeIncrease*_agent.getAmbiguityAversion() << " with aversion: " << _agent.getAmbiguityAversion() << " risk: " << neighborKnowledgeIncrease*_agent.getRiskAversion() << " with aversion: " << _agent.getRiskAversion() << std::endl;
//	std::cout << "final cost: " << starvationCost+knowledgeCost << " starv.: " << starvationCost << " knowledge: " << knowledgeCost << std::endl;
	return starvationCost+knowledgeCost;
}

void DecisionModel::next( const ForagerState & state, action_t index, OutcomeVector & outcomes ) const
{
	ForagerState stateNext(state);
	const BaseAction & action = state.getAvailableAction(index);
	action.executeMDP( _agent, state, stateNext );
	applyFrameEffects(state, stateNext);
	stateNext.computeHash();	
	makeActionsForState(stateNext);
	outcomes.push_back(std::make_pair(stateNext, 1.0));
}

void DecisionModel::applyFrameEffects( const ForagerState & state,  ForagerState & stateNext ) const
{
	stateNext.consume();
	stateNext.increaseTimeStep();
}

void DecisionModel::makeActionsForState( ForagerState & state ) const
{
	state.clearActions();
	assert(state.getNumAvailableActions()==0);
	// 8 possible movements
	for(int i=state.getPosition()._x-1; i<=state.getPosition()._x+1; i++)
	{
		for(int j=state.getPosition()._y-1; j<=state.getPosition()._y+1; j++)
		{
			Engine::Point2D<int> newPosition(i,j);
			if(!_agent.getWorld()->checkPosition(newPosition))
			{
				continue;
			}
			if(state.getPosition()==newPosition)
			{
				continue;
			}

			MoveAction * newAction = new MoveAction(newPosition);
			state.addAction(newAction);
		}
	}
	// the last possible action is to remain in cell and forage
	ForageAction * forageAction = new ForageAction(state.getPosition(), state.getResourcesMap(), state.getNeededResources());
	state.addAction(forageAction);
	state.randomizeActions();
	assert(state.getNumAvailableActions()>0);
}

} // namespace QuantumExperiment

