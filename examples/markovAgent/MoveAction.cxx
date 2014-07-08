
#include <MoveAction.hxx>

#include <World.hxx>
#include <MDPAgent.hxx>
#include <MDPAgentState.hxx>
#include <GeneralState.hxx>
#include "RandomWorld.hxx"

namespace Examples
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

void MoveAction::executeMDP( const MDPAgent & agent, const MDPAgentState & state, MDPAgentState & stateNext ) const
{
	stateNext.setPosition(_newPosition);
	Engine::Point2D<int> localPos = agent.getPosition() - agent.getWorld()->getOverlapBoundaries()._origin;
	stateNext.setResources(state.getResources()+state.getRasterResources().getValue(localPos));
//	std::cout << "executing from state: " << state << " to: " << stateNext << " getting value: " << state.getRasterResources().getValue(localPos) << " size of inc. raster: " << state.getRasterResources().getSize() << std::endl;

	stateNext.getRasterResources().setValue(localPos, 0);
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
	MDPAgent & mdpAgent = (MDPAgent&)agent;
	mdpAgent.setResources(mdpAgent.getResources()+mdpAgent.getWorld()->getValue(eResources,_newPosition));
	mdpAgent.getWorld()->setValue(eResources,_newPosition, 0);
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

} // namespace Examples

