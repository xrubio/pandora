
#include <ForageAction.hxx>

#include <World.hxx>
#include "Forager.hxx"
#include "ForagerState.hxx"
#include "QuantumWorld.hxx"
#include <GeneralState.hxx>

namespace QuantumExperiment
{

ForageAction::ForageAction( const Engine::Point2D<int> & position ) : BaseAction(position)
{
}

ForageAction::~ForageAction()
{
}

BaseAction * ForageAction::copy() const
{
	ForageAction * newAction = new ForageAction(_position);
	return newAction;
}

void ForageAction::executeMDP( const Forager & forager, const ForagerState & state, ForagerState & stateNext ) const
{
	std::cout << "bar" << std::endl;
	Engine::Point2D<int> localPos = _position - forager.getWorld()->getOverlapBoundaries()._origin;
	int previousValue = forager.getWorld()->getValue(eResources,_position);
	int foragedResources = std::min(forager.getNeededResources(), previousValue);
	stateNext.setForagedResources(foragedResources);

//	std::cout << "executing from state: " << state << " to: " << stateNext << " getting value: " << state.getRasterResources().getValue(localPos) << " size of inc. raster: " << state.getRasterResources().getSize() << std::endl;
	stateNext.getResourcesMap().setValue(localPos, previousValue-foragedResources);	
	
	int oldKnowledge = state.getKnowledgeMap().getValue(localPos);
	if(oldKnowledge<state.getKnowledgeMap().getMaxValueAt(localPos))
	{
		stateNext.getKnowledgeMap().setValue(localPos, 1);
	}
}

void ForageAction::execute( Engine::Agent & agent )
{
	std::cout << "bar2" << std::endl;
	Forager & forager = (Forager&)agent;

	int previousValue = forager.getWorld()->getValue(eResources,_position);
	int collected = std::min(forager.getNeededResources(), previousValue);
	forager.setCurrentResources(collected);

	int rest = previousValue - collected;
	forager.getWorld()->setValue(eResources, _position, rest);

	// we set the maximum value of the mental map of the agent to the value before gathering in case it is the first time this year
	Engine::Raster & knowledge = forager.getWorld()->getDynamicRasterStr(forager.getKnowledgeMap());
	int qualityKnowledge = knowledge.getValue(_position);
	if(qualityKnowledge<knowledge.getMaxValueAt(_position))
	{
		knowledge.setValue(_position, qualityKnowledge+1);
	}

	// how to model an improvement?
	if(forager.getWorld()->getDynamicRasterStr(forager.getResourcesMap()).getMaxValueAt(_position)<rest)
	{
		forager.getWorld()->getDynamicRasterStr(forager.getResourcesMap()).setMaxValue(_position, rest);
	}
	forager.getWorld()->getDynamicRasterStr(forager.getResourcesMap()).setValue(_position, rest);
}
	
std::string ForageAction::describe() const
{
	std::ostringstream oss;
	oss << "Forage Action at: " << _position << ", resources: " << _resources;
	return oss.str();
}

} // namespace QuantumExperiment

