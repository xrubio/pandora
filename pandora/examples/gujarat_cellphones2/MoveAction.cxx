
#include <MoveAction.hxx>

#include <World.hxx>
#include "Herder.hxx"
#include "HerderState.hxx"
#include <GeneralState.hxx>
#include "HerderWorld.hxx"

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

void MoveAction::executeMDP( const Herder & agent, const HerderState & state, HerderState & stateNext ) const
{
	stateNext.setPosition(_newPosition);
	Engine::Point2D<int> localPos = agent.getPosition() - agent.getWorld()->getOverlapBoundaries()._origin;

	int collected = std::min(agent.getNeededResources(), state.getResourcesMap().getValue(localPos));
	stateNext.setResources(collected);
//	std::cout << "executing from state: " << state << " to: " << stateNext << " getting value: " << state.getRasterResources().getValue(localPos) << " size of inc. raster: " << state.getRasterResources().getSize() << std::endl;
	stateNext.getResourcesMap().setValue(localPos, state.getResourcesMap().getValue(localPos)-collected);
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
	Herder & herder = (Herder&)agent;

	int previousValue = herder.getWorld()->getValue(eResources,_newPosition);
	int collected = std::min(herder.getNeededResources(), agent.getWorld()->getValue(eResources,_newPosition));
	herder.setResources(collected);
	herder.getWorld()->setValue(eResources, _newPosition, previousValue - collected);

	// we set the maximum value of the mental map of the agent to the value before gathering in case it is the first time this year
	if(herder.getWorld()->getDynamicRasterStr(herder.getKnowledgeMap()).getValue(_newPosition)!=0)
	{
		herder.getWorld()->getDynamicRasterStr(herder.getKnowledgeMap()).setValue(_newPosition, 0);
		herder.getWorld()->getDynamicRasterStr(herder.getResourcesMap()).setMaxValue(_newPosition, previousValue);
	}
	herder.getWorld()->getDynamicRasterStr(herder.getResourcesMap()).setValue(_newPosition, previousValue - collected);
	// knowledge is updated to 0 years
	herder.getWorld()->getDynamicRasterStr("gathered").setValue(_newPosition, herder.getWorld()->getDynamicRasterStr("gathered").getValue(_newPosition)+1);
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

