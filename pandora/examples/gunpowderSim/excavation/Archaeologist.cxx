
#include "Archaeologist.hxx"

#include "Exceptions.hxx"
#include "Statistics.hxx"
#include "World.hxx"
#include <cstring>

namespace BattleSim
{

Archaeologist::Archaeologist( const std::string & id, bool horizontalMovement, int skill ) : Agent(id), _bulletsGathered(0), _horizontalMovement(horizontalMovement), _skill(skill)																							
{
}

Archaeologist::Archaeologist( const ArchaeologistPackage & package ) : Agent(package._id)
{
	_position = package._position;
	_bulletsGathered = package._bulletsGathered;
	_horizontalMovement = package._horizontalMovement;
	_skill = package._skill;
}

Archaeologist::~Archaeologist()
{
}

void Archaeologist::move()
{
	if(_horizontalMovement)
	{
		_position._x++;
	}
	else
	{
		_position._y++;
	}
}

void Archaeologist::collectFindings()
{
	// as each cell is 0.5 meters, the archaologist checks 1.5 meters (left, right and center of his/her direction)
	for(int i=-1; i<2; i++)
	{
		Engine::Point2D<int> posToCheck(_position);
		if(_horizontalMovement)
		{
			posToCheck._y = _position._y+i;
		}
		else
		{
			posToCheck._x = _position._x+i;
		}
		if(!_world->checkPosition(posToCheck))
		{
			continue;
		}
		int numBullets = _world->getValue("degradedRecord", posToCheck);
		int collectedBullets = 0;
		for(int i=0; i<numBullets; i++)
		{
			if(_world->getStatistics().getUniformDistValue(0,100)<_skill)
			{
				collectedBullets++;
			}
		}
		int pastCollectedBullets = _world->getValue("collectedBullets", posToCheck);
		_world->setMaxValue("collectedBullets", posToCheck, pastCollectedBullets + collectedBullets);
		_world->setValue("collectedBullets", posToCheck, pastCollectedBullets + collectedBullets);

		int exploredRasters = _world->getValue("exploredRasters", posToCheck);		
		_world->setMaxValue("exploredRasters", posToCheck, exploredRasters+1);
		_world->setValue("exploredRasters", posToCheck, exploredRasters+1);
		//std::cout << this << " collected bullets in pos: " << posToCheck << ":" << collectedBullets << " of: " << numBullets << std::endl; 
	}
}

void Archaeologist::step()
{
	bool finished = false;
	while(!finished)
	{
		collectFindings();
		move();	
		if(!_world->checkPosition(_position))
		{
			finished = true;
		}
	}
}

void * Archaeologist::createPackage()
{
	ArchaeologistPackage * package = new ArchaeologistPackage;
	memcpy(&package->_id, _id.c_str(), sizeof(char)*_id.size());
	package->_id[_id.size()] = '\0';
	package->_position = _position;
	package->_bulletsGathered = _bulletsGathered;
	package->_horizontalMovement = _horizontalMovement;
	package->_skill = _skill;
	
	return package;
}

void Archaeologist::store()
{
	storeAttribute("bullets gathered", _bulletsGathered);
	storeAttribute("horizontal movement", _horizontalMovement);
	storeAttribute("skill", _skill);
}

} // namespace BattleSim

