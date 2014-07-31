
#include "Forager.hxx"

#include <Statistics.hxx>

#include "MoveAction.hxx"
#include "QuantumWorld.hxx"
#include "DecisionModel.hxx"

namespace QuantumExperiment
{

Forager::Forager( const std::string & id, int neededResources, bool perfectInformation, float riskAversion ) : Agent(id), _currentResources(0), _neededResources(neededResources), _perfectInformation(perfectInformation), _starvation(0), _model(0), _uctBasePolicy(0), _horizon(0), _width(0), _explorationBonus(0), _riskAversion(riskAversion), _ambiguityAversion(1.0f-riskAversion), _moveActions(0)
{
}

Forager::~Forager()
{
	if(_model)
	{
		delete _model;
	}
	if(_uctBasePolicy)
	{
		delete _uctBasePolicy;
	}
}

void Forager::setCurrentResources( int currentResources )
{
	_currentResources = currentResources;
}

int Forager::getCurrentResources() const
{
	return _currentResources;
}

int Forager::getNeededResources() const
{
	return _neededResources; 
}

const std::string & Forager::getKnowledgeMap() const
{
	return _knowledgeMap;
}

const std::string & Forager::getResourcesMap() const
{
	return _resourcesMap;
}

void Forager::createKnowledge()
{	
	std::ostringstream oss;
	oss << _id << "_knowledge";
	_knowledgeMap = oss.str();
	std::ostringstream oss2;
	oss2 << _id << "_resources";
	_resourcesMap = oss2.str();
}

void Forager::updateKnowledge()
{
	Engine::DynamicRaster & knowledge = _world->getDynamicRaster(_knowledgeMap);
	Engine::DynamicRaster & resources = _world->getDynamicRaster(_resourcesMap);

	// first step, init environment
	if(_world->getCurrentStep()==0)
	{
		// average value on every cell
		_world->getDynamicRaster(eResources).updateCurrentMinMaxValues();
		int minValue =  _world->getDynamicRaster(eResources).getCurrentMinValue();
		int maxValue =  _world->getDynamicRaster(eResources).getCurrentMaxValue();
		// tentative value = 1/2
		int averageValue = maxValue/2;
		//std::cout << "min: " << minValue << " max: " << maxValue << " average: " << averageValue << std::endl;

        for( auto index : _world->getBoundaries())
		{
			if(_perfectInformation)
			{
				resources.setMaxValue(index, _world->getValue(eResources, index));
			}
			else
			{
				resources.setMaxValue(index, averageValue);
			}
		}
		resources.updateRasterToMaxValues();
		return;
	}
}

void Forager::configureMDP( const int & horizon, const int & width, const int & explorationBonus )
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
	_model = new DecisionModel(*this);
	_uctBasePolicy = new BasePolicy(*_model);
}

void Forager::selectActions()
{
	QuantumWorld & world = (QuantumWorld &)*_world;
	_model->reset();
	UCT * uctPolicy = new UCT(*_uctBasePolicy, _width, _horizon, _explorationBonus, false);
	Problem::action_t index = (*uctPolicy)(_model->init());
	BaseAction * action = _model->init().getAvailableAction(index).copy();
	delete uctPolicy;
	_actions.push_back(action);

	if(action->getPosition()!=_position)
	{
		_moveActions++;
	}
}

void Forager::updateState()
{
	_starvation += 1.0f - float(_currentResources)/float(_neededResources);
	_currentResources = 0;
}

void Forager::registerAttributes()
{
	registerIntAttribute("starvation x100");
	registerIntAttribute("needed resources");
	registerIntAttribute("width");
	registerIntAttribute("horizon");
	registerIntAttribute("move actions");
	registerIntAttribute("risk aversion x100");
}

void Forager::serialize()
{
	serializeAttribute("starvation x100", (int)(_starvation*100.0f));
	serializeAttribute("needed resources", getNeededResources());
	serializeAttribute("width", _width); 
	serializeAttribute("horizon", _horizon);
	serializeAttribute("move actions", _moveActions);
	serializeAttribute("risk aversion x100", (int)(_riskAversion*100.0f));
}

int Forager::getHorizon() const
{
	return _horizon;
}

float Forager::getRiskAversion() const
{
	return _riskAversion;
}

float Forager::getAmbiguityAversion() const
{
	return _ambiguityAversion;
}

} // namespace QuantumExperiment

