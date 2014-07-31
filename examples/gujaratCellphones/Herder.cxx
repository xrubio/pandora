
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

void Herder::setHerdSize( int herdSize )
{
	_herdSize = herdSize;
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
	_world->getDynamicRaster(_knowledgeMap).setInitValues(-1, std::numeric_limits<int>::max(), -1);
	_world->getDynamicRaster(_resourcesMap).setInitValues(0, std::numeric_limits<int>::max(), 0);
}

void Herder::updateKnowledge()
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
		int averageValue = (maxValue-minValue)/2;
		// tentative value = 0.5 of mean
        averageValue /= 2;

        for(auto index: _world->getBoundaries())
        {
			knowledge.setValue(index, -1);
			resources.setMaxValue(index, averageValue);
			resources.setValue(index, averageValue);
		}
		return;
	}
	HerderWorld & world = (HerderWorld &)*_world;
	if(world.daysUntilWetSeason()!=0)
	{
		return;
	}
	// info is one year old 
    for(auto index: _world->getBoundaries())
    {
        // if it has been explored
        if(knowledge.getValue(index)!=-1)
        {
            knowledge.setValue(index, knowledge.getValue(index)+1);
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
	for(std::list<Herder*>::const_iterator it=_village->beginHerders(); it!=_village->endHerders(); it++)
	{
		int value = Engine::GeneralState::statistics().getUniformDistValue(0, 100);
		if(value<frequency) 
		{
			shareKnowledge(**it);
		}
	}
}

void Herder::copyValue( const Herder & origin, const Herder & target, const Engine::Point2D<int> & index ) const
{
	Engine::DynamicRaster & knowledgeOrigin = _world->getDynamicRaster(origin.getKnowledgeMap());
	Engine::DynamicRaster & resourcesOrigin = _world->getDynamicRaster(origin.getResourcesMap());

	Engine::DynamicRaster & knowledgeTarget = _world->getDynamicRaster(target.getKnowledgeMap());
	Engine::DynamicRaster & resourcesTarget = _world->getDynamicRaster(target.getResourcesMap());

	knowledgeTarget.setValue(index, knowledgeOrigin.getValue(index));
	resourcesTarget.setMaxValue(index, resourcesOrigin.getMaxValue(index));
	resourcesTarget.setValue(index, resourcesOrigin.getValue(index));
}

void Herder::shareCell( const Herder & herderA, const Herder & herderB, const Engine::Point2D<int> & index ) const
{
	Engine::DynamicRaster & knowledgeA = _world->getDynamicRaster(herderA.getKnowledgeMap());
	Engine::DynamicRaster & knowledgeB = _world->getDynamicRaster(herderB.getKnowledgeMap());

    // agent B did not visit the cell
	if(knowledgeA.getValue(index)>=knowledgeB.getValue(index) && knowledgeB.getValue(index)==-1)
	{
		copyValue(herderA, herderB, index);
	}
	// agent A has more recent information
    else if(knowledgeA.getValue(index)<knowledgeB.getValue(index))
    {
		copyValue(herderA, herderB, index);
	}
}

void Herder::shareKnowledge(Herder & herder) const
{
    for(auto index : _world->getBoundaries())
    {
        // give knowledge to other agent
	    shareCell(*this, herder, index );
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
	_model->reset(*this, world.daysUntilWetSeason(), _horizon);
	UCT * uctPolicy = new UCT(*_uctBasePolicy, _width, std::min(_horizon, world.daysUntilWetSeason()), _explorationBonus, false);
	Problem::action_t index = (*uctPolicy)(_model->init());
	MoveAction * action = _model->init().getAvailableAction(index).copy();

	delete uctPolicy;
	_actions.push_back(action);
}

void Herder::updateState()
{
    if(!_exists || _world->getCurrentStep()==0)
    {
        return;
    }

    HerderWorld & world = (HerderWorld &)*_world;
    const HerderWorldConfig & config = (const HerderWorldConfig &)world.getConfig();
    // in village activities
	if(world.daysUntilWetSeason()==0)
	{	
        int percentage = 100*(int)_starvationDays/config._daysDrySeason;
        std::cout << this << " starvation: " << _starvationDays << " of: " << config._daysDrySeason << " precentage: " <<percentage <<" old herd: " << _herdSize;
        int newHerdSize = 0;
        for(int i=0; i<_herdSize; i++)
        {
            int value = Engine::GeneralState::statistics().getUniformDistValue(0, 100);
            if(value>=percentage)
            {
                newHerdSize++;
            }
        }
        // animals death by starvation
        std::cout << " new herd: " << newHerdSize;
        _herdSize = newHerdSize;
		// 20% reproduction
        for(int i=0; i<newHerdSize; i++)
        {
            int value = Engine::GeneralState::statistics().getUniformDistValue(0, 9);
            if(value<2)
            {
                _herdSize++;
            }

        }
        std::cout << " after breeding: " << _herdSize << std::endl;
        // if no herd, die
		if(_herdSize==0)
		{
			_village->removeHerder(this);
			remove();
			return;
		}
		// fission
		if(_herdSize>config._animalsPerHerder)
		{
			_village->fission(*this);
		}

		_starvationDays = 0.0f;
		setPosition(_village->getPosition());		

		if(config._inVillageTransmission)
		{
			inVillageKnowledgeTransmission();
		}
		return;
	}

    // out of village activities
	if(config._outVillageTransmission)
	{
		outVillageKnowledgeTransmission();
	}
	if(_resources<getNeededResources())
	{
		_starvationDays += 1.0f - float(_resources)/float(getNeededResources());
	}
}

void Herder::registerAttributes()
{
	registerIntAttribute("starvation x100");
	registerIntAttribute("herd size");
	registerIntAttribute("needed resources");
	registerIntAttribute("collected");
	registerIntAttribute("village");
}

void Herder::serialize()
{
	serializeAttribute("starvation x100", (int)(_starvationDays*100.0f));
	serializeAttribute("herd size", _herdSize);
	serializeAttribute("needed resources", getNeededResources());
	serializeAttribute("collected", _resources);
	serializeAttribute("village", _village->getIndex());
}

} // namespace GujaratCellphones

