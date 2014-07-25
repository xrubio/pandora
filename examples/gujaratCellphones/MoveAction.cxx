
#include <MoveAction.hxx>

#include <World.hxx>
#include "Herder.hxx"
#include "HerderState.hxx"
#include <GeneralState.hxx>
#include "HerderWorld.hxx"

namespace GujaratCellphones
{

MoveAction::MoveAction( const Engine::Point2D<int> & newPosition) : _newPosition(newPosition)
{
}

MoveAction::~MoveAction()
{
}

MoveAction * MoveAction::copy() const
{
	MoveAction * newAction = new MoveAction(_newPosition);
	return newAction;
}

void MoveAction::executeMDP( const Herder & agent, const HerderState & state, HerderState & stateNext ) const
{
	stateNext.setPosition(_newPosition);
	Engine::Point2D<int> localPos = agent.getPosition() - agent.getWorld()->getBoundaries()._origin;
	int collected = std::min(agent.getNeededResources(), state.getResourcesMap().getValue(localPos));
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
	int modifiedValue = previousValue - collected;
	herder.getWorld()->setValue(eResources, _newPosition, modifiedValue);

	// we set the maximum value of the mental map of the agent to the value before gathering in case it is the first time this year
	if(herder.getWorld()->getDynamicRaster(herder.getKnowledgeMap()).getValue(_newPosition)!=0)
	{
		herder.getWorld()->getDynamicRaster(herder.getKnowledgeMap()).setValue(_newPosition, 0);
		herder.getWorld()->getDynamicRaster(herder.getResourcesMap()).setMaxValue(_newPosition, previousValue);
	}
	// if the agent already visited the cell this year we will set the max value to an average value between both samples
	else 
	{	
		if(herder.getWorld()->getDynamicRaster(herder.getResourcesMap()).getMaxValue(_newPosition)<modifiedValue)
		{
			herder.getWorld()->getDynamicRaster(herder.getResourcesMap()).setMaxValue(_newPosition, modifiedValue);
		}
		else
		{
			int previousMaxValue = herder.getWorld()->getDynamicRaster(herder.getResourcesMap()).getMaxValue(_newPosition);
			int newMaxValue = previousValue;
			herder.getWorld()->getDynamicRaster(herder.getResourcesMap()).setMaxValue(_newPosition, (previousMaxValue+newMaxValue)/2);
		}
	}

	/*
	if(herder.getWorld()->getDynamicRaster(herder.getResourcesMap()).getMaxValue(_newPosition)<modifiedValue)
	{
		herder.getWorld()->getDynamicRaster(herder.getResourcesMap()).setMaxValue(_newPosition, modifiedValue);
	}
	*/
	herder.getWorld()->getDynamicRaster(herder.getResourcesMap()).setValue(_newPosition, modifiedValue);
	// knowledge is updated to 0 years
	//herder.getWorld()->getDynamicRaster("gathered").setValue(_newPosition, herder.getWorld()->getDynamicRaster("gathered").getValue(_newPosition)+1);

	//std::cout << "herder: " << herder << " has collected: " << collected << " from: " << previousValue << " with needed: " << herder.getNeededResources() << std::endl;
}

const Engine::Point2D<int> & MoveAction::getNewPosition() const
{
	return _newPosition;
}


/*
std::ostream & operator<<( std::ostream & stream, MoveAction * action )
{
	return stream << " action: " << *action << " newPos: " << action->_newPosition;
}
*/

} // namespace GujaratCellphones

