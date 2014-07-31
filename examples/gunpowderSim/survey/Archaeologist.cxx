
#include "Archaeologist.hxx"

#include <Exception.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>
#include <World.hxx>
#include <SurveyConfig.hxx>

namespace BattleSim
{

Archaeologist::Archaeologist( const std::string & id, int skill ) : Agent(id), _bulletsGathered(0), _skill(skill)
{
}

Archaeologist::~Archaeologist()
{
}

void Archaeologist::move()
{
    const SurveyConfig & surveyConfig = (const SurveyConfig&)getWorld()->getConfig();

    Engine::Point2D<int> newPos(_position);
	if(surveyConfig._horizontal)
	{
        newPos._x++;
	}
	else
	{
        newPos._y++;
	}
    if(getWorld()->checkPosition(newPos))
    {
        setPosition(newPos);
    }
}

void Archaeologist::collectFindings()
{
    const SurveyConfig & surveyConfig = (const SurveyConfig&)getWorld()->getConfig();
	// as each cell is 0.5 meters, the archaologist checks 1.5 meters (left, right and center of his/her direction)
	for(int i=-1; i<2; i++)
	{
		Engine::Point2D<int> posToCheck(_position);
		if(surveyConfig._horizontal)
		{
			posToCheck._y = _position._y+i;
		}
		else
		{
			posToCheck._x = _position._x+i;
		}
		if(!_world->checkPosition(posToCheck))
		{
			continue;
		}
		int numBullets = _world->getValue("degradedRecord", posToCheck);
		int collectedBullets = 0;
		for(int i=0; i<numBullets; i++)
		{
			if(Engine::GeneralState::statistics().getUniformDistValue(0,100)<_skill)
			{
				collectedBullets++;
			}
		}
		int pastCollectedBullets = _world->getValue("collectedBullets", posToCheck);
		_world->setValue("collectedBullets", posToCheck, pastCollectedBullets + collectedBullets);

		int exploredRasters = _world->getValue("exploredRasters", posToCheck);		
		_world->setValue("exploredRasters", posToCheck, exploredRasters+1);
	}
}

void Archaeologist::updateState()
{
	collectFindings();
    move();	
}

void Archaeologist::registerAttributes()
{
	registerIntAttribute("bullets gathered");
	registerIntAttribute("skill");
}

void Archaeologist::serialize()
{
	serializeAttribute("bullets gathered", _bulletsGathered);
	serializeAttribute("skill", _skill);
}

} // namespace BattleSim

