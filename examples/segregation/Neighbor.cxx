
#include "Neighbor.hxx"
#include <typedefs.hxx>

namespace Segregation
{
Neighbor::Neighbor( const std::string & id ) : Agent(id), _friendlyPercentage(0.5f), _needsToMove(false), _neighborDistance(1.0f), _maxMovingDistance(1.0f)
{
}

Neighbor::~Neighbor()
{
}

void Neighbor::updateKnowledge()
{
	Engine::AgentsVector neighbors = getWorld()->getNeighbours( this, _neighborDistance );
	int numNeighbors = neighbors.size();
	int friendlyNeighbors = 0;
	for(Engine::AgentsVector::iterator it=neighbors.begin(); it!=neighbors.end(); it++)
	{
		if((*it)->isType(getType()))
		{
			friendlyNeighbors++;
		}
	}
	if(numNeighbors==0 || (float)friendlyNeighbors/(float)numNeighbors<=_friendlyPercentage)
	{
		_needsToMove = false;
		return;
	}
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
			if(!getWorld()->checkPosition(index) || !getWorld()->getBoundaries().contains(index) || index.isEqual(_position))
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
		_position = possiblePositions[0];
	}
}

void Neighbor::updateState()
{
	updateKnowledge();
	if(_needsToMove)
	{
		moveHome();
	}
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

