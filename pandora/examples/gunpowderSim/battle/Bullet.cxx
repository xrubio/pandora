
#include "Bullet.hxx"

#include "Exceptions.hxx"
#include "Soldier.hxx"
#include "World.hxx"
#include <cstring>

namespace BattleSim
{

Bullet::Bullet( const std::string & id, const int & impactProbability ) : Agent(id), _isBlueSide(isType("blueBullet")), _velocity(0), _fallingTime(0.0f), _impactProbability(impactProbability)
{
}

Bullet::Bullet( const BulletPackage & package ) : Agent(package._id)
{
	_position = package._position;
	_isBlueSide = package._isBlueSide;
	_impactProbability = package._impactProbability;
}

Bullet::~Bullet()
{
}

void Bullet::step()
{
	int distance = _fallingTime*_velocity;
	// resolution is 0.5 m.
	distance *= 2;
	Engine::Point2D<int> newPos = _position;
	if(_isBlueSide)
	{
		for(int i=0; i<distance; i++)
		{
			newPos._y++;
			// bullets can move 1 meter to every side every time they displace
			newPos._x += _world->getStatistics().getUniformDistValue(-1,1);
			if(!checkContinue(newPos))
			{
				return;
			}
		}
	}
	else
	{
		for(int i=0; i<distance; i++)
		{
			newPos._y--;
			newPos._x += _world->getStatistics().getUniformDistValue(-1,1);
			if(!checkContinue(newPos))
			{
				return;
			}
		}
	}
	// no impacts, hitting the ground	
	int numBullets = _world->getValue("virtualRecord", newPos);
	_world->setMaxValue("virtualRecord", newPos, numBullets+1);
	_world->setValue("virtualRecord", newPos, numBullets+1);
	remove();
}

bool Bullet::checkContinue( const Engine::Point2D<int> & newPos )
{
	Agent * target = _world->getAgent(newPos);
	// out of battlefield
	if(!target || target->isType("blueBullet") || target->isType("redBullet"))
	{
		if(!_world->checkPosition(newPos))
		{
			remove();
			return false;
		}
		else
		{
			//_position = newPos;			
			return true;
		}
	}
	Soldier * soldier = (Soldier*)target;
	// pass through ranks
	if(_isBlueSide && soldier->isType("blueSoldier"))
	{
		//_position = newPos;
		return true;
	}
	if(!_isBlueSide && soldier->isType("redSoldier"))
	{
		//_position = newPos;
		return true;
	}

	if(_world->getStatistics().getUniformDistValue(1, 100)<=_impactProbability)
	{
		// impact
		//std::cout << soldier << " has been impacted by bullet: " << this << " with type: " <<  target->getType() << std::endl;
		int numImpacts = _world->getValue("impacts", newPos);
		_world->setMaxValue("impacts", newPos, numImpacts+1);
		_world->setValue("impacts", newPos, numImpacts+1);

		soldier->setExists(false);
		remove();
		return false;
	}
	else
	{
		soldier->addStress(100-soldier->getCohesionRating());
		//std::cout << "bullet: " << this << " passing through" << std::endl;
		return true;
	}
	return true;
}

void * Bullet::createPackage()
{
	BulletPackage * package = new BulletPackage;
	memcpy(&package->_id, _id.c_str(), sizeof(char)*_id.size());
	package->_id[_id.size()] = '\0';
	package->_position = _position;
	package->_isBlueSide = _isBlueSide;
	package->_impactProbability = _impactProbability;
	
	return package;
}

void Bullet::serialize()
{
	serializeAttribute("isBlueSide", _isBlueSide);
}

void Bullet::setMuzzleVelocity( const int & standardVelocity )
{
	_velocity = _world->getStatistics().getNormalDistValue(standardVelocity-100, standardVelocity+100);
	// normal height
	float height = _world->getStatistics().getNormalDistValue(1.3f,1.6f);	
	_fallingTime = sqrt(2*height/9.8f);
	// we remove some time, due to resistance
	_fallingTime -= _world->getStatistics().getNormalDistValue(0.0f, 0.2f);
}

} // namespace BattleSim

