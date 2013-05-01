
#include <ForageAction.hxx>

#include <World.hxx>
#include "Forager.hxx"
#include "ForagerState.hxx"
#include "QuantumWorld.hxx"
#include <Raster.hxx>
#include <GeneralState.hxx>

namespace QuantumExperiment
{

ForageAction::ForageAction( const Engine::Point2D<int> & position, float fractionForaged ) : BaseAction(position), _fractionForaged(fractionForaged)
{
}

ForageAction::ForageAction( const Engine::Point2D<int> & position, const Engine::Raster & resourcesMap, int neededResources) : BaseAction(position)
{	
	int previousValue = resourcesMap.getValue(position);
	int foragedResources = std::min(neededResources, previousValue);
	_fractionForaged = (float)foragedResources/(float)neededResources; 
}

ForageAction::~ForageAction()
{
}

BaseAction * ForageAction::copy() const
{
	ForageAction * newAction = new ForageAction(_position, _fractionForaged);
	return newAction;
}

void ForageAction::executeMDP( const Forager & forager, const ForagerState & state, ForagerState & stateNext ) const
{
	Engine::Point2D<int> localPos = _position - forager.getWorld()->getOverlapBoundaries()._origin;

	int previousValue = state.getResourcesMap().getValue(localPos);
	int foragedResources = std::min(forager.getNeededResources(), previousValue);
	stateNext.setForagedResources(foragedResources);

//	std::cout << "executing from state: " << state << " to: " << stateNext << " getting value: " << state.getRasterResources().getValue(localPos) << " size of inc. raster: " << state.getRasterResources().getSize() << std::endl;
	stateNext.getResourcesMap().setValue(localPos, previousValue-foragedResources);	
	
	int oldKnowledge = state.getKnowledgeMap().getValue(localPos);
	if(oldKnowledge<state.getKnowledgeMap().getMaxValueAt(localPos))
	{
		stateNext.getKnowledgeMap().setValue(localPos, oldKnowledge+1);
	}
}

void ForageAction::execute( Engine::Agent & agent )
{
	Forager & forager = (Forager&)agent;

	int previousValue = forager.getWorld()->getValue(eResources,_position);
	int collected = std::min(forager.getNeededResources(), previousValue);
	forager.setCurrentResources(collected);

	int rest = previousValue - collected;
	forager.getWorld()->setValue(eResources, _position, rest);

	// decrease the uncertainty of values
	Engine::Raster & knowledge = forager.getWorld()->getDynamicRaster(forager.getKnowledgeMap());
	int qualityKnowledge = knowledge.getValue(_position);
	if(qualityKnowledge<knowledge.getMaxValueAt(_position))
	{
		knowledge.setValue(_position, qualityKnowledge+1);
	}

	// finally, we test uncertainty in order to know if the agent gets the real value
	int value = Engine::GeneralState::statistics().getUniformDistValue(0,knowledge.getMaxValueAt(_position));
	if(value<knowledge.getValue(_position))
	{
		return;
	}
	if(forager.getWorld()->getDynamicRaster(forager.getResourcesMap()).getMaxValueAt(_position)<rest)
	{
		forager.getWorld()->getDynamicRaster(forager.getResourcesMap()).setMaxValue(_position, rest);
	}
	forager.getWorld()->getDynamicRaster(forager.getResourcesMap()).setValue(_position, rest);
}
	
std::string ForageAction::describe() const
{
	std::ostringstream oss;
	oss << "Forage Action at: " << _position << ", fraction foraged: " << _fractionForaged;
	return oss.str();
}

float ForageAction::getStarvationCost() const
{
	return 1.0f - _fractionForaged;
}

} // namespace QuantumExperiment

