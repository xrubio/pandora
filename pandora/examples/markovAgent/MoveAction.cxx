
#include <MoveAction.hxx>

#include <World.hxx>
#include <MDPAgent.hxx>
#include <MDPAgentState.hxx>
#include <GeneralState.hxx>

namespace Examples
{

MoveAction::MoveAction( const Engine::Point2D<int> & newPosition ) : _newPosition(newPosition)
{
}

MoveAction::~MoveAction()
{
}

MoveAction * MoveAction::copy() const
{
	return new MoveAction(_newPosition);
}

void MoveAction::executeMDP( const MDPAgent & agent, const MDPAgentState & state, MDPAgentState & stateNext ) const
{
	stateNext.setPosition(_newPosition);
}

void MoveAction::execute( Engine::Agent & agent )
{
	agent.setPosition(_newPosition);
}

} // namespace Examples

