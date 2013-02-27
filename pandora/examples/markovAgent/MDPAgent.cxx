
#include "MDPAgent.hxx"

#include <Statistics.hxx>
#include <World.hxx>
#include "MoveAction.hxx"

namespace Examples
{

MDPAgent::MDPAgent( const std::string & id, const int & neededResources, const int & horizon, const int & width, const int & explorationBonus ) : Agent(id), _resources(0), _neededResources(neededResources), _model(0), _uctBasePolicy(0), _horizon(horizon), _width(width), _explorationBonus(explorationBonus)
{
	_model = new MDPAgentModel();
	// horizon
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
}

void MDPAgent::registerAttributes()
{
	registerIntAttribute("resources");
	registerIntAttribute("width");
	registerIntAttribute("horizon");
}

void MDPAgent::serialize()
{
	serializeAttribute("resources", _resources);
	serializeAttribute("width", _width);
	serializeAttribute("horizon", _horizon);
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

