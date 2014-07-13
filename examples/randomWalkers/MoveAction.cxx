
#include <MoveAction.hxx>

#include <World.hxx>
#include <RandomAgent.hxx>
#include <GeneralState.hxx>

namespace Examples
{

MoveAction::MoveAction()
{
}

MoveAction::~MoveAction()
{
}

void MoveAction::execute( Engine::Agent & agent )
{	
	Engine::World * world = agent.getWorld();
	
	Engine::Point2D<int> newPosition = agent.getPosition();
	int modX = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
	newPosition._x += modX;
	int modY = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
	newPosition._y += modY;

	if(world->checkPosition(newPosition))
	{
		agent.setPosition(newPosition);
	}
}

std::string MoveAction::describe() const
{
	return "MoveAction";
}

} // namespace Examples

