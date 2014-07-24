
#include <Zombie.hxx>
#include <MoveAction.hxx>
#include <DoNothingAction.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>
#include <Logger.hxx>

namespace Examples
{

Zombie::Zombie( const std::string & id ) : Agent(id), /*_target(0), */ _hasHumans(false), _remainingTime(10)
{
}

Zombie::~Zombie()
{
}

void Zombie::selectActions()
{	
	// if it does not have target it will move
	// if it has target it will move randomly to another position 10% of times
//	if(_target)
	if(_hasHumans)
	{
		if(Engine::GeneralState::statistics().getUniformDistValue()<0.1)
		{
			_actions.push_back(new MoveAction());
		}
		else
		{
			_actions.push_back(new DoNothingAction());
		}
	}
	else
	{
		_actions.push_back(new MoveAction());
	}
}

void Zombie::updateKnowledge()
{
	std::stringstream logName;
	logName << "zombies_" << _world->getId();
	//_target = 0;

	int numHumans = getWorld()->getValue("humans", _position);
	int numZombies = getWorld()->getValue("zombies", _position);
	if(numHumans==0)
	{
		_hasHumans = false;
	}
	else
	{
		_hasHumans = true;
	}
	log_DEBUG(logName.str(), this << " with: " << numHumans << " humans and: " << numZombies << " zombies has food: " << _hasHumans);	

	/* TODO interact?
	std::vector<Agent *> susceptible = _world->getAgent(_position, "Human");
	if(susceptible.size()==0)
	{
		return;
	}
	std::random_shuffle(susceptible.begin(), susceptible.end());
	// TODO interact?
	_target = susceptible.at(0);
	log_DEBUG(logName.str(), this << " with: " << numHumans << " humans and: " << numZombies << " zombies with target: " << _target);
	*/
}

void Zombie::updateState()
{
	_remainingTime--;
	if(_remainingTime<0)
	{
		remove();
	}
}

void Zombie::serialize()
{
}

} // namespace Examples

