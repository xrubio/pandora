
#include <Earth.hxx>

#include <EarthConfig.hxx>
#include <Human.hxx>
#include <Zombie.hxx>
#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <typedefs.hxx>

namespace Examples 
{

Earth::Earth( EarthConfig * config, Engine::Scheduler * scheduler ) : World(config, scheduler, true), _indexZombies(0)
{
}

Earth::~Earth()
{
}

void Earth::createRasters()
{
    const EarthConfig & earthConfig = (const EarthConfig&)getConfig();
	registerStaticRaster("dem", true);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("dem"), earthConfig._demName, getBoundaries());	

	registerStaticRaster("population", true);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("population"), earthConfig._populationName, getBoundaries());		

	registerDynamicRaster("humans", true);
	getDynamicRaster("humans").setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	registerDynamicRaster("newCases", true);
	getDynamicRaster("newCases").setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	registerDynamicRaster("zombies", true);
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
    for(auto index : getBoundaries())
    {
        int numHumans = getValue("population", index);
        if(numHumans>0)
        {
            int scaledHumans = 0;
            for(int i=0; i<numHumans; i=i+earthConfig._scale)
            {	
                std::ostringstream oss;
                oss << "Human_" << totalHumans+scaledHumans;
                Human * newHuman = new Human(oss.str());
                addAgent(newHuman);
                newHuman->setPosition(index);
                scaledHumans++;
            }
            totalHumans += scaledHumans;
            log_INFO(logName.str(), getWallTime() << " created: " << scaledHumans << " humans at pos: " << index);
            setValue("humans", index, scaledHumans);
        }
	}
	log_INFO(logName.str(), getWallTime() << " " << totalHumans << " humans created");

	for(int i=0; i<earthConfig._numCases; i++)
	{
		std::ostringstream oss;
		oss << "Zombie_" << _indexZombies;
		Zombie * newZombie = new Zombie(oss.str());
		addAgent(newZombie);
		newZombie->setPosition(earthConfig._firstCase);
		_indexZombies++;
	}
	setValue("newCases", earthConfig._firstCase, earthConfig._numCases);
	setValue("zombies", earthConfig._firstCase, earthConfig._numCases);
	log_INFO(logName.str(), getWallTime() << " " << earthConfig._numCases << " zombies created at infection focus: " << earthConfig._firstCase);
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
    	setValue("humans", index, 0);
	    setValue("zombies", index, 0);
	}
	for(Engine::AgentsList::const_iterator it=_agents.begin(); it!=_agents.end(); it++)
	{
		const Engine::Agent & agent = **it;	
		if(!agent.exists())
		{
			continue;
		}
		if(agent.isType("Human"))
		{
			int previousHumans = getValue("humans", agent.getPosition());
			setValue("humans", agent.getPosition(), previousHumans+1);
			totalHumans++;
		}
		else if(agent.isType("Zombie"))
		{	
			int previousZombies = getValue("zombies", agent.getPosition());
			setValue("zombies", agent.getPosition(), previousZombies+1);
			totalZombies++;
		}
	}
	log_INFO(logName.str(), getWallTime() << " step: " << _step << " num humans: " << totalHumans << " num zombies: " << totalZombies);
}
	
} // namespace Examples

