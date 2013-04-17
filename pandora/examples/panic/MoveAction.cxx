
#include "MoveAction.hxx"
#include <Agent.hxx>

namespace Panic
{
	
MoveAction::MoveAction( const Engine::Point2D<int> & position ) : _position(position)
{
}

MoveAction::~MoveAction()
{
}

void MoveAction::execute( Engine::Agent & agent )
{
	agent.setPosition(_position);
}

std::string MoveAction::describe() const
{
	return "Move action";
}

} // namespace Panic

