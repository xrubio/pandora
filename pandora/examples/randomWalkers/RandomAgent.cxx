
#include <RandomAgent.hxx>
#include <EatAction.hxx>
#include <MoveAction.hxx>
#include <Statistics.hxx>
#include <World.hxx>

namespace Examples
{

RandomAgent::RandomAgent( const std::string & id ) : Agent(id), _resources(5)
{
}

RandomAgent::~RandomAgent()
{
}

void RandomAgent::selectActions()
{
	_actions.push_back(new MoveAction());
	_actions.push_back(new EatAction());
}

void RandomAgent::updateState()
{
	if(_resources<0)
	{
		remove();
	}
}

void RandomAgent::registerAttributes()
{
	registerIntAttribute("resources");
}

void RandomAgent::serialize()
{
	serializeAttribute("resources", _resources);
}

void RandomAgent::setResources( int resources )
{
	_resources = resources;
}

int RandomAgent::getResources() const
{
	return _resources;
}

} // namespace Examples

