
#include "DecisionModel.hxx"
#include "Herder.hxx"
#include "HerderState.hxx"
#include "HerderWorld.hxx"
#include "MoveAction.hxx"

#include <Exceptions.hxx>

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
//	std::cout << "reset size: " << agent.getWorld()->getDynamicRaster(eResources).getSize() << std::endl;
//	std::cout << "reset model for agent: " << agent << " days until wet: " << _daysUntilWetSeason << std::endl;
	if(_initial)
	{
		delete _initial;
		_initial = 0;
	}
	_agent= &agent;
	_initial = new HerderState(agent.getPosition(), agent.getVillage().getPosition(), agent.getResources(), agent.getWorld()->getDynamicRasterStr(agent.getKnowledgeMap()), agent.getWorld()->getDynamicRasterStr(agent.getResourcesMap()), agent.getNeededResources(), _daysUntilWetSeason);
	makeActionsForState(*_initial);
//	std::cout << "end reset model for agent: " << agent << std::endl;
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
	int currentHorizon = std::min(_horizon, (int)_daysUntilWetSeason);
	// end of dry season
	if(state.getTimeStep()==0)
	{
		return true;
	}
	if(state.getTimeStep()<(_daysUntilWetSeason-currentHorizon))
	{
//		std::cout << "state: " << state << " is terminal due to be lesser than " << _daysUntilWetSeason << " less: " << currentHorizon << std::endl;
		return true;
	}
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
	// last day and next move is not into village, maximum cost
	if(state.getTimeStep()==1)
	{
		return 0.0f;
	}
	float cost = 0.0f;
	float resourcesToCollect = state.getResourcesMap().getValue(state.getAvailableAction(action).getNewPosition());
	float neededResources = state.getResourcesToEat();
	if(resourcesToCollect<neededResources)
	{
		cost += (neededResources-resourcesToCollect)/neededResources;
	}
//	std::cout << "action: " << action << " from state: " << state << " moving to: " << state.getAvailableAction(action).getNewPosition() << " is getting: " << resourcesToCollect << " resources needed: " << neededResources << " base cost: " << cost<< std::endl;

	int maxDist = state.getResourcesMap().getSize()._x;
	if(state.getTimeStep()<maxDist)
	{
		float firstDistance = state.getPosition().distance(state.getVillagePosition());
		float secondDistance = state.getAvailableAction(action).getNewPosition().distance(state.getVillagePosition());
		if(secondDistance>=firstDistance)
		{
			cost += (maxDist-state.getTimeStep());
//			std::cout << "new cost: " << cost << " due to distance: " << secondDistance << " - " << firstDistance << " weight: " <<25-state.getTimeStep() << std::endl;
		}
	}

	/*
	// knowledge penalisation if the zone is not explored (increased risk)
	int knowledge = state.getKnowledgeMap().getValue(state.getAvailableAction(action).getNewPosition());
	if(knowledge>=2 && knowledge <10)
	{
		cost += (float)(knowledge)/(10.0f*2.0f);
	}
	else if(knowledge>=0 && knowledge <2)
	{
	}
	// now known or really old
	else
	{
		cost += Engine::GeneralState::statistics().getUniformDistValue(0.0f,1.0f);
	}
	*/
//	std::cout << "added cost for risk: " << cost << " knowledge: " << knowledge << std::endl;
//	std::cout << " new cost: " << cost << std::endl;
	return cost;
}

void DecisionModel::next( const HerderState & state, action_t index, OutcomeVector & outcomes ) const
{
//	std::cout << "creating next state" << std::endl;
	HerderState stateNext(state);
	const MoveAction & moveAction = state.getAvailableAction(index);
	moveAction.executeMDP( *_agent, state, stateNext );
	applyFrameEffects(state, stateNext);
	stateNext.computeHash();	
	makeActionsForState(stateNext);
	outcomes.push_back(std::make_pair(stateNext, 1.0));
//	std::cout << "end creating next state" << std::endl;
}

void DecisionModel::applyFrameEffects( const HerderState & state,  HerderState & stateNext ) const
{
	stateNext.eat();
	stateNext.increaseTimeStep();
}

void DecisionModel::makeActionsForState( HerderState & state ) const
{
	state.clearActions();
	assert(state.getNumAvailableActions()==0);
	//is it necessary?
	//_agentSim->updateKnowledge(state.getPosition(), s.getResources());
	for(int i=state.getPosition()._x-1; i<=state.getPosition()._x+1; i++)
	{
		for(int j=state.getPosition()._y-1; j<=state.getPosition()._y+1; j++)
		{
			Engine::Point2D<int> newPosition(i,j);
			if(!_agent->getWorld()->checkPosition(newPosition))
			{
	//			std::cout << "from position: " << state.getPosition() << " discarding position: " << newPosition << std::endl;
				continue;
			}
	//		std::cout << "from position: " << state.getPosition() << " creating new at: " << newPosition << std::endl;
			MoveAction * newAction = new MoveAction(newPosition);
	//		std::cout << "creating new action: " << newAction << std::endl;
			state.addAction(newAction);
		}
	}
	state.randomizeActions();

	for(int i=0; i<state.getNumAvailableActions(); i++)
	{
		const MoveAction & action = state.getAvailableAction(i);

	}
	assert(state.getNumAvailableActions()>0);
}

} // namespace GujaratCellphones

