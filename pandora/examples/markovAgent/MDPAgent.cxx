
#include "MDPAgent.hxx"

#include <Statistics.hxx>
#include <World.hxx>
#include "MoveAction.hxx"

namespace Examples
{

MDPAgent::MDPAgent( const std::string & id, const int & horizon, const int & width, const int & explorationBonus ) : Agent(id), _resources(0), _model(0), _uctBasePolicy(0), _horizon(horizon), _width(width), _explorationBonus(explorationBonus)
{
	_model = new MDPAgentModel();
	// horizon
	_model->setup(1);
	_uctBasePolicy = new BasePolicy(*_model);
}

MDPAgent::~MDPAgent()
{
	delete _model;
	delete _uctBasePolicy;
}

void MDPAgent::selectActions()
{
	std::cout << this << " selecting actions for time step: " << _world->getCurrentStep() << std::endl;
	_model->reset(*this);
	UCT * uctPolicy = new UCT(*_uctBasePolicy, _horizon, _width, _explorationBonus, false);
	Problem::action_t index = (*uctPolicy)(_model->init());
	MoveAction * action = _model->init().getAvailableAction(index).copy();
	std::cout << "action chosen with index: " << index << " is moving from: " << _position << " to: " << action->getNewPosition() << std::endl;
	delete uctPolicy;
	_actions.push_back(action);
	std::cout << this << "end selecting actions for time step: " << _world->getCurrentStep() << std::endl;
}

void MDPAgent::updateState()
{
//	_resources -= 2;
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

