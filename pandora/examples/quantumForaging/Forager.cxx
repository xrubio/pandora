
#include "Forager.hxx"

#include <Statistics.hxx>

#include "MoveAction.hxx"
#include "QuantumWorld.hxx"
#include "QuantumWorldConfig.hxx"
#include "DecisionModel.hxx"

namespace QuantumExperiment
{

Forager::Forager( const std::string & id, int neededResources) : Agent(id), _currentResources(0), _neededResources(neededResources), _model(0), _uctBasePolicy(0), _horizon(0), _width(0), _explorationBonus(0)
{
}

Forager::~Forager()
{
	delete _model;
	delete _uctBasePolicy;
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

	_world->registerDynamicRaster(_knowledgeMap, false);
	_world->registerDynamicRaster(_resourcesMap, false);
	_world->getDynamicRasterStr(_knowledgeMap).setInitValues(0, 100, 0);
	_world->getDynamicRasterStr(_resourcesMap).setInitValues(0, std::numeric_limits<int>::max(), 0);
}

void Forager::updateKnowledge()
{
	Engine::Raster & knowledge = _world->getDynamicRasterStr(_knowledgeMap);
	Engine::Raster & resources = _world->getDynamicRasterStr(_resourcesMap);

	// first step, init environment
	if(_world->getCurrentStep()==0)
	{
		// average value on every cell
		_world->getDynamicRaster(eResources).updateCurrentMinMaxValues();
		int minValue =  _world->getDynamicRaster(eResources).getCurrentMinValue();
		int maxValue =  _world->getDynamicRaster(eResources).getCurrentMaxValue();
		// tentative value = 1/2
		int averageValue = maxValue/2;
		std::cout << "min: " << minValue << " max: " << maxValue << " average: " << averageValue << std::endl;

		Engine::Point2D<int> index(0,0);
		for(index._x=0; index._x<_world->getOverlapBoundaries()._size._x; index._x++)
		{
			for(index._y=0; index._y<_world->getOverlapBoundaries()._size._y; index._y++)
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
	//std::cout << "mdp config, width: " << _width << " horizon: " << std::min(_horizon, world.daysUntilWetSeason()) << " bonus: " << _explorationBonus << std::endl;
	UCT * uctPolicy = new UCT(*_uctBasePolicy, _width, _horizon, _explorationBonus, false);
	Problem::action_t index = (*uctPolicy)(_model->init());
	MoveAction * action = _model->init().getAvailableAction(index).copy();

	for(int i=0; i<_model->init().getNumAvailableActions(); i++)
	{
		std::cout << "possible action: " << i << " moving from: " << _model->init().getPosition()  << " to: " << _model->init().getAvailableAction(i).getNewPosition() << " would have cost: "<< _model->cost( _model->init(), i) << std::endl;
	}
	std::cout << "action chosen for step: " << _world->getCurrentStep() << " with index: " << index << " is moving from: " << _position << " to: " << action->getNewPosition() << std::endl;
	
	delete uctPolicy;
	_actions.push_back(action);
	std::cout << this << " end selecting actions for time step: " << _world->getCurrentStep() << std::endl;
}

void Forager::updateState()
{
	_starvation += 1.0f - float(_currentResources)/float(_neededResources);
	std::cout << this << " starvation: " << _starvation << std::endl;
	//std::cout << " needed: " << getNeededResources() << " resources: " << _resources << " starvation acc: " << _starvationDays << std::endl;
	_currentResources = 0;
}

void Forager::registerAttributes()
{
	registerIntAttribute("starvation x100");
	registerIntAttribute("needed resources");
}

void Forager::serialize()
{
	serializeAttribute("starvation x100", (int)(_starvation*100.0f));
	serializeAttribute("needed resources", getNeededResources());
}

int Forager::getHorizon() const
{
	return _horizon;
}

} // namespace QuantumExperiment

