
#include <MDPAgent.hxx>
#include <EatAction.hxx>
#include <MoveAction.hxx>
#include <Statistics.hxx>
#include <World.hxx>

namespace Examples
{

MDPAgent::MDPAgent( const std::string & id ) : Agent(id), _resources(5)
{
}

MDPAgent::~MDPAgent()
{
}

void MDPAgent::selectActions()
{
	_actions.push_back(new MoveAction());
	_actions.push_back(new EatAction());
}

void MDPAgent::updateState()
{
	if(_resources<0)
	{
		remove();
	}
}

void MDPAgent::registerAttributes()
{
	registerIntAttribute("resources");
}

void MDPAgent::serialize()
{
	serializeAttribute("resources", _resources);
}

void MDPAgent::setResources( int resources )
{
	_resources = resources;
}

int MDPAgent::getResources() const
{
	return _resources;
}

} // namespace Examples

