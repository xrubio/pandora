
#include "Herder.hxx"

#include <Statistics.hxx>
#include <World.hxx>
#include "MoveAction.hxx"
#include "HerderWorld.hxx"
#include "Village.hxx"

namespace GujaratCellphones
{

Herder::Herder( const std::string & id, int herdSize, int resourcesPerAnimal) : Agent(id), _resources(0), _starvationDays(0), _herdSize(herdSize), _resourcesPerAnimal(resourcesPerAnimal), _model(0), _uctBasePolicy(0), _horizon(0), _width(0), _explorationBonus(0)
{
}

void Herder::configureMDP( const int & horizon, const int & width, const int & explorationBonus )
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
	_model = new DecisionModel();
	_uctBasePolicy = new BasePolicy(*_model);
}

Herder::~Herder()
{
	delete _model;
	delete _uctBasePolicy;
}

void Herder::selectActions()
{
	HerderWorld & world = (HerderWorld &)*_world;
	if(world.daysUntilWetSeason()==0)
	{
		return;
	}
	//std::cout << this << " selecting actions for time step: " << _world->getCurrentStep() << " days until wet: " << world.daysUntilWetSeason() << std::endl;
	_model->reset(*this, world.daysUntilWetSeason(), _horizon);
	//std::cout << "mdp config, width: " << _width << " horizon: " << std::min(_horizon, world.daysUntilWetSeason()) << " bonus: " << _explorationBonus << std::endl;
	UCT * uctPolicy = new UCT(*_uctBasePolicy, _width, std::min(_horizon, world.daysUntilWetSeason()), _explorationBonus, false);
	Problem::action_t index = (*uctPolicy)(_model->init());
	MoveAction * action = _model->init().getAvailableAction(index).copy();

	/*
	for(int i=0; i<_model->init().getNumAvailableActions(); i++)
	{
		std::cout << "possible action: " << i << " moving from: " << _model->init().getPosition()  << " to: " << _model->init().getAvailableAction(i).getNewPosition() << " would have cost: "<< _model->cost( _model->init(), i) << std::endl;
	}
	std::cout << "action chosen for step: " << _world->getCurrentStep() << " with index: " << index << " is moving from: " << _position << " to: " << action->getNewPosition() << std::endl;
	*/
	delete uctPolicy;
	_actions.push_back(action);
	//std::cout << this << "end selecting actions for time step: " << _world->getCurrentStep() << std::endl;
}

int Herder::getNeededResources() const
{
	return _herdSize*_resourcesPerAnimal;
}

void Herder::updateState()
{
	HerderWorld & world = (HerderWorld &)*_world;
	if(world.getCurrentStep()==0)
	{
		return;
	}
	if(world.daysUntilWetSeason()==0)
	{
		std::cout << "dry season, herder: " << this << std::endl;
		// animals death by starvation
		int oldHerdSize = _herdSize;
		for(int i=0; i<oldHerdSize; i++)
		{
			int die = Engine::GeneralState::statistics().getUniformDistValue(0,1000);
			if(die<10*_starvationDays)
			{
				_herdSize = std::max(0, _herdSize-1);
			}
		}
		std::cout << this << " starvation: " << _starvationDays << " old herd size: " << oldHerdSize << " new: " << _herdSize << std::endl;
		_starvationDays = 0.0f;
		// no animals, remove agent
		if(_herdSize==0)
		{
			_village->removeHerder(this);
			remove();
			return;
		}
		// animals reproduction, 20%
		oldHerdSize = _herdSize;
		for(int i=0; i<oldHerdSize; i++)
		{
			if(Engine::GeneralState::statistics().getUniformDistValue(0,4)==0)
			{
				_herdSize++;
			}
		}
		setPosition(_village->getPosition());
		std::cout << this << " reproduction: old herd size: " << oldHerdSize << " new: " << _herdSize << std::endl;
		return;
	}
	if(_resources<getNeededResources())
	{
		_starvationDays += float(getNeededResources()-_resources)/float(getNeededResources());
	}
	_resources = 0;
}

void Herder::registerAttributes()
{
	registerIntAttribute("starvation x100");
	registerIntAttribute("herd size");
	registerIntAttribute("needed resources");
}

void Herder::serialize()
{
	serializeAttribute("starvation x100", _starvationDays*100.0f);
	serializeAttribute("herd size", _herdSize);
	serializeAttribute("needed resources", getNeededResources());
}

void Herder::setResources( int resources )
{
	_resources = resources;
}

int Herder::getResources() const
{
	return _resources;
}

void Herder::setVillage(Village * village)
{
	_village = village;
	_position = village->getPosition();
}

const Village & Herder::getVillage() const
{
	return *_village;
}
	
int Herder::getHerdSize() const
{
	return _herdSize;
}

void Herder::updateKnowledge()
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
		// tentative value = 1/3
		int averageValue = maxValue/3;
		std::cout << "min: " << minValue << " max: " << maxValue << " average: " << averageValue << std::endl;

		Engine::Point2D<int> index(0,0);
		for(index._x=0; index._x<_world->getOverlapBoundaries()._size._x; index._x++)
		{
			for(index._y=0; index._y<_world->getOverlapBoundaries()._size._y; index._y++)
			{
				knowledge.setValue(index, -1);
				resources.setMaxValue(index, averageValue);
				resources.setValue(index, averageValue);
			}
		}
		return;
	}
	HerderWorld & world = (HerderWorld &)*_world;
	if(world.daysUntilWetSeason()!=0)
	{
		return;
	}

	// info is one year old
	Engine::Point2D<int> index(0,0);
	for(index._x=0; index._x<_world->getOverlapBoundaries()._size._x; index._x++)
	{
		for(index._y=0; index._y<_world->getOverlapBoundaries()._size._y; index._y++)
		{
			// if it has been explored
			if(knowledge.getValue(index)!=-1)
			{
				knowledge.setValue(index, knowledge.getValue(index)+1);
			}
		}
	}
	// current resource values must be updated to max value
	resources.updateRasterToMaxValues();
}

void Herder::createKnowledge()
{	
	std::ostringstream oss;
	oss << _id << "_knowledge";
	_knowledgeMap = oss.str();
	std::ostringstream oss2;
	oss2 << _id << "_resources";
	_resourcesMap = oss2.str();

	_knowledgeMap = "knowledge";
	_resourcesMap = "resourcesMap";

//	_world->registerDynamicRaster(_knowledgeMap, false);
//	_world->registerDynamicRaster(_resourcesMap, false);
	_world->getDynamicRasterStr(_knowledgeMap).setInitValues(0, std::numeric_limits<int>::max(), 0);
	_world->getDynamicRasterStr(_resourcesMap).setInitValues(0, 0, 0);
	_world->getDynamicRasterStr(_resourcesMap).setMaxValue(std::numeric_limits<int>::max());
}
	
const std::string & Herder::getKnowledgeMap() const
{
	return _knowledgeMap;
}

const std::string & Herder::getResourcesMap() const
{
	return _resourcesMap;
}

} // namespace GujaratCellphones

