
#include "SugarAgent.hxx"
#include "World.hxx"
#include "Exceptions.hxx"

#include <algorithm>
#include <cstring>

#include "Statistics.hxx"
#include "SugarWorld.hxx"

namespace SUG
{

SugarAgent::SugarAgent( const std::string & id ) 
	: Agent(id)	
	, _exists(true)
	, _state(ALIVE)
	//, _world(0)
	, _sugar(0)
	, _initialEndowment(0)
	, _vision(0)
	, _age(0)
	, _deathAge(0)
	, _metabolism(0)
	, _beginChildBearingAge(0)
	, _beginPausiaAge(0)
	, _sex((SexCategories)0)
{
	_position._x = 0;
	_position._y = 0;		
	_world = (SugarWorld*)0;
}

SugarAgent::SugarAgent( const std::string & id, SugarWorld * w, Engine::Point2D<int> position) 
	: Agent(id)	
	, _exists(true)
	, _state(ALIVE)
	, _world(w)
	, _age(0)
{
// REQUIRE: Agent::_config defined
	_position = position;
	_sugar					= _world->getStatistics().getUniformDistValue(_world->_config._agentMinEndowment
																		,_world->_config._agentMaxEndowment);
	_initialEndowment		= _sugar;
	_vision					= _world->getStatistics().getUniformDistValue(_world->_config._agentMinVision,_world->_config._agentMaxVision);
	_deathAge				= _world->getStatistics().getUniformDistValue(_world->_config._agentMinDeathAge
																		, _world->_config._agentMaxDeathAge);
	_metabolism				= _world->getStatistics().getUniformDistValue(_world->_config._agentMinMetabolism
																		, _world->_config._agentMaxMetabolism);
	_beginChildBearingAge	= _world->getStatistics().getUniformDistValue(_world->_config._agentMinBeginChildBearing
																		, _world->_config._agentMaxBeginChildBearing);	
	_sex					= (SexCategories) _world->getStatistics().getUniformDistValue(0,1);
	
	if (_sex)
	{
		_beginPausiaAge	= _world->getStatistics().getUniformDistValue(_world->_config._agentMinPausiaAgeMen
																	, _world->_config._agentMaxPausiaAgeMen);	
	}
	else
	{
		_beginPausiaAge	= _world->getStatistics().getUniformDistValue(_world->_config._agentMinPausiaAgeWomen
																	, _world->_config._agentMaxPausiaAgeWomen);
	}
}

SugarAgent::SugarAgent( const SugarAgentPackage & package ) : Agent(package._id)	
{  
	_position				= package._position;	
	_sugar					= package._sugar;
	_initialEndowment		= package._initialEndowment;
	_exists					= package._exists;
	_state					= package._state;
	_age					= package._age;
	_vision					= package._vision;
	_deathAge				= package._deathAge;
	_metabolism				= package._metabolism;
	_beginChildBearingAge	= package._beginChildBearingAge;
	_beginPausiaAge			= package._beginPausiaAge;
	_sex					= package._sex;
	
	//TODO no faltaria _world???
	
}


SugarAgent::SugarAgent( const std::string & id 
						, Engine::Point2D<int> position
						, bool exists
						, SugarAgentState state
						, SugarWorld * world
						, int sugar
						, int initialEndowment
						, int vision
						, int age
						, int deathAge
						, int metabolism
						, int beginChildBearingAge
						, int beginPausiaAge
						, SexCategories sex
) 
	: Agent(id)	
	, _exists(exists)
	, _state(state)
	, _world(world)
	, _sugar(sugar)
	, _initialEndowment(initialEndowment)
	, _vision(vision)
	, _age(age)
	, _deathAge(deathAge)
	, _metabolism(metabolism)
	, _beginChildBearingAge(beginChildBearingAge)
	, _beginPausiaAge(beginPausiaAge)
	, _sex(sex)
{
	_position = position;
}


SugarAgent::~SugarAgent()
{
}


void SugarAgent::step()
{	
	if(!_exists)
	{
		_world->removeAgent(this);
		return;
	}
	move();
	mating();
	metabolism();
	death();
	_age++;
}


void SugarAgent::checkMoveCell(Engine::Point2D<int> & newPosition
					, int *maxValue
					, int *bestDistance
					, int distance
					, std::vector< Engine::Point2D<int> > * possiblePositions)
{
	
	int valueAtNewPosition;
	if (!_world->_config._pollutionDiffusion)
	{
		valueAtNewPosition = _world->getValue("sugar", newPosition);
	}
	else 
	{
		valueAtNewPosition = _world->getValue("sugar", newPosition) / (1+_world->getValue("pollution", _position));
	}

	if(valueAtNewPosition > *maxValue)	// new position better than current
	{
		possiblePositions->clear();
		possiblePositions->push_back(newPosition);
		*maxValue     = valueAtNewPosition;
		*bestDistance = distance;
	}
	// in case of being equal target and new position, check distance
	else if(valueAtNewPosition == *maxValue)
	{
		if (distance < *bestDistance)
		{
			possiblePositions->clear();
			possiblePositions->push_back(newPosition);
			*bestDistance = distance;
		}
		else if (distance == *bestDistance)
		{
			possiblePositions->push_back(newPosition);
		}
	}
}

void SugarAgent::move()
{
	//std::cout << "begin move for agent: " << this << std::endl;
	Engine::Point2D<int> newPosition;
	int distance;
	// looking for cells at vision distance
	int currentValue = _world->getValue("sugar", _position);
	int maxValue;
	if (!_world->_config._pollutionDiffusion)
	{
		maxValue = currentValue;
	}
	else 
	{
		maxValue = currentValue / (1+_world->getValue("pollution", _position));
	}
	int bestDistance = 0;
	std::vector<Engine::Point2D<int> > possiblePositions; // max size 4
	possiblePositions.push_back(_position);
	
/*	
	std::list<int> northTabu;	// PENDENT should be an array<bool> of size _vision, the upper bound
	std::list<int> southTabu;
	std::list<int> eastTabu;
	std::list<int> westTabu;
*/

	// Scan Neighbours and constructing the tabu lists for occupied positions
/*	
	forall b Agent
	{
		if (b._location._x == _location._x && b._location._y != _location._y)
		{
			if (b._location._y < _location._y)
			{
				northTabu.push_back(_location._y - b._location._y);
			}
			else
			{
				southTabu.push_back(b._location._y - _location._y);
			}
		}
		if (b._location._y == _location._y && b._location._x != _location._x)
		{
			if (b._location._x < _location._x)
			{
				westTabu.push_back(_location._x - b._location._x);
			}
			else
			{
				eastTabu.push_back(b._location._x - _location._x);
			}
		}
	}	
*/		
	// loop initializations taking into account cache memory 
	// PENDENT if distance goes from 0 to _vision, check only value for improving value
	// you do not need distance to compare cells with equal resources.
		
	// WEST
	distance	   = _vision;
	newPosition._x = _position._x - distance;
	newPosition._y = _position._y;
	while (distance > 0)
	{                                                                
		// PENDENT ---------------------------------------------------V----- when using tabu lists, remove that calls 
		if(_world->getOverlapBoundaries().isInside(newPosition) && !_world->getAgent(newPosition))
		{
			checkMoveCell(newPosition, &maxValue, &bestDistance, distance, &possiblePositions);
		}
	newPosition._x++;
	distance--;
	}

	// EAST
	distance	   = 0;
	newPosition._x = _position._x;
	newPosition._y = _position._y;
	while (distance <= _vision)
	{
		if(_world->getOverlapBoundaries().isInside(newPosition) && !_world->getAgent(newPosition))
		{
			checkMoveCell(newPosition, &maxValue, &bestDistance, distance, &possiblePositions);
		}
	newPosition._x++;
	distance++;
	}

	// NORTH
	distance       = _vision;
	newPosition._x = _position._x;
	newPosition._y = _position._y - _vision;
	while (distance > 0)
	{
		if(_world->getOverlapBoundaries().isInside(newPosition) && !_world->getAgent(newPosition))
		{
			checkMoveCell(newPosition, &maxValue, &bestDistance, distance, &possiblePositions);
		}
	newPosition._y++;
	distance--;
	}
	
	// SOUTH
	distance        = 0;
	newPosition._x  = _position._x;
	newPosition._y  = _position._y;
	while (distance <= _vision)
	{
		if(_world->getOverlapBoundaries().isInside(newPosition) && !_world->getAgent(newPosition))
		{
			checkMoveCell(newPosition, &maxValue, &bestDistance, distance, &possiblePositions);
		}
	newPosition._y++;
	distance++;
	}

	if(possiblePositions.size()==0)
	{
	// LOG; A well constructed World will not arrive a state that will
	// conduct you here.
	// You will arrive here if bad dimensioning happens.
		std::stringstream oss;
		oss << "SugarAgent::move: agent: " << this << " doesn't have any valid position";
		throw Engine::Exception(oss.str());
	}	
	
	std::random_shuffle(possiblePositions.begin(), possiblePositions.end());
	_position = possiblePositions[0];
	std::cout << "HERE " << _position << std::endl;
	// PENDENT _position = possiblePositions[random(possiblePositions.begin(),possiblePositions.end())];
	maxValue  = _world->getValue("sugar", _position);
	_sugar    = _sugar + maxValue;
	_world->setValue("sugar",_position,0);
	if (_world->_config._pollutionFormation)	// Pollution from Gathering
	{
		_world->setValue("pollution",_position,_world->getValue("pollution", _position) + maxValue*_world->_config._pollutionFormationAlpha);
	}
	possiblePositions.clear();
}



void SugarAgent::metabolism()
{
	int amountMetabolized = _metabolism;
	_sugar = _sugar - _metabolism;
	if(_sugar < 0)
	{
		setState(DEAD);		
	}
	
	if (_world->_config._pollutionFormation)	// Pollution from Metabolize sugar
	{
		_world->setValue("pollution",_position,_world->getValue("pollution", _position) + amountMetabolized*_world->_config._pollutionFormationBeta);
	}	
}


void SugarAgent::setState(SugarAgentState s)
{
	_state = s;
}

// agent DNA={_metabolism,_vision, _maximumAge}
void SugarAgent::crossOver(SugarAgent *x,SugarAgent *y)
{
// PENDENT : extract only 1 random number, 
// consider it as a bit mask, use each bit for each "IF"
	if(_world->getStatistics().getUniformDistValue(0,1))
	{
		_metabolism = x->_metabolism;
	}
	else
	{
		_metabolism = y->_metabolism;
	}
	
	if(_world->getStatistics().getUniformDistValue(0,1))
	{
		_vision = x->_vision;
	}
	else
	{
		_vision = y->_vision;
	}
	
	if(_world->getStatistics().getUniformDistValue(0,1))
	{
		_deathAge = x->_deathAge;
	}
	else
	{
		_deathAge = y->_deathAge;
	}
}


SugarAgent* SugarAgent::matingWithPartner(SugarAgent * a,Engine::Point2D<int> & bornPosition)
{	
	SugarAgent * child = new SugarAgent("agent_"+_world->reserveAgentKey(),_world,bornPosition);
	// Default SugarAgent with stochastic initializations plus the parents' effects.
	child->crossOver(this, a);	
	child->_sugar = a->_initialEndowment/2 + _initialEndowment/2;
	child->_initialEndowment = child->_sugar;
return child;
}


Engine::Point2D<int> SugarAgent::allocateNewBorn(SugarAgent * b)
{	
/*
 * Search for a usable cell contained in the shared space of the agents "this" and its partner "b"
*/	
	
	int n = 0;
	Engine::Point2D<int> v[2*NUMNEIGHBOURS];
	
	v[n] = _position;			// WEST(a)
	v[n]._x--;
	
	if (_world->getOverlapBoundaries().isInside(v[n]) && !_world->getAgent(v[n]))
	{
		n++;	
	}
	
	v[n] = v[n-1];
	v[n]._x++;					// EAST(a)
	v[n]._x++;
	
	if (_world->getOverlapBoundaries().isInside(v[n]) && !_world->getAgent(v[n]))
	{
		n++;	
	}
	
	v[n] = v[n-1];
	v[n]._x--;					// NORTH(a)
	v[n]._y--;
	
	if (_world->getOverlapBoundaries().isInside(v[n]) && !_world->getAgent(v[n]))
	{
		n++;	
	}
	
	v[n] = v[n-1];
	v[n]._y++;					// SOUTH(a)
	v[n]._y++;
	
	if (_world->getOverlapBoundaries().isInside(v[n]) && !_world->getAgent(v[n]))
	{
		n++;	
	}
	
	//------------------------------------------
	
	v[n] = b->_position;			// WEST(a)
	v[n]._x--;
	
	if (b->_world->getOverlapBoundaries().isInside(v[n]) && !b->_world->getAgent(v[n]))
	{
		n++;	
	}
	
	v[n] = v[n-1];
	v[n]._x++;					// EAST(a)
	v[n]._x++;
	
	if (b->_world->getOverlapBoundaries().isInside(v[n]) && !b->_world->getAgent(v[n]))
	{
		n++;	
	}
	
	v[n] = v[n-1];
	v[n]._x--;					// NORTH(a)
	v[n]._y--;
	
	if (b->_world->getOverlapBoundaries().isInside(v[n]) && !b->_world->getAgent(v[n]))
	{
		n++;	
	}
	
	v[n] = v[n-1];
	v[n]._y++;					// SOUTH(a)
	v[n]._y++;
	
	if (b->_world->getOverlapBoundaries().isInside(v[n]) && !b->_world->getAgent(v[n]))
	{
		n++;	
	}
	
	if ( n > 0 )
	{
		return v[b->_world->getStatistics().getUniformDistValue(0,n-1)];
	}
	else
	{
		v[0]._x = -1;
		v[0]._y = -1;
		return v[0];
	}
	
}

void SugarAgent::mating()
{
	if(isFertile())
	{
		std::vector<SugarAgent*> l; // Bounded number of neighbours : 4 --> array/vector
		Engine::Point2D<int> index = _position;
		Engine::Point2D<int> neighPos;
		// Evaluate surroundings. 
		// Count free space for offspring
		int freecells = 0;
		//for (u,v) | |u-i|<=1 && |v-j|<=1 && !(u!=i && v!=j)  
		// PENDENT use loop? unfold loop and use explicitly cells North,South,East,West?	
		for(neighPos._x = index._x-1; neighPos._x <= index._x+1; neighPos._x++)
		{
			for(neighPos._y = index._y-1; neighPos._y <= index._y+1; neighPos._y++)
			{
				if (!(neighPos._x != index._x && neighPos._y != index._y) 
					&& !(neighPos._x == index._x && neighPos._y == index._y))
				{
					SugarAgent *a = (SugarAgent*)_world->getAgent(neighPos);
					if (a)
					{
						if (a->_sex != this->_sex 
						&& a->isFertile())
						{
							l.push_back(a);
						}
					}
				}
			}
		}
		//PENDENT state==DEAD --> fertile=false
		// random shuffle
		int neighIndex = 0;
		while(neighIndex < l.size())
		{
			int i = _world->getStatistics().getUniformDistValue(neighIndex,l.size()-1);
			Engine::Point2D<int> p;
			SugarAgent * a = l[i];			
			p = allocateNewBorn(a); // PENDENT p in {this->homeRange + a.homeRange}
			if (p._x != -1 && p._y != -1)
			{
				SugarAgent * child = matingWithPartner(a,p);
				_world->addAgent(child);
			}			
			l[i] = l[neighIndex];
			l[neighIndex] = a;
			neighIndex++;
		}
	}
}


void SugarAgent::death()
{
	if(checkDeath())
	{
		_exists = false;
		_world->removeAgent(this);
		if(_world->_config._replacementRule)
		{
			_world->replace();
		}
	//return;
	}	
}


bool SugarAgent::starvation()
{
	return _sugar <= 0; // TODO change 0 by minResourcesToLive
}


bool SugarAgent::checkDeath()
{
	return (starvation() || _age > _deathAge);
}


void SugarAgent::serialize()
{
	//serializeAttribute("id", 		 _id);
	//serializeAttribute("position", 	 _position);	
	serializeAttribute("sugar", 	 _sugar);
	//serializeAttribute("exists", 	 _exists);
	serializeAttribute("sex", 		 _sex);
	serializeAttribute("age", 		 _age);	
	serializeAttribute("beginChildBearingAge", _beginChildBearingAge);
	serializeAttribute("beginPausiaAge",       _beginPausiaAge);
	
	serializeAttribute("vision", 	 _vision);
	serializeAttribute("deathAge", 	 _deathAge);
	serializeAttribute("metabolism", _metabolism);
}

void * SugarAgent::createPackage()
{
	SugarAgentPackage * package = new SugarAgentPackage;
	memcpy(&package->_id, _id.c_str(), sizeof(char)*_id.size());
	package->_id[_id.size()] = '\0';
	
	package->_position 	 = _position;
	package->_sugar 	 = _sugar;
	package->_exists 	 = _exists;
	package->_sex 		 = _sex;
	package->_age 		 = _age;
	package->_beginChildBearingAge = _beginChildBearingAge;
	package->_beginPausiaAge       = _beginPausiaAge;
	// ADN
	package->_vision 	 = _vision;
	package->_deathAge 	 = _deathAge;
	package->_metabolism = _metabolism;
	
	return package;
}
/*
std::ostream & operator<<( std::ostream & stream, SugarAgent & agent )
{
	if(agent.getWorld())
	{
		return stream << "id: " << agent.getId() << " pos: " << agent.getPosition() << " population: " << agent.getPopulation() << " and cannibalism: " << agent.getCannibalism() << " exists: " << agent.exists() << " at env: " << agent.getEnvironment() << " located at: " << agent.getEnvironment()->getPosition();
	}
	else
	{
		return stream << "id: " << agent.getId() << " pos: " << agent.getPosition();
	}
}

std::ostream & operator<<( std::ostream & stream, SugarAgent * agent )
{
	if(agent->getWorld())
	{
		return stream << "id: " << agent->getId() << " pos: " << agent->getPosition() <<  " population: " << agent->getPopulation() << " and cannibalism: " << agent->getCannibalism() << " exists: " << agent->exists();
	}
	else
	{
		return stream << "id: " << agent->getId() << " pos: " << agent->getPosition() << " exists: " << agent->exists();
	}
}
*/

} // namespace SUG

