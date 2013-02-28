
#include "MDPAgent.hxx"

#include <Statistics.hxx>
#include <World.hxx>
#include "MoveAction.hxx"

namespace Examples
{

MDPAgent::MDPAgent( const std::string & id, const int & neededResources, const int & birthResources ) : Agent(id), _resources(0), _neededResources(neededResources), _birthResources(birthResources), _numChildren(0), _model(0), _uctBasePolicy(0), _horizon(0), _width(0), _explorationBonus(0)
{
}

void MDPAgent::configureMDP( const int & horizon, const int & width, const int & explorationBonus )
{
	_horizon = horizon;
	_width = width;
	_explorationBonus = explorationBonus;

	if(_model)
	{
		delete _model;
	}
	if(_uctBasePolicy)
	{
		delete _uctBasePolicy;
	}
	_model = new MDPAgentModel();
	_model->setup(_horizon);
	_uctBasePolicy = new BasePolicy(*_model);
}

MDPAgent::~MDPAgent()
{
	delete _model;
	delete _uctBasePolicy;
}

void MDPAgent::selectActions()
{
//	std::cout << this << " selecting actions for time step: " << _world->getCurrentStep() << std::endl;	
	_model->reset(*this);
	UCT * uctPolicy = new UCT(*_uctBasePolicy, _horizon, _width, _explorationBonus, false);
	Problem::action_t index = (*uctPolicy)(_model->init());
	MoveAction * action = _model->init().getAvailableAction(index).copy();
//	std::cout << "action chosen with index: " << index << " is moving from: " << _position << " to: " << action->getNewPosition() << std::endl;
	delete uctPolicy;
	_actions.push_back(action);
//	std::cout << this << "end selecting actions for time step: " << _world->getCurrentStep() << std::endl;
}

void MDPAgent::updateState()
{
	_resources -= _neededResources;
	if(_resources<0)
	{
		remove();
	}
	if(_resources>_birthResources)
	{
		std::ostringstream oss;
		oss << _id << "_child" << _numChildren;

		MDPAgent * child = new MDPAgent(oss.str(), _neededResources, _birthResources);
		child->configureMDP(_horizon, _width, _explorationBonus);
		_world->addAgent(child);
		child->setPosition(_position);
		_resources -= _birthResources;
		_numChildren++;
	}
}

void MDPAgent::registerAttributes()
{
	registerIntAttribute("resources");
	registerIntAttribute("width");
	registerIntAttribute("horizon");
	registerIntAttribute("number of children");
}

void MDPAgent::serialize()
{
	serializeAttribute("resources", _resources);
	serializeAttribute("width", _width);
	serializeAttribute("horizon", _horizon);
	serializeAttribute("number of children", _numChildren);
}

void MDPAgent::setResources( int resources )
{
	_resources = resources;
}

int MDPAgent::getResources() const
{
	return _resources;
}

int MDPAgent::getNeededResources() const
{
	return _neededResources;
}

} // namespace Examples

