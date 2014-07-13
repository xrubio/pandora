
#include "MoveAction.hxx"

#include <Agent.hxx>

#include "Scenario.hxx"

namespace Panic
{
	
MoveAction::MoveAction( const Engine::Point2D<int> & position, float agentCompressiontWeight, float wallCompressionWeight, int panicRadius ) : _position(position), _agentCompressionWeight(agentCompressiontWeight), _wallCompressionWeight(wallCompressionWeight), _panicRadius(panicRadius)
{
}

MoveAction::~MoveAction()
{
}

void MoveAction::execute( Engine::Agent & agent )
{	
	// update the number of agents in the cell and the compression level
	int numAgentsPreviousCell = agent.getWorld()->getValue(eNumAgents, agent.getPosition());
	agent.getWorld()->setValue(eNumAgents, agent.getPosition(), numAgentsPreviousCell-1);
	int adjacentWallsPreviousCell = agent.getWorld()->getValue(eWalls, agent.getPosition());
	int value = (numAgentsPreviousCell-1)*_agentCompressionWeight*(1+(adjacentWallsPreviousCell*_wallCompressionWeight));
	agent.getWorld()->setValue(eCompression, agent.getPosition(), value);
	agent.setPosition(_position);

	// new position is exit, no effect
	if(agent.getWorld()->getValue(eExits, _position)==1)
	{
		return;
	}

	int numAgentsNewCell= agent.getWorld()->getValue(eNumAgents, _position);
	agent.getWorld()->setValue(eNumAgents, _position, numAgentsNewCell+1);
	int adjacentWallsNewCell = agent.getWorld()->getValue(eWalls, _position);
	value = (numAgentsNewCell+1)*_agentCompressionWeight*(1+(adjacentWallsNewCell*_wallCompressionWeight));	
	agent.getWorld()->setValue(eCompression, _position, value);

	// if the place and its neighbors were not panicked now they are
	Engine::Point2D<int> neighbor(0,0);
	for(neighbor._x=_position._x-_panicRadius; neighbor._x<=_position._x+_panicRadius; neighbor._x++)
	{
		for(neighbor._y=_position._y-_panicRadius; neighbor._y<=_position._y+_panicRadius; neighbor._y++)
		{
			if(!agent.getWorld()->checkPosition(neighbor) || agent.getWorld()->getValue(eObstacles, neighbor)==1 || agent.getWorld()->getValue(eExits, neighbor)==1)
			{
				continue;
			}
			agent.getWorld()->setValue(ePanic, neighbor, 1);
		}
	}

}

std::string MoveAction::describe() const
{
	return "Move action";
}

} // namespace Panic

