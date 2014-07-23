
#include "Neighbor.hxx"
#include <typedefs.hxx>
#include <iostream>
#include <Logger.hxx>
#include <GeneralState.hxx>
#include <sstream>

namespace Segregation
{
Neighbor::Neighbor( const std::string & id ) : Agent(id), _friendlyPercentage(0.5f), _needsToMove(false), _neighborDistance(1.0f), _maxMovingDistance(1.0f), _friends(0), _neighbors(0)
{
}

Neighbor::~Neighbor()
{
}

void Neighbor::updateKnowledge()
{
	Engine::AgentsVector neighbors = getWorld()->getNeighbours( this, _neighborDistance );
	_neighbors = neighbors.size();
    _friends= 0;
    

	for(Engine::AgentsVector::iterator it=neighbors.begin(); it!=neighbors.end(); it++)
	{
		if((*it)->isType(getType()))
		{
			_friends++;
		}
	}
	if(_neighbors==0 || (float)_friends/(float)_neighbors>=_friendlyPercentage)
	{
		_needsToMove = false;
		return;
	}    
    std::stringstream logName;
	logName << "friends";
    log_INFO( logName.str(), this << " will move, neighbors: " << _neighbors<< " friendly: " << _friends << " percentage: " << (float)_friends/(float)_neighbors << " required: " << _friendlyPercentage);
	_needsToMove = true;
}

void Neighbor::moveHome()
{
	Engine::Point2D<int> index;
	std::vector< Engine::Point2D<int> > possiblePositions;
	for(index._x=_position._x-_maxMovingDistance; index._x<=_position._x+_maxMovingDistance; index._x++)
	{
		for(index._y=_position._y-_maxMovingDistance; index._y<=_position._y+_maxMovingDistance; index._y++)
		{
			if(!getWorld()->checkPosition(index) || index==_position)
			{
				continue;
			}
			if(_position.distance(index)<=_maxMovingDistance)
			{
				possiblePositions.push_back(index);
			}
		}
	}
	if(possiblePositions.size()!=0)
	{
		std::random_shuffle(possiblePositions.begin(), possiblePositions.end());
        setPosition(possiblePositions[0]);
	}
}

void Neighbor::updateState()
{
	if(_needsToMove)
	{
		moveHome();
	}
}

void Neighbor::registerAttributes()
{
	registerIntAttribute("needs to move");
	registerIntAttribute("friends");
	registerIntAttribute("neighbors");
}

void Neighbor::serialize()
{
	serializeAttribute("needs to move", (int)_needsToMove);
	serializeAttribute("friends", _friends);
	serializeAttribute("neighbors", _neighbors);
}


void Neighbor::setFriendlyPercentage( const float & friendlyPercentage )
{
	_friendlyPercentage = friendlyPercentage;
}

void Neighbor::setNeighborDistance( const float & neighborDistance )
{
	_neighborDistance = neighborDistance;
}

void Neighbor::setMaxMovingDistance( const float & maxMovingDistance )
{
	_maxMovingDistance = maxMovingDistance;
}

} // namespace Segregation

