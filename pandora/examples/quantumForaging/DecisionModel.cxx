
#include "DecisionModel.hxx"
#include "Forager.hxx"
#include "ForagerState.hxx"

#include "BaseAction.hxx"
#include "MoveAction.hxx"
#include "ForageAction.hxx"

#include <Exceptions.hxx>

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
	_initial = new ForagerState(_agent.getPosition(), _agent.getNeededResources(), _agent.getWorld()->getDynamicRasterStr(_agent.getKnowledgeMap()), _agent.getWorld()->getDynamicRasterStr(_agent.getResourcesMap()), 0);
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
		std::cout << "state: " << state << " is terminal due to exceed horizon: " << _agent.getHorizon() << std::endl;
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
	// TODO cost of action!
	float starvationCost = state.getAvailableAction(action).getStarvationCost();

	const Engine::Raster & knowledge = state.getKnowledgeMap();
	float qualityKnowledgeInPos = float(knowledge.getValue(state.getPosition()))/float(knowledge.getMaxValueAt(state.getPosition()));

	// adjacent cells
	Engine::Point2D<int> index(0,0);
	float qualityKnowledgeInAdjacentCells = 0.0f;
	int numNeighbors = 0.0f;
	std::cout << "agent: " << _agent << " at state: " << state << " has quality of position: " << qualityKnowledgeInPos << std::endl;
	for(int i=state.getPosition()._x-1; i<=state.getPosition()._x+1; i++)
	{
		for(int j=state.getPosition()._y-1; j<=state.getPosition()._y+1; j++)
		{
			Engine::Point2D<int> neighbor(i,j);
			if(!_agent.getWorld()->checkPosition(neighbor))
			{
				continue;
			}
			if(state.getPosition()==neighbor)
			{
				continue;
			}
			numNeighbors++;
			float value = float(knowledge.getValue(neighbor))/float(knowledge.getMaxValueAt(neighbor));
			std::cout << "\tknowledge value for neighbor: " << neighbor << " is: " << value << " having base balue: " << knowledge.getValue(neighbor) << " and max: " << knowledge.getMaxValueAt(neighbor) << std::endl;
			qualityKnowledgeInAdjacentCells += value;
		}
	}
	std::cout << "agent: " << _agent << " has quality of neighbors: " << qualityKnowledgeInAdjacentCells << " num neighbors: " << numNeighbors << " final value: " << 	qualityKnowledgeInAdjacentCells / numNeighbors << std::endl;
	qualityKnowledgeInAdjacentCells /= numNeighbors;

	//std::cout << "cost for state: " << state << " and action: " << action << " is starvation: " << starvationCost << " in knowledge: " << qualityKnowledgeInPos << " adjacent: " << qualityKnowledgeInAdjacentCells << std::endl;
	//std::cout << "action: " << action << " from state: " << state << " moving to: " << state.getAvailableAction(action).getPosition() << " is getting: " << foragedResources << " resources needed: " << neededResources << " base cost: " << cost << std::endl;
	float knowledgeCost = 0.0f;
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

			//std::cout << "from position: " << state.getPosition() << " creating new at: " << newPosition << std::endl;
			MoveAction * newAction = new MoveAction(newPosition);
			//std::cout << "creating new action: " << newAction << std::endl;
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

