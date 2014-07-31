
#include <Earth.hxx>

#include <EarthConfig.hxx>
#include <Human.hxx>
#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <typedefs.hxx>

namespace Examples 
{

Earth::Earth( EarthConfig * config, Engine::Scheduler * scheduler ) : World(config, scheduler, true)
{
}

Earth::~Earth()
{
}

void Earth::createRasters()
{
    const EarthConfig & earthConfig = (const EarthConfig&)getConfig();
	registerStaticRaster("dem", true, eDem);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("dem"), earthConfig._demName, getBoundaries());	

	registerStaticRaster("population", true, ePopulation);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("population"), earthConfig._populationName, getBoundaries());		
	registerDynamicRaster("humans", true, eHumans);
	getDynamicRaster("humans").setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	registerDynamicRaster("newCases", true, eNewCases);
	getDynamicRaster("newCases").setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	registerDynamicRaster("zombies", true, eZombies);
	getDynamicRaster("zombies").setInitValues(0, std::numeric_limits<int>::max(), 0);

}

void Earth::createAgents()
{
    const EarthConfig & earthConfig = (const EarthConfig&)getConfig();
	std::stringstream logName;
	logName << "simulation_" << getId();
	log_INFO(logName.str(), getWallTime() << " creating agents");

	Engine::Point2D<int> index;
	int totalHumans = 0;
    int indexHumans = 0;
    for(auto index : getBoundaries())
    {
        int numHumans = getValue(ePopulation, index);
        if(numHumans>0)
        {
            int scaledHumans = 0;
            for(int i=0; i<numHumans; i=i+earthConfig._scale)
            {	
                std::ostringstream oss;
                oss << "Human_" << indexHumans;
                Human * newHuman = new Human(oss.str());
                addAgent(newHuman);
                newHuman->setPosition(index);
                scaledHumans++;
                indexHumans++;
            }
            totalHumans += scaledHumans;
            log_DEBUG(logName.str(), getWallTime() << " created: " << scaledHumans << " humans at pos: " << index);
            setValue(eHumans, index, scaledHumans);
        }
	}
	log_INFO(logName.str(), getWallTime() << " " << indexHumans << " humans created");

	for(int i=0; i<earthConfig._numCases; i++)
	{
		std::ostringstream oss;
		oss << "Zombie_" << indexHumans;
        indexHumans++;
		Human * newZombie = new Human(oss.str(), true);
		addAgent(newZombie);
		newZombie->setPosition(earthConfig._firstCase);
	}

    if(getBoundaries().contains(earthConfig._firstCase))
    {
	    setValue(eNewCases, earthConfig._firstCase, earthConfig._numCases);
    	setValue(eZombies, earthConfig._firstCase, earthConfig._numCases);
	    log_INFO(logName.str(), getWallTime() << " " << earthConfig._numCases << " zombies created at infection focus: " << earthConfig._firstCase);
    }
}

float Earth::getZombieVirulence() const
{
    const EarthConfig & earthConfig = (const EarthConfig&)getConfig();
	return earthConfig._virulence;
}

void Earth::addZombie( const Engine::Point2D<int> & position )
{
	_newCases.push_back(position);
}

void Earth::stepEnvironment()
{
	std::stringstream logName;
	logName << "simulation_" << getId();
	int totalHumans = 0;
	int totalZombies = 0;

    for(auto index : getBoundaries())
    {
    	setValue(eHumans, index, 0);
	    setValue(eZombies, index, 0);
	}
	for(Engine::AgentsList::const_iterator it=beginAgents(); it!=endAgents(); it++)
	{
		if(!(*it)->exists() || !getBoundaries().contains((*it)->getPosition()))
		{
			continue;
		}
		if((*it)->isType("Human"))
		{
			int previousHumans = getValue(eHumans, (*it)->getPosition());
			setValue(eHumans, (*it)->getPosition(), previousHumans+1);
			totalHumans++;
		}
		else
		{	
			int previousZombies = getValue(eZombies, (*it)->getPosition());
			setValue(eZombies, (*it)->getPosition(), previousZombies+1);
			totalZombies++;
		}
	}
	log_INFO(logName.str(), getWallTime() << " step: " << _step << " num humans: " << totalHumans << " num zombies: " << totalZombies);
}
	
} // namespace Examples

