
#include "Herder.hxx"

#include <Statistics.hxx>

#include "MoveAction.hxx"
#include "HerderWorld.hxx"
#include "HerderWorldConfig.hxx"
#include "DecisionModel.hxx"

namespace GujaratCellphones
{

Herder::Herder( const std::string & id, int herdSize, int resourcesPerAnimal, Village & village) : Agent(id), _resources(0), _starvationDays(0), _herdSize(herdSize), _resourcesPerAnimal(resourcesPerAnimal), _model(0), _uctBasePolicy(0), _horizon(0), _width(0), _explorationBonus(0), _village(&village)
{
	_village->addHerder(this);
	setPosition(_village->getPosition());
}

Herder::~Herder()
{
	delete _model;
	delete _uctBasePolicy;
}

void Herder::setResources( int resources )
{
	_resources = resources;
}

int Herder::getResources() const
{
	return _resources;
}

int Herder::getNeededResources() const
{
	return _herdSize*_resourcesPerAnimal;
}

int Herder::getHerdSize() const
{
	return _herdSize;
}

const std::string & Herder::getKnowledgeMap() const
{
	return _knowledgeMap;
}

const std::string & Herder::getResourcesMap() const
{
	return _resourcesMap;
}

const Village & Herder::getVillage() const
{
	return *_village;
}

void Herder::createKnowledge()
{	
	std::ostringstream oss;
	oss << _id << "_knowledge";
	_knowledgeMap = oss.str();
	std::ostringstream oss2;
	oss2 << _id << "_resources";
	_resourcesMap = oss2.str();

	_world->registerDynamicRaster(_knowledgeMap, false);
	_world->registerDynamicRaster(_resourcesMap, false);
	_world->getDynamicRasterStr(_knowledgeMap).setInitValues(-1, std::numeric_limits<int>::max(), -1);
	_world->getDynamicRasterStr(_resourcesMap).setInitValues(0, std::numeric_limits<int>::max(), 0);
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
		//std::cout << "min: " << minValue << " max: " << maxValue << " average: " << averageValue << std::endl;

		Engine::Point2D<int> index(0,0);
		for(index._x=0; index._x<_world->getOverlapBoundaries()._size._x; index._x++)
		{
			for(index._y=0; index._y<_world->getOverlapBoundaries()._size._y; index._y++)
			{
				knowledge.setValue(index, -1);
				resources.setMaxValue(index, std::numeric_limits<int>::max());
				resources.setMaxValue(index, averageValue);
				resources.setValue(index, averageValue);
			}
		}
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

void Herder::inVillageKnowledgeTransmission() const
{
	knowledgeTransmission(_village->getInVillageTransmission());
}

void Herder::outVillageKnowledgeTransmission() const
{
	knowledgeTransmission(_village->getOutVillageTransmission());
}

void Herder::knowledgeTransmission( int frequency ) const
{
	//std::cout << "checking knowledge for: " << this << " with frequency: " << frequency << std::endl;
	for(std::list<Herder*>::const_iterator it=_village->beginHerders(); it!=_village->endHerders(); it++)
	{
		int value = Engine::GeneralState::statistics().getUniformDistValue(0, 100);
		if(value<frequency) 
		{
			//std::cout << "sharing knowledge between: " << this << " and: " << **it << " frequency: " << frequency  << " at village with transmission: " << _village->getInVillageTransmission() << "/" << _village->getOutVillageTransmission() << std::endl;
			shareKnowledge(**it);
		}
	}
}

/*void Herder::printRaster(Engine::Raster & raster) {
	Engine::Point2D<int> index(0,0);
	for(index._x=0; index._x<_world->getOverlapBoundaries()._size._x; index._x++)
	{
		for(index._y=0; index._y<_world->getOverlapBoundaries()._size._y; index._y++)
		{
			std::cout << raster.getValue(index) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}*/

void Herder::copyValue( const Herder & origin, const Herder & target, const Engine::Point2D<int> & index ) const
{
	Engine::Raster & knowledgeOrigin = _world->getDynamicRasterStr(origin.getKnowledgeMap());
	Engine::Raster & resourcesOrigin = _world->getDynamicRasterStr(origin.getResourcesMap());

	Engine::Raster & knowledgeTarget = _world->getDynamicRasterStr(target.getKnowledgeMap());
	Engine::Raster & resourcesTarget = _world->getDynamicRasterStr(target.getResourcesMap());

	knowledgeTarget.setValue(index, knowledgeOrigin.getValue(index));
	resourcesTarget.setMaxValue(index, resourcesOrigin.getMaxValueAt(index));
	resourcesTarget.setValue(index, resourcesOrigin.getValue(index));
}

void Herder::shareCell( const Herder & herderA, const Herder & herderB, const Engine::Point2D<int> & index ) const
{
	Engine::Raster & knowledgeA = _world->getDynamicRasterStr(herderA.getKnowledgeMap());
	Engine::Raster & resourcesA = _world->getDynamicRasterStr(herderA.getResourcesMap());

	Engine::Raster & knowledgeB = _world->getDynamicRasterStr(herderB.getKnowledgeMap());
	Engine::Raster & resourcesB = _world->getDynamicRasterStr(herderB.getResourcesMap());

	if(knowledgeA.getValue(index)>knowledgeB.getValue(index))
	{
		copyValue(herderB, herderA, index);
		return;
	}
	if(knowledgeA.getValue(index)<knowledgeB.getValue(index))
	{
		copyValue(herderA, herderB, index);
		return;
	}
	if(knowledgeA.getValue(index)==knowledgeB.getValue(index))
	{
		if(Engine::GeneralState::statistics().getUniformDistValue(0,1)==0)
		{
			copyValue(herderA, herderB, index);
		}
		else
		{
			copyValue(herderB, herderA, index);
		}
	}
}

void Herder::shareKnowledge(Herder & herder) const
{
	Engine::Point2D<int> index(0,0);
	for(index._x=0; index._x<_world->getOverlapBoundaries()._size._x; index._x++)
	{
		for(index._y=0; index._y<_world->getOverlapBoundaries()._size._y; index._y++)
		{
			shareCell(*this, herder, index );
		}
	}
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

void Herder::updateState()
{
	HerderWorld & world = (HerderWorld &)*_world;
	const HerderWorldConfig & config = world.getConfig();
	if(world.getCurrentStep()==0)
	{
		return;
	}
	if(world.daysUntilWetSeason()==0)
	{
		//std::cout << "dry season, herder: " << this << std::endl;
		// animals death by starvation
		int oldHerdSize = _herdSize;
		for(int i=0; i<oldHerdSize; i++)
		{
			int die = Engine::GeneralState::statistics().getUniformDistValue(0,1000);
			if(die<int((1000.0f*_starvationDays)/(float)config._daysDrySeason))
			{
				_herdSize = std::max(0, _herdSize-1);
			}
		}
		//std::cout << this << " starvation: " << _starvationDays << " check: " << (int)(1000.0f*_starvationDays/(float)config._daysDrySeason) << " old herd size: " << oldHerdSize << " new: " << _herdSize << std::endl;
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
		//std::cout << this << " reproduction: old herd size: " << oldHerdSize << " new: " << _herdSize << std::endl;

		if(config._inVillageTransmission)
		{
			inVillageKnowledgeTransmission();
		}
		return;
	}
	
	
	if(config._outVillageTransmission)
	{
		outVillageKnowledgeTransmission();
	}

	if(_resources<getNeededResources())
	{
		_starvationDays += float(getNeededResources()-_resources)/float(getNeededResources());
	}
	//std::cout << " needed: " << getNeededResources() << " resources: " << _resources << " starvation acc: " << _starvationDays << std::endl;
	_resources = 0;
}

void Herder::registerAttributes()
{
	registerIntAttribute("starvation x100");
	registerIntAttribute("herd size");
	registerIntAttribute("needed resources");
	registerIntAttribute("village");
}

void Herder::serialize()
{
	serializeAttribute("starvation x100", (int)(_starvationDays*100.0f));
	serializeAttribute("herd size", _herdSize);
	serializeAttribute("needed resources", getNeededResources());
	serializeAttribute("village", _village->getIndex());
}

} // namespace GujaratCellphones

