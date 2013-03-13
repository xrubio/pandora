#include "Village.hxx"
#include "Herder.hxx"

namespace GujaratCellphones
{

Village::Village(const std::string & id) : Agent(id)
{
}

Village::~Village()
{
}

void Village::updateState()
{
}

void Village::setShareKnowledge( int shareKnowledge ) 
{
	_shareKnowledge = shareKnowledge;
}

int Village::getShareKnowledge()
{
	return _shareKnowledge;
}

void Village::addHerder(Herder * herder)
{
	_herders.push_back(herder);
	herder->setVillage(this);
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

Herder* Village::getRandomHerder( const std::string & id) 
{
	int r = Engine::GeneralState::statistics().getUniformDistValue(0, _herders.size() - 2);
	int counter = 0;
	for(std::list<Herder*>::iterator it=_herders.begin(); it!=_herders.end(); it++)
	{
		if ((*it)->getId() != id) 
		{
			if (counter == r) 
			{
				return (*it);
			}
			else 
			{
				++counter;
			}
		}
	}
}

void Village::registerAttributes()
{
	registerIntAttribute("herders");
	registerIntAttribute("total animals");
	registerIntAttribute("share knowledge");
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
	serializeAttribute("share knowledge", _shareKnowledge);
}

}

