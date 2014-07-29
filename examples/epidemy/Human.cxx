
#include <Human.hxx>
#include <MoveAction.hxx>
#include <Statistics.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <Statistics.hxx>
#include <Earth.hxx>

#include <DoNothingAction.hxx>
#include <MoveAction.hxx>

namespace Examples
{

Human::Human( const std::string & id, bool infected ) : Agent(id), _threatLevel(0.0f), _infected(infected), _hasHumans(false), _remainingTime(30)
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

    if(!_infected)
    {
	    int numZombies = getWorld()->getValue(eZombies, _position);
    	if(numZombies==0)
	    {
		    _threatLevel = 0.0f;
    		return;
    	    }
	    int numHumans = getWorld()->getValue(eHumans, _position);
    	// remove this one
    	numHumans--;
	    _threatLevel = (float)numZombies/(float)numHumans;
    	log_DEBUG(logName.str(), this << " with: " << numHumans << " humans and: " << numZombies << " zombies with threat level: " << _threatLevel);
        return;
    }
	int numHumans = getWorld()->getValue(eHumans, _position);
	int numZombies = getWorld()->getValue(eZombies, _position);
	if(numHumans==0)
	{
		_hasHumans = false;
	}
	else
	{
		_hasHumans = true;
	}
}

void Human::selectActions()
{
    if(!_infected)
    {
	    _actions.push_back(new DoNothingAction());
        return;
    }
    if(_hasHumans)
	{
		if(Engine::GeneralState::statistics().getUniformDistValue()>_threatLevel)
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

void Human::updateState()
{
    if(_infected)
    {
	    _remainingTime--;
    	if(_remainingTime<0)
	    {
		    remove();
    	}
        return;
    }

	// check attacks
	Earth & world = (Earth&)getWorldRef();
	float probabilityAttack = _threatLevel*world.getZombieVirulence();
	if(Engine::GeneralState::statistics().getUniformDistValue()>=probabilityAttack)
	{
        return;
    }
    _infected = true;
    changeType("Zombie");
    log_DEBUG(logName.str(), this << " had threat level: " << _threatLevel << " turned to zombie!");
}

void Human::serialize()
{
    if(!_infected)
    {
	    serializeAttribute("threatLevel", (int)(_threatLevel*100.0f));
        return;
    }
	serializeAttribute("threatLevel", 0);
}

} // namespace Examples

