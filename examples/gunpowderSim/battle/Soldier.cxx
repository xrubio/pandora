
#include "Soldier.hxx"
#include "Bullet.hxx"

#include <Exception.hxx>
#include <World.hxx>
#include <cstring>
#include <Statistics.hxx>
#include <GeneralState.hxx>

namespace BattleSim
{

Soldier::Soldier( const std::string & id, const int & rank, const int & cohesionRating, const int & cohesionDistance, const int & accuracy, const int & reloadingTime ) : Agent(id), _isBlueSide(isType("blueSoldier")), _rank(rank), _stress(0), _threshold(1000), _fireDistance(160), _cohesionRating(cohesionRating), _cohesionDistance(cohesionDistance), _moving(true), _routing(false), _reloadingTime(reloadingTime), _timeUntilLoaded(0), _accuracy(accuracy), _delayBeforeFirstFire(0)
{
}

Soldier::~Soldier()
{
}

void Soldier::move()
{
	if(_rank==1)
	{
		if(_isBlueSide)
		{
			// if an enemy is within fire Distance, stop
			Engine::Point2D<int> front(_position._x, 0);
			// we check 5 meters after firing distance, in order to allow read lines to fire
			for(front._y=_position._y+_fireDistance-10; front._y>_position._y; front._y--)
			{
				Engine::AgentsVector agents = _world->getAgent(front);
				if(agents.size()==0)
				{
					continue;
				}
				Agent * agent = agents.at(0).get();
				if(agent->exists() && agent->isType("redSoldier"))
				{
					_moving = false;
					//std::cout << "unit: " << this << " in rank: " << _rank << " stops" << std::endl;
					return;
				}
			}
			Engine::Point2D<int> newPos = _position;
			newPos._y++;
			if(_world->checkPosition(newPos))
			{	
				//std::cout << "unit: " << this << " in rank: " << _rank << " moving" << std::endl;
				_position = newPos;
			}
		}
		else
		{
			Engine::Point2D<int> front(_position._x, 0);
			// we check 5 meters after firing distance, in order to allow read lines to fire
			for(front._y=_position._y-_fireDistance+10; front._y<_position._y; front._y++)
			{	
				Engine::AgentsVector agents = _world->getAgent(front);
				if(agents.size()==0)
				{
					continue;
				}
				Agent * agent = agents.at(0).get();
				if(agent->exists() && agent->isType("blueSoldier"))
				{
					_moving = false;
					//std::cout << "unit: " << this << " in rank: " << _rank << " stops" << std::endl;
					return;
				}
			}
			Engine::Point2D<int> newPos = _position;
			newPos._y--;
			if(_world->checkPosition(newPos))
			{
				//std::cout << "unit: " << this << " in rank: " << _rank << " moving" << std::endl;
				_position = newPos;
			}
		}
	}
	else
	{
		Engine::Point2D<int> forwardPos(_position._x, _position._y);
		int advance = 1;
		if(!_isBlueSide)
		{
			advance = -1;
		}
		// 5 meters forward
		for(int i=1; i<11; i++)
		{
			Engine::Point2D<int> forwardPos(_position._x, _position._y+advance*i);
			Engine::AgentsVector forwardAgents = _world->getAgent(forwardPos);
			if(forwardAgents.size()==0)
			{
				continue;
			}
			Agent * forward = forwardAgents.at(0).get();
			// if you found a soldier in front of you
			if(forward->isType(getType()))
			{
				Soldier * forwardSoldier = (Soldier*)forward;
				if(forwardSoldier->isMoving())
				{
					Engine::Point2D<int> newPos = _position;
					newPos._y += advance;
					if(_world->checkPosition(newPos))
					{
						//std::cout << "unit: " << this << " in rank: " << _rank << " moving" << std::endl;
						_position = newPos;
					}
				}
				else
				{
					//std::cout << "unit: " << this << " in rank: " << _rank << " stops" << std::endl;
					_moving = false;
				}
				return;
			}
		}
	}
}

void Soldier::fire()
{
	// muskets failing to fire
	/*
	if(_world->getStatistics().getUniformDistValue(1,10)<4)
	{
		return;
	}
	*/

	Engine::Point2D<int> bulletPosition = _position;
	std::ostringstream oss;
	if(_isBlueSide)
	{
		bulletPosition._y++;			
		oss << "blueBullet_" << rand();
	}
	else
	{
		bulletPosition._y--;
		oss << "redBullet_" << rand();
	}
	//std::cout << this << " firing!" << std::endl;
	
	Bullet * firedBullet = new Bullet(oss.str(), _accuracy);
	firedBullet->setPosition(bulletPosition);
	_world->addAgent(firedBullet);
	firedBullet->setMuzzleVelocity(Engine::GeneralState::statistics().getNormalDistValueMinMax(50, 450));
	
	_timeUntilLoaded = _reloadingTime;
}

void Soldier::updateStress()
{
	std::string enemyType = "redSoldier";
	std::string friendType = "blueSoldier";
	if(!_isBlueSide)
	{
		enemyType = "blueSoldier";
		friendType = "redSoldier";
	}
	float globalThreat = 0.0f;

	Engine::AgentsVector enemies = _world->getNeighbours(this, _fireDistance*1.5f, enemyType);
	//std::cout << this << " number of enemies: " << enemies.size() << std::endl;
	for(Engine::AgentsVector::iterator it=enemies.begin(); it!=enemies.end(); it++)
	{
		Soldier * enemy = (Soldier*)(*it).get();
		if(!enemy->isRouting())
		{
			//std::cout << "\t" << this << " will add stress: " << enemy->getThreatRating(_position.distance(enemy->getPosition())) << " due to enemy: " << enemy << " at distance: " << _position.distance(enemy->getPosition()) << std::endl;
			if(enemy->firedThisStep())
			{
				globalThreat += 1.5f*enemy->getThreatRating(_position.distance(enemy->getPosition()));
			}
			else
			{
				globalThreat += enemy->getThreatRating(_position.distance(enemy->getPosition()));
			}
		}
		// routed enemies benefits threat
		/*
		else
		{
			//std::cout << "\t" << this << " will remove stress: " << enemy->getThreatRating(_position.distance(enemy->getPosition())) << " due to routed enemy: " << enemy << " at distance: " << _position.distance(enemy->getPosition()) << std::endl;
			globalThreat -= enemy->getThreatRating(_position.distance(enemy->getPosition()));
		}
		*/
	}
	int globalCohesion = 0;	
	Engine::AgentsVector friends = _world->getNeighbours(this, _cohesionDistance, friendType);
	
	//std::cout << this << " number of friends: " << friends.size() << " at cohesion distance: " << _cohesionDistance << std::endl;
	for(Engine::AgentsVector::iterator it=friends.begin(); it!=friends.end(); it++)
	{
		Soldier * friendly = (Soldier*)(*it).get();
		Engine::Point2D<int> friendPosition = friendly->getPosition();
		// if in the same column
		if(friendPosition._x==_position._x)
		{
			if(_isBlueSide)
			{
				// if behind
				if(friendPosition._y<_position._y)
				{
					if(!friendly->isRouting())
					{
						//std::cout << "\t" << this << " will remove 2 stress due to friend: " << friendly << std::endl;
						globalCohesion += 1;
					}
					/*
					else
					{
						//std::cout << "\t" << this << " will add 2 stress due to routing friend: " << friendly << std::endl;
						globalThreat += 5;
					}
					*/
				}
				else
				{
					if(!friendly->isRouting())
					{
						//std::cout << "\t" << this << " will remove 1 stress due to friend: " << friendly << std::endl;
						globalCohesion += 1;
					}
					/*
					else
					{
						//std::cout << "\t" << this << " will add 1 stress due to routing friend: " << friendly << std::endl;
						globalThreat += 1;
					}
					*/
				}
			}
			else
			{	
				if(friendPosition._y>_position._y)
				{
					if(!friendly->isRouting())
					{
						//std::cout << "\t" << this << " will remove 2 stress due to friend: " << friendly << std::endl;
						globalCohesion += 1;
					}
					/*
					else
					{
						//std::cout << "\t" << this << " will add 2 stress due to routing friend: " << friendly << std::endl;
						globalThreat += 5;
					}
					*/
				}
				else
				{
					if(!friendly->isRouting())
					{
						//std::cout << "\t" << this << " will remove 1 stress due to friend: " << friendly << std::endl;
						globalCohesion += 1;
					}
					/*
					else
					{
						//std::cout << "\t" << this << " will add 1 stress due to routing friend: " << friendly << std::endl;
						globalThreat += 1;
					}
					*/
				}
			}
		}
		else
		{
			if(!friendly->isRouting())
			{
				//std::cout << "\t" << this << " will remove 1 stress due to friend: " << friendly << std::endl;
				globalCohesion += 1;
			}
			/*
			else
			{
				//std::cout << "\t" << this << " will add 1 stress due to routing friend: " << friendly << std::endl;
				globalCohesion += 1;
			}
			*/
		}
	}
	//int finalGlobalCohesion = globalCohesion*_cohesionRating;
//	globalCohesion *= 3;
	//finalGlobalCohesion /= 100;

	float modifCohesion = (100.0f - float(_cohesionRating)) / 100.0f;
	int diffCohesion = modifCohesion * (globalThreat - globalCohesion);
	//std::cout << this << " stress: " << _stress << " cohesion: " << globalCohesion << " diffCohesion: " << diffCohesion << " threat: " << globalThreat << " final value: " << _stress + diffCohesion << std::endl;
	_stress = std::max(0, _stress + diffCohesion);

//	int finalGlobalCohesion = _world->getStatistics().getUniformDistValue(0, globalCohesion);
//	int finalGlobalThreat = _world->getStatistics().getUniformDistValue(0, globalThreat);

//	_stress = std::max(0, _stress + (int)globalThreat - globalCohesion);
//	_stress = std::max(0, _stress + finalGlobalThreat - finalGlobalCohesion);

	//std::cout << this << " stress: " << _stress << " global threat: " << globalThreat << " and global cohesion: " << globalCohesion << " with cohesion rating: " << _cohesionRating << " num. near friends: " << friends.size() << " and num.enemies: " << enemies.size() << std::endl;
	//std::cout << this << " stress: " << _stress << " global threat: " << finalGlobalThreat << "/" << globalThreat << " and global cohesion: " << finalGlobalCohesion << "/" << globalCohesion << " with cohesion rating: " << _cohesionRating << " num. near friends: " << friends.size() << " and num.enemies: " << enemies.size() << std::endl;
}

void Soldier::rout()
{
	if(!_routing)
	{
		return;
	}
	
	Engine::Point2D<int> newPos = _position;
	newPos._y--;

	if(!_isBlueSide)
	{
		newPos._y = _position._y+1;
	}
	// there's another soldier rearguard, check against cohesion rating; if ok, it won't rout
	Engine::AgentsVector agents = _world->getAgent(newPos);
	if(agents.size()==0)
	{		
		if(_world->checkPosition(newPos))
		{
			_position = newPos;
		}
		else
		{
			remove();
		}
		return;
	}
	Engine::Agent * agent = agents.at(0).get();
	if(agent->isType(getType()))
	{
		if(Engine::GeneralState::statistics().getUniformDistValue(0, 100)<_cohesionRating)
		{
			_stress = _threshold*0.9f;
			Soldier * soldier = (Soldier*)agent;
			soldier->addStress(20);
			_routing = false;
			return;
		}
		else
		{
			// if not, it will check a hole; if it is already occupied, it won't move; in the contrary, it will displace to the another position
			if(Engine::GeneralState::statistics().getUniformDistValue(0, 1)==0)
			{
				newPos._x--;
			}
			else
			{
				newPos._x++;
			}
			Engine::AgentsVector agents2 = _world->getAgent(newPos);
			if(agents2.size()==0)
			{
				if(_world->checkPosition(newPos))
				{
					_position = newPos;
				}
				else
				{
					remove();
				}
			}
			else
			{
				Engine::Agent * agent2 = agents.at(0).get();
				if(agent2->isType(getType()))
				{
					_stress = _threshold*0.9f;
					Soldier * soldier = (Soldier*)agent2;
					soldier->addStress(20);
					_routing = false;
					return;				
				}
			}
		}
	}
	else
	{
		if(_world->checkPosition(newPos))
		{
			_position = newPos;
		}
		else
		{
			remove();
		}
	}
}

void Soldier::casualty()
{
	// we disperse the stress of seeing a casualty unit to other soldiers near it.
	Engine::AgentsVector affectedSoldier = _world->getNeighbours(this, _cohesionDistance, getType());
	for(Engine::AgentsVector::iterator it=affectedSoldier.begin(); it!=affectedSoldier.end(); it++)
	{
		Agent * agent = (*it).get();
		if(agent->isType("blueSoldier") || agent->isType("redSoldier"))
		{
			Soldier * soldier = (Soldier*)(*it).get();
			soldier->addStress(100-soldier->getCohesionRating());
		}
	}
	remove();
}

void Soldier::updateState()
{
	if(!_exists)
	{
		casualty();
		return;
	}
	if(!_routing)
	{
		if(_moving)
		{
			move();
		}
		else
		{
			if(_delayBeforeFirstFire==0 && _timeUntilLoaded==0)
			{
				fire();
			}
			else
			{
				if(_timeUntilLoaded>0)
				{
					_timeUntilLoaded--;
				}
				if(_delayBeforeFirstFire>0)
				{
					_delayBeforeFirstFire--;
				}
			}
		}
	}

	updateStress();	
	if(_stress>_threshold)
	{
		_routing = true;
		//std::cout << this << " routing with stress: " << _stress << std::endl;
	}
	rout();
}

float Soldier::getThreatRating( const int & distance )
{
	if(distance>(_fireDistance*1.5f))
	{
		std::stringstream oss;
		oss << "Soldier::getThreatRating error! distance: " << distance;
		throw Engine::Exception(oss.str());
		return 0;
	}
	// if out of effective distance, but between that and the double, minimum increase
	// TODO multiply rating by danger of the weapon (with muskets, always 1.0)
	float rating = float(_fireDistance*1.5f-distance)/float(_fireDistance*1.5f);	
	// at least 0.1 due to being near the soldier
	//return std::max(0.1f, rating);
	return rating;
}

void Soldier::addStress( const int & value )
{
	if(_routing)
	{
		return;
	}
	_stress += value;
	/*
	if(_stress>_threshold)
	{
		_routing = true;
	}
	*/
}

int Soldier::getCohesionRating()
{
	return _cohesionRating;
}

bool Soldier::isRouting()
{
	return _routing;
}

bool Soldier::isMoving()
{
	return _moving;
}

void Soldier::serialize()
{
	serializeAttribute("stress", _stress);
	serializeAttribute("cohesion rating", _cohesionRating);
	serializeAttribute("cohesion distance", _cohesionDistance);
	serializeAttribute("rank", _rank);
	serializeAttribute("timeUntilLoaded", _timeUntilLoaded);
	serializeAttribute("routing", _routing);
	if(_reloadingTime==_timeUntilLoaded && _delayBeforeFirstFire==0)
	{
		serializeAttribute("fired", true);
	}
	else
	{
		serializeAttribute("fired", false);
	}
}

void Soldier::registerAttributes()
{
	registerIntAttribute("stress");
	registerIntAttribute("cohesion rating");
	registerIntAttribute("cohesion distance");
	registerIntAttribute("rank");
	registerIntAttribute("timeUntilLoaded");
	registerIntAttribute("routing");
	registerIntAttribute("fired");
}
	
void Soldier::setDelayBeforeFirstFire( const int & delayBeforeFirstFire )
{
	_delayBeforeFirstFire = delayBeforeFirstFire;
}
	
bool Soldier::firedThisStep()
{	
	if(_reloadingTime==_timeUntilLoaded && _delayBeforeFirstFire==0)
	{
		return true;
	}
	return false;
}

} // namespace BattleSim

