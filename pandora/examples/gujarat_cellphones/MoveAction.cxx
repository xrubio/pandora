
#include <MoveAction.hxx>

#include <World.hxx>
#include <MyAgent.hxx>
#include <MDPAgentState.hxx>
#include <GeneralState.hxx>
#include "MyWorld.hxx"

namespace GujaratCellphones
{

MoveAction::MoveAction( const Engine::Point2D<int> & newPosition, int resourcesToCollect ) : _newPosition(newPosition), _resourcesToCollect(resourcesToCollect)
{
}

MoveAction::~MoveAction()
{
}

MoveAction * MoveAction::copy() const
{
	MoveAction * newAction = new MoveAction(_newPosition, _resourcesToCollect);
	return newAction;
}

void MoveAction::executeMDP( MyAgent & agent, const MDPAgentState & state, MDPAgentState & stateNext ) const
{
	stateNext.setPosition(_newPosition);
	Engine::Point2D<int> localPos = agent.getPosition() - agent.getWorld()->getOverlapBoundaries()._origin;
	
	int valueInMentalWorldRepr = agent.getValueCellMentalWorldRepresentation(localPos._x, localPos._y);
	//if the info is unknown, average(2) is assumed
	if (valueInMentalWorldRepr == -1) valueInMentalWorldRepr = 2; 
	if (valueInMentalWorldRepr == 3) valueInMentalWorldRepr = agent._config.getResourcesHighLevel() + agent._config.getResourcesLowLevel()/2;
	else if (valueInMentalWorldRepr == 2) valueInMentalWorldRepr = agent._config.getResourcesHighLevel() - agent._config.getResourcesLowLevel();
	else valueInMentalWorldRepr = agent._config.getResourcesLowLevel()/2;
	stateNext.setResources(state.getResources()+valueInMentalWorldRepr);
//	std::cout << "executing from state: " << state << " to: " << stateNext << " getting value: " << state.getRasterResources().getValue(localPos) << " size of inc. raster: " << state.getRasterResources().getSize() << std::endl;

	--valueInMentalWorldRepr;
	if (valueInMentalWorldRepr < 0) valueInMentalWorldRepr = 0;
	if (valueInMentalWorldRepr == 3) valueInMentalWorldRepr = agent._config.getResourcesHighLevel() + agent._config.getResourcesLowLevel()/2;
	else if (valueInMentalWorldRepr == 2) valueInMentalWorldRepr = agent._config.getResourcesHighLevel() -agent._config.getResourcesLowLevel();
	else if (valueInMentalWorldRepr == 1) valueInMentalWorldRepr = agent._config.getResourcesLowLevel()/2;
	else valueInMentalWorldRepr = 0;
	stateNext.getRasterResources().setValue(localPos, valueInMentalWorldRepr);

	/*
	for(int i=0; i<stateNext.getRasterResources().getSize()._x; i++)
	{
		for(int j=0; j<stateNext.getRasterResources().getSize()._y; j++)
		{
			std::cout << "raster - " << i << "/" << j << " : " << stateNext.getRasterResources().getValue(Engine::Point2D<int>(i,j)) << std::endl;
		}
	}
	*/
}

void MoveAction::execute( Engine::Agent & agent )
{
	agent.setPosition(_newPosition);
	MyAgent & mdpAgent = (MyAgent&)agent;
	int resourcesToCollect = mdpAgent._config.getResourcesAgentCanGetPerDay();
	if (resourcesToCollect > mdpAgent.getWorld()->getValue(eResources,_newPosition)) {
		resourcesToCollect = mdpAgent.getWorld()->getValue(eResources,_newPosition);
	}
	mdpAgent.setResources(mdpAgent.getResources() + resourcesToCollect);
	mdpAgent.getWorld()->setValue(eResources,_newPosition, mdpAgent.getWorld()->getValue(eResources,_newPosition) - resourcesToCollect);
}

const Engine::Point2D<int> & MoveAction::getNewPosition() const
{
	return _newPosition;
}

int MoveAction::getResourcesToCollect() const
{
	return _resourcesToCollect;
}

/*
std::ostream & operator<<( std::ostream & stream, MoveAction * action )
{
	return stream << " action: " << *action << " newPos: " << action->_newPosition;
}
*/

} // namespace GujaratCellphones

