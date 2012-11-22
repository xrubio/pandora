
#include <Bullet.hxx>

#include <Exceptions.hxx>
#include <Soldier.hxx>
#include <World.hxx>
#include <cstring>
#include <GeneralState.hxx>
#include <Logger.hxx>

namespace MusketSim
{

Bullet::Bullet( const std::string & id, bool isBlueSide, const int & impactProbability ) : Agent(id), _isBlueSide(isBlueSide), _velocity(0), _fallingTime(0.0f), _impactProbability(impactProbability)
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
	int numBullets = _world->getValue("virtualRecord", newPos);
	_world->setMaxValue("virtualRecord", newPos, numBullets+1);
	_world->setValue("virtualRecord", newPos, numBullets+1);
	remove();
}

bool Bullet::checkContinue( const Engine::Point2D<int> & newPos )
{
	if(!_world->checkPosition(newPos))
	{
		remove();
		return false;
	}
	std::vector<Agent *> targets = _world->getAgent(newPos);
	for(int i=0; i<targets.size(); i++)
	{
		Agent * target = targets.at(i);
		// out of battlefield
		if(!target->isType("Soldier"))
		{
			continue;
		}
		Soldier * soldier = (Soldier*)target;
		// pass through ranks
		if(_isBlueSide && soldier->isBlueSide())
		{
			//_position = newPos;
			continue;
		}
		if(!_isBlueSide && !soldier->isBlueSide())
		{
			//_position = newPos;
			continue;
		}

		std::stringstream logName;
		logName << "bullets_" << _world->getId();
		// impact
		if(Engine::GeneralState::statistics().getUniformDistValue(1, 100)<=_impactProbability)
		{
			log_DEBUG(logName.str(), soldier << " has been impacted by: " << this);
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
			log_DEBUG(logName.str(), soldier << " has been passed through by: " << this);
			continue;
		}
	}
	return true;
}

void Bullet::registerAttributes()
{
	registerIntAttribute("isBlueSide");
}

void Bullet::serialize()
{
	serializeAttribute("isBlueSide", _isBlueSide);
}

void Bullet::setMuzzleVelocity( const int & standardVelocity )
{
	_velocity = Engine::GeneralState::statistics().getNormalDistValue(standardVelocity-100, standardVelocity+100);
	// normal height
	float height = Engine::GeneralState::statistics().getNormalDistValue(1.3f,1.6f);	
	_fallingTime = sqrt(2*height/9.8f);
	// we remove some time, due to resistance
	_fallingTime -= Engine::GeneralState::statistics().getNormalDistValue(0.0f, 0.2f);
}

} // namespace MusketSim

