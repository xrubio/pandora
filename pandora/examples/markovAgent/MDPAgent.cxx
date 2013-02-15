
#include "MDPAgent.hxx"

#include <Statistics.hxx>
#include <World.hxx>
#include "MoveAction.hxx"

namespace Examples
{

MDPAgent::MDPAgent( const std::string & id ) : Agent(id), _resources(4), _model(0), _uctBasePolicy(0)
{
	_model = new MDPAgentModel();
	// horizon
	_model->setup(1);
	_uctBasePolicy = new BasePolicy(*_model);
}

MDPAgent::~MDPAgent()
{
	delete _model;
	if(_uctBasePolicy)
	{
		delete _uctBasePolicy;
	}
}

void MDPAgent::selectActions()
{
	_model->reset(*this);
	UCT * uctPolicy = new UCT(*_uctBasePolicy, 10, 2, 10, false);
//	UCT * uctPolicy = new UCT(*_uctBasePolicy, _mdpWidth, _mdpHorizon, _mdpExplorationBonus, false);
	Problem::action_t index = (*uctPolicy)(_model->init());
	MoveAction * action = _model->init().getAvailableAction(index).copy();
	delete uctPolicy;
	_actions.push_back(action);
}

void MDPAgent::updateState()
{
	_resources -= 2;
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

