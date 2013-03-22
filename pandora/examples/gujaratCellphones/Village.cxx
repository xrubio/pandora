#include "Village.hxx"
#include "Herder.hxx"

namespace GujaratCellphones
{

Village::Village(const std::string & id, int index) : Agent(id), _inVillageTransmission(0), _outVillageTransmission(0), _index(index)
													  
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
	serializeAttribute("herders", _herders.size());
	int totalAnimals = 0;
	for(std::list<Herder*>::iterator it=_herders.begin(); it!=_herders.end(); it++)
	{
		totalAnimals += (*it)->getHerdSize();
	}
	serializeAttribute("total animals", totalAnimals); 
	serializeAttribute("in village transmission", _inVillageTransmission);
	serializeAttribute("out village transmission", _outVillageTransmission);

	int knownCells = 0;
	for(std::list<Herder*>::iterator it=_herders.begin(); it!=_herders.end(); it++)
	{
		Herder & herder = **it;
		Engine::Raster knowledge = _world->getDynamicRasterStr(herder.getKnowledgeMap());

		Engine::Point2D<int> index(0,0);
		for(index._x=0; index._x<_world->getOverlapBoundaries()._size._x; index._x++)
		{
			for(index._y=0; index._y<_world->getOverlapBoundaries()._size._y; index._y++)
			{
				int value = knowledge.getValue(index);
				if(value>-1 && value<3)
				{
					knownCells++;
				}
			}
		}
	}
	serializeAttribute("known 3-year cells", knownCells);
}

}

