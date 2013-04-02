
#include <MoveAction.hxx>

#include <World.hxx>
#include "Forager.hxx"
#include "ForagerState.hxx"
#include "QuantumWorld.hxx"
#include <GeneralState.hxx>

namespace QuantumExperiment
{

MoveAction::MoveAction( const Engine::Point2D<int> & newPosition ) : _newPosition(newPosition)
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

void MoveAction::executeMDP( const Forager & forager, const ForagerState & state, ForagerState & stateNext ) const
{
	stateNext.setPosition(_newPosition);
	Engine::Point2D<int> localPos = forager.getPosition() - forager.getWorld()->getOverlapBoundaries()._origin;
	int previousValue = forager.getWorld()->getValue(eResources,_newPosition);
	int collected = std::min(forager.getNeededResources(), previousValue);
	stateNext.setForagedResources(collected);
//	std::cout << "executing from state: " << state << " to: " << stateNext << " getting value: " << state.getRasterResources().getValue(localPos) << " size of inc. raster: " << state.getRasterResources().getSize() << std::endl;
	stateNext.getResourcesMap().setValue(localPos, previousValue-collected);
}

void MoveAction::execute( Engine::Agent & agent )
{
	Forager & forager = (Forager&)agent;
	forager.setPosition(_newPosition);

	int previousValue = forager.getWorld()->getValue(eResources,_newPosition);
	int collected = std::min(forager.getNeededResources(), previousValue);
	forager.setCurrentResources(collected);

	int rest = previousValue - collected;
	forager.getWorld()->setValue(eResources, _newPosition, rest);

	// we set the maximum value of the mental map of the agent to the value before gathering in case it is the first time this year
	int qualityKnowledge = forager.getWorld()->getDynamicRasterStr(forager.getKnowledgeMap()).getValue(_newPosition);
	if(qualityKnowledge<100)
	{
		forager.getWorld()->getDynamicRasterStr(forager.getKnowledgeMap()).setValue(_newPosition, qualityKnowledge+1);
	}

	if(forager.getWorld()->getDynamicRasterStr(forager.getResourcesMap()).getMaxValueAt(_newPosition)<rest)
	{
		forager.getWorld()->getDynamicRasterStr(forager.getResourcesMap()).setMaxValue(_newPosition, rest);
	}
	forager.getWorld()->getDynamicRasterStr(forager.getResourcesMap()).setValue(_newPosition, rest);
}

const Engine::Point2D<int> & MoveAction::getNewPosition() const
{
	return _newPosition;
}

} // namespace QuantumExperiment

