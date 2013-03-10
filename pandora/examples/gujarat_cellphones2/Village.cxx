
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

void Village::updateState()
{
}

void Village::registerAttributes()
{
	registerIntAttribute("herders");
	registerIntAttribute("total animals");
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
}


/*
std::vector<std::string> Village::getCitizens() {
	return _citizens;
}

int Village::getId() {
	return _id;
}

Engine::Point2D<int> Village::getLocation() {
	return _location;
}

bool Village::isCitizenOfVillage(const std::string & id)
{
	for (int i = 0; i < _citizens.size(); ++i)
	{
		if (_citizens.at(i).compare(id)==0)
		{
			return true;
		}
	}
	return false;
}

void Village::setLocation(const Engine::Point2D<int> & location )
{
	_location = location;
}
*/
}

