
#include <MoveAction.hxx>

#include <World.hxx>
#include "Forager.hxx"
#include "ForagerState.hxx"
#include "QuantumWorld.hxx"
#include <GeneralState.hxx>

namespace QuantumExperiment
{

MoveAction::MoveAction( const Engine::Point2D<int> & position) : BaseAction(position)
{
}

MoveAction::~MoveAction()
{
}

BaseAction * MoveAction::copy() const
{
	MoveAction * newAction = new MoveAction(_position);
	return newAction;
}

void MoveAction::executeMDP( const Forager & forager, const ForagerState & state, ForagerState & stateNext ) const
{
	stateNext.setPosition(_position);

	Engine::Point2D<int> localPos = _position - forager.getWorld()->getOverlapBoundaries()._origin;
	int oldKnowledge = state.getKnowledgeMap().getValue(localPos);
	if(oldKnowledge<state.getKnowledgeMap().getMaxValueAt(localPos))
	{
		stateNext.getKnowledgeMap().setValue(localPos, 1);
	}
}

void MoveAction::execute( Engine::Agent & agent )
{
	Forager & forager = (Forager&)agent;
	forager.setPosition(_position);	
	
	Engine::Raster & knowledge = forager.getWorld()->getDynamicRasterStr(forager.getKnowledgeMap());
	int qualityKnowledge = knowledge.getValue(_position);
	if(qualityKnowledge<knowledge.getMaxValueAt(_position))
	{
		knowledge.setValue(_position, qualityKnowledge+1);
	}
}

std::string MoveAction::describe() const
{
	std::ostringstream oss;
	oss << "Move Action to: " << _position;
	return oss.str();
}

float MoveAction::getStarvationCost() const
{
	return 1.0f;
}

} // namespace QuantumExperiment

