
#include "Bullet.hxx"
#include "Soldier.hxx"
#include "Battlefield.hxx"

#include <Exception.hxx>
#include <World.hxx>
#include <cstring>
#include <Statistics.hxx>
#include <GeneralState.hxx>
#include <typedefs.hxx>

namespace BattleSim
{

Bullet::Bullet( const std::string & id, const int & impactProbability ) : Agent(id), _isBlueSide(isType("blueBullet")), _velocity(0), _fallingTime(0.0f), _impactProbability(impactProbability)
{
}

Bullet::~Bullet()
{
}

void Bullet::updateState()
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
			newPos._x += Engine::GeneralState::statistics().getUniformDistValue(-1,1);
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
			newPos._x += Engine::GeneralState::statistics().getUniformDistValue(-1,1);
			if(!checkContinue(newPos))
			{
				return;
			}
		}
	}
	// no impacts, hitting the ground	
	int numBullets = _world->getValue(eVirtualRecord, newPos);
	_world->setMaxValue(eVirtualRecord, newPos, numBullets+1);
	_world->setValue(eVirtualRecord, newPos, numBullets+1);
	remove();
}

bool Bullet::checkContinue( const Engine::Point2D<int> & newPos )
{
    Engine::AgentsVector targets = _world->getAgent(newPos);
	// out of battlefield
	if(targets.size()==0 || targets.at(0)->isType("blueBullet") || targets.at(0)->isType("redBullet"))
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
	Soldier * soldier = (Soldier*)targets.at(0).get();
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
	
	if(Engine::GeneralState::statistics().getUniformDistValue(1, 100)<=_impactProbability)
	{
		// impact
		//std::cout << soldier << " has been impacted by bullet: " << this << " with type: " <<  target->getType() << std::endl;
		int numImpacts = _world->getValue(eImpacts, newPos);
		_world->setMaxValue(eImpacts, newPos, numImpacts+1);
		_world->setValue(eImpacts, newPos, numImpacts+1);

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

void Bullet::serialize()
{
	serializeAttribute("isBlueSide", _isBlueSide);
}

void Bullet::registerAttributes()
{
	registerIntAttribute("isBlueSide");
}

void Bullet::setMuzzleVelocity( const int & standardVelocity )
{
	_velocity = Engine::GeneralState::statistics().getNormalDistValueMinMax(standardVelocity-100, standardVelocity+100);
	// normal height
	float height = Engine::GeneralState::statistics().getNormalDistValueMinMax(1.3f,1.6f);	
	_fallingTime = sqrt(2*height/9.8f);
	// we remove some time, due to resistance
	_fallingTime -= Engine::GeneralState::statistics().getNormalDistValueMinMax(0.0f, 0.2f);
}

} // namespace BattleSim

