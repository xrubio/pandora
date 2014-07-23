#include "Village.hxx"
#include "Herder.hxx"
#include "HerderWorld.hxx"
#include "HerderWorldConfig.hxx"

namespace GujaratCellphones
{

Village::Village(const std::string & id, int index) : Agent(id), _inVillageTransmission(0), _outVillageTransmission(0), _index(index), _agentKey(0)
													  
{
}

Village::~Village()
{
}

void Village::updateState()
{
}

int Village::getIndex() const
{
	return _index;
}

void Village::setInVillageTransmission( int inVillageTransmission) 
{
	_inVillageTransmission = inVillageTransmission;
}

int Village::getInVillageTransmission() const
{
	return _inVillageTransmission;
}

void Village::setOutVillageTransmission( int outVillageTransmission) 
{
	_outVillageTransmission = outVillageTransmission;
}

int Village::getOutVillageTransmission() const
{
	return _outVillageTransmission;
}


void Village::addHerder(Herder * herder)
{
	_herders.push_back(herder);
}

void Village::removeHerder(Herder * herder)
{
	std::list<Herder*>::iterator it=_herders.begin();
	while(it!=_herders.end())
	{	
		if((*it)->getId().compare(herder->getId())==0)
		{
			it = _herders.erase(it);
		}
		else
		{
			it++;
		}
	}
}

std::list<Herder*>::iterator Village::beginHerders()
{
	return _herders.begin();
}

std::list<Herder*>::iterator Village::endHerders()
{
	return _herders.end();
}

void Village::registerAttributes()
{
	registerIntAttribute("herders");
	registerIntAttribute("total animals");
	registerIntAttribute("in village transmission");
	registerIntAttribute("out village transmission");
	registerIntAttribute("known 3-year cells");
}

void Village::serialize()
{
	serializeAttribute("herders", int(_herders.size()));
	int totalAnimals = 0;
	for(std::list<Herder*>::iterator it=_herders.begin(); it!=_herders.end(); it++)
	{
		totalAnimals += (*it)->getHerdSize();
	}
	serializeAttribute("total animals", totalAnimals); 
	serializeAttribute("in village transmission", _inVillageTransmission);
	serializeAttribute("out village transmission", _outVillageTransmission);

	int knownCells = 0;
	//std::cout << "init village: " << *this << std::endl;
	for(std::list<Herder*>::iterator it=_herders.begin(); it!=_herders.end(); it++)
	{
		Herder & herder = **it;
		Engine::DynamicRaster knowledge = _world->getDynamicRaster(herder.getKnowledgeMap());

		int herderKnownCells = 0;
		Engine::Point2D<int> index(0,0);
        for(auto index : _world->getBoundaries())
		{
			int value = knowledge.getValue(index);
			if(value>-1 && value<3)
			{
				//std::cout << "\t\tindex : " << index << " value: " << value << std::endl;
				herderKnownCells++;
			}
		}
		//std::cout << "\therder: " << herder << " with raster: " << herder.getKnowledgeMap() << " knows: " << herderKnownCells << std::endl;
		knownCells += herderKnownCells;
	}
	//std::cout << "end village: " << *this << " known cells: " << knownCells << std::endl;
	serializeAttribute("known 3-year cells", knownCells);
}

void Village::fission( Herder & original )
{
	HerderWorld & world = (HerderWorld &)*_world;
	const HerderWorldConfig & config = (const HerderWorldConfig &)world.getConfig();

	std::ostringstream ossH;
	ossH << "Herder_"<<getNewKey()<<"_vil" << _index;
	int numAnimals = original.getHerdSize()/2;
	
	Herder * newHerder = new Herder(ossH.str(), numAnimals, config._resourcesNeededPerAnimal, *this);
	newHerder->configureMDP(config._horizon, config._width, config._explorationBonus);

	world.addAgent(newHerder);
	newHerder->createKnowledge();
	newHerder->shareKnowledge(original);
	original.setHerdSize(original.getHerdSize()-numAnimals);

}

long int Village::getNewKey()
{
	return _agentKey++;
}

} // namespace GujaratCellphones

