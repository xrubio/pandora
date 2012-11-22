
#include <Human.hxx>
#include <MoveAction.hxx>
#include <Statistics.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <Statistics.hxx>
#include <Earth.hxx>
#include <Zombie.hxx>

#include <DoNothingAction.hxx>
#include <MoveAction.hxx>

namespace Examples
{

Human::Human( const std::string & id ) : Agent(id), _threatLevel(0.0f)
{
}

Human::~Human()
{
}

void Human::registerAttributes()
{
	registerIntAttribute("threatLevel");
}

void Human::updateKnowledge()
{
	std::stringstream logName;
	logName << "agents_" << _world->getId();

	int numZombies = getWorld()->getValue("zombies", _position);
	if(numZombies==0)
	{
		_threatLevel = 0.0f;
		return;
	}
	int numHumans = getWorld()->getValue("humans", _position);
	// remove this one
	numHumans--;
	_threatLevel = (float)numZombies/(float)numHumans;
	log_DEBUG(logName.str(), this << " with: " << numHumans << " humans and: " << numZombies << " zombies with threat level: " << _threatLevel);
}

void Human::selectActions()
{
	_actions.push_back(new DoNothingAction());

	/*
	// more zombies than humans
	if(_threatLevel>1.0f)
	{
		_actions.push_back(new MoveAction());
	}
	else
	{
		_actions.push_back(new DoNothingAction());
	}
	*/
}

void Human::updateState()
{
	// check attacks
	Earth & world = (Earth&)getWorldRef();
	float probabilityAttack = _threatLevel*world.getZombieVirulence();
	// until 10 million people...
	// value entre with 0'00000001 precision
	float randomValue = Engine::GeneralState::statistics().getUniformDistValue(0, 10000000);
	randomValue /= 10000000.0f;
	if(randomValue<probabilityAttack)
	{
		std::ostringstream oss;
		std::size_t endType = getId().find("_");
		oss << "Zombie_" << getId().substr(endType+1);
		Zombie * newZombie = new Zombie(oss.str());
		getWorld()->addAgent(newZombie);
		newZombie->setPosition(_position);
		int previous = getWorld()->getValue("newCases", _position);
		getWorld()->setValue("newCases", _position, previous+1);
		std::stringstream logName;
		logName << "agents_" << _world->getId();

		log_DEBUG(logName.str(), this << " had threat level: " << _threatLevel << " turned to zombie!");
		remove();
	}
}

void Human::serialize()
{
	serializeAttribute("threatLevel", (int)(_threatLevel*100.0f));
}

} // namespace Examples

