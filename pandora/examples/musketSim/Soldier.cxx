
#include <Soldier.hxx>

#include <Bullet.hxx>

#include <Exceptions.hxx>
#include <Statistics.hxx>
#include <World.hxx>
#include <cstring>
#include <GeneralState.hxx>
#include <Logger.hxx>

namespace MusketSim
{

Soldier::Soldier( const std::string & id, bool isBlueSide, const int & rank, const int & cohesionRating, const int & cohesionDistance, const int & accuracy ) : Agent(id), _isBlueSide(isBlueSide), _rank(rank), _stress(0), _threshold(1000), _fireDistance(160), _cohesionRating(cohesionRating), _cohesionDistance(cohesionDistance), _moving(true), _routing(false), _reloadingTime(45), _timeUntilLoaded(0), _accuracy(accuracy), _delayBeforeFirstFire(0)
{
}

Soldier::~Soldier()
{
}

void Soldier::move()
{
	std::stringstream logName;
	logName << "agents_" << _world->getId();

	if(_rank==1)
	{
		if(_isBlueSide)
		{
			// if an enemy is within fire Distance, stop
			Engine::Point2D<int> front(_position._x, 0);
			// we check 5 meters after firing distance, in order to allow read lines to fire
			for(front._y=_position._y+_fireDistance-10; front._y>_position._y; front._y--)
			{
				std::vector<Agent*> agents = _world->getAgent(front);
				for(int i=0; i<agents.size(); i++)
				{
					Soldier * agent = (Soldier*)agents.at(i);
					if(agent && agent->exists() && !agent->isBlueSide())
					{
						_moving = false;
						log_DEBUG( logName.str(), this << " in rank: " << _rank << " stops");
						return;
					}
				}
			}
			Engine::Point2D<int> newPos = _position;
			newPos._y++;
			if(_world->checkPosition(newPos))
			{	
				log_DEBUG( logName.str(), this << " in rank: " << _rank << " moving to: " << newPos);
				_position = newPos;
			}
		}
		else
		{
			Engine::Point2D<int> front(_position._x, 0);
			// we check 5 meters after firing distance, in order to allow read lines to fire
			for(front._y=_position._y-_fireDistance+10; front._y<_position._y; front._y++)
			{
				std::vector<Agent*> agents = _world->getAgent(front);
				for(int i=0; i<agents.size(); i++)
				{
					Soldier * agent = (Soldier*)agents.at(i);
					if(agent && agent->exists() && agent->isBlueSide())
					{
						_moving = false;
						log_DEBUG( logName.str(), this << " in rank: " << _rank << " stops");
						return;
					
					}
				}
			}
			Engine::Point2D<int> newPos = _position;
			newPos._y--;
			if(_world->checkPosition(newPos))
			{
				log_DEBUG( logName.str(), this << " in rank: " << _rank << " moving to: " << newPos);
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
			
		bool friendForward = false;
		// 5 meters forward
		for(int i=1; i<11; i++)
		{
			Engine::Point2D<int> forwardPos(_position._x, _position._y+advance*i);
			std::vector<Agent*> forwards = _world->getAgent(forwardPos);
			log_DEBUG( logName.str(), this << " in rank: " << _rank << " forward friends: " << forwards.size() << " in forward pos: " << forwardPos);
			for(int i=0; i<forwards.size(); i++)
			{
				Agent * forward = forwards.at(i);
				// if you found a soldier in front of you
				if(forward && forward->isType(getType()))
				{
					Soldier * forwardSoldier = (Soldier*)forward;
					if(forwardSoldier->isMoving())
					{
						Engine::Point2D<int> newPos = _position;
						newPos._y += advance;
						if(_world->checkPosition(newPos))
						{
							log_DEBUG( logName.str(), this << " in rank: " << _rank << " moving to: " << newPos);
							_position = newPos;
							return;
						}
					}
					else
					{
						log_DEBUG( logName.str(), this << " in rank: " << _rank << " stops");
						_moving = false;
					}
					friendForward = true;
				}
			}
		}
		if(!friendForward)
		{
			log_DEBUG( logName.str(), this << " in rank: " << _rank << " stops");
			_moving = false;
		}
	}
}

void Soldier::fire()
{
	std::stringstream logName;
	logName << "agents_" << _world->getId();

	// muskets failing to fire
	/*
	if(_world->getStatistics().getUniformDistValue(1,10)<4)
	{
		return;
	}
	*/

	Engine::Point2D<int> bulletPosition = _position;
	std::ostringstream oss;
	oss << "Bullet_" << rand();
	Bullet * firedBullet = new Bullet(oss.str(), _isBlueSide, _accuracy);
	if(_isBlueSide)
	{
		bulletPosition._y++;			
	}
	else
	{
		bulletPosition._y--;
	}
	//std::cout << this << " firing!" << std::endl;
	log_DEBUG( logName.str(), this << " in rank: " << _rank << " firing bullet: " << firedBullet);
	
	firedBullet->setPosition(bulletPosition);
	_world->addAgent(firedBullet);
	firedBullet->setMuzzleVelocity(Engine::GeneralState::statistics().getNormalDistValue(50, 450));
	
	_timeUntilLoaded = _reloadingTime;
}

void Soldier::updateStress()
{
	std::stringstream logName;
	logName << "stress_" << _world->getId();
	float globalThreat = 0.0f;

	log_DEBUG( logName.str(), this << " evaluating stress");

	Engine::World::AgentsList soldiers = _world->getNeighbours(this, _fireDistance*1.5f, "Soldier");
	for(Engine::World::AgentsList::iterator it=soldiers.begin(); it!=soldiers.end(); it++)
	{
		Soldier * enemy = (Soldier*)(*it);
		// friends
		if(enemy->isBlueSide()==_isBlueSide)
		{
			continue;
		}
		if(!enemy->isRouting())
		{
			log_DEBUG( logName.str(), "\t" << this << " will add stress: " << enemy->getThreatRating(_position.distance(enemy->getPosition())) << " due to enemy: " << enemy << " at distance: " << _position.distance(enemy->getPosition()));
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
	//std::cout << this << " number of friends: " << friends.size() << " at cohesion distance: " << _cohesionDistance << std::endl;
	for(Engine::World::AgentsList::iterator it=soldiers.begin(); it!=soldiers.end(); it++)
	{
		Soldier * friendly = (Soldier*)(*it);	
		// enemy
		if(friendly->isBlueSide()!=_isBlueSide)
		{
			continue;
		}
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
						log_DEBUG( logName.str(), "\t" << this << " will remove 1 stress due to friend: " << friendly);
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
						log_DEBUG( logName.str(), "\t" << this << " will remove 1 stress due to friend: " << friendly);
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
						log_DEBUG( logName.str(), "\t" << this << " will remove 1 stress due to friend: " << friendly);
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
						log_DEBUG( logName.str(), "\t" << this << " will remove 1 stress due to friend: " << friendly);
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
				log_DEBUG( logName.str(), "\t" << this << " will remove 1 stress due to friend: " << friendly);
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

	float modifCohesion = float(_cohesionRating)/100.0f;
	//int diffCohesion = modifCohesion * (globalThreat - globalCohesion);
	int diffCohesion = globalThreat - (modifCohesion * globalCohesion);
	//std::cout << this << " stress: " << _stress << " cohesion: " << globalCohesion << " diffCohesion: " << diffCohesion << " threat: " << globalThreat << " final value: " << _stress + diffCohesion << std::endl;
	_stress = std::max(0, _stress + diffCohesion);
	log_DEBUG( logName.str(), this << " new stress: " << _stress << " with modif cohesion: " << modifCohesion << " diff: " << diffCohesion << " globalThreat: " << globalThreat<< " and global cohesion: " << globalCohesion);

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
	std::vector<Agent*> agents = _world->getAgent(newPos);
	for(int i=0; i<agents.size(); i++)
	{
		Agent * agent = agents.at(i);

		if(agent && agent->isType(getType()))
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
				std::vector<Agent*> agents2 = _world->getAgent(newPos);	
				for(int j=0; j<agents2.size(); j++)
				{
					Agent * agent2 = agents.at(j);
					if(!agent2)
					{
						if(_world->checkPosition(newPos))
						{
							_position = newPos;
							return;
						}	
						else
						{
							remove();
							return;
						}
					}
					else
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
	}
	if(_world->checkPosition(newPos))
	{
		_position = newPos;
	}
	else
	{
		remove();
	}
	// we disperse the stress of seeing the routing unit to other soldiers near it.
	// TODO no! it is done in updateStress of the other agents
	/*
	Engine::World::AgentsList affectedSoldier = _world->getNeighbours(this, 10, getType());
	for(Engine::World::AgentsList::iterator it=affectedSoldier.begin(); it!=affectedSoldier.end(); it++)
	{
		Agent * agent = (*it);
		if(agent->isType("blueSoldier") || agent->isType("redSoldier"))
		{
			Soldier * soldier = (Soldier*)(*it);
			soldier->addStress((100-soldier->getCohesionRating())/2);
		}
	}
	*/
}

void Soldier::casualty()
{
	// we disperse the stress of seeing a casualty unit to other soldiers near it.
	Engine::World::AgentsList affectedSoldier = _world->getNeighbours(this, _cohesionDistance, getType());
	for(Engine::World::AgentsList::iterator it=affectedSoldier.begin(); it!=affectedSoldier.end(); it++)
	{
		Agent * agent = (*it);
		if(agent->isType("Soldier"))
		{
			Soldier * soldier = (Soldier*)(*it);
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

void Soldier::registerAttributes()
{
	registerIntAttribute("stress");
	registerIntAttribute("cohesion rating");
	registerIntAttribute("cohesion distance");
	registerIntAttribute("rank");
	registerIntAttribute("timeUntilLoaded");
	registerIntAttribute("routing");
	registerIntAttribute("fired");
	registerIntAttribute("isBlueSide");
}

void Soldier::serialize()
{
	serializeAttribute("stress", _stress);
	serializeAttribute("cohesion rating", _cohesionRating);
	serializeAttribute("cohesion distance", _cohesionDistance);
	serializeAttribute("rank", _rank);
	serializeAttribute("timeUntilLoaded", _timeUntilLoaded);
	serializeAttribute("routing", _routing);
	serializeAttribute("isBlueSide", _isBlueSide);
	if(_reloadingTime==_timeUntilLoaded && _delayBeforeFirstFire==0)
	{
		serializeAttribute("fired", true);
	}
	else
	{
		serializeAttribute("fired", false);
	}
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

} // namespace MusketSim

