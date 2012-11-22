
#include <Earth.hxx>

#include <EarthConfig.hxx>
#include <Human.hxx>
#include <Zombie.hxx>
#include <Simulation.hxx>
#include <Raster.hxx>
#include <Point2D.hxx>
#include <GeneralState.hxx>

namespace Examples 
{

Earth::Earth( Engine::Simulation & simulation, const EarthConfig & config ) : World(simulation, 11, true, config._resultsFile), _config(config), _indexZombies(0)
{
}

Earth::~Earth()
{
}

void Earth::createRasters()
{	
	registerStaticRaster("dem", true);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("dem"), _config._gisData._demName, this);	

	registerStaticRaster("population", true);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("population"), _config._gisData._populationName, this);		

	registerDynamicRaster("humans", true);
	getDynamicRaster("humans").setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	registerDynamicRaster("newCases", true);
	getDynamicRaster("newCases").setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	registerDynamicRaster("zombies", true);
	getDynamicRaster("zombies").setInitValues(0, std::numeric_limits<int>::max(), 0);

}

void Earth::createAgents()
{
	std::stringstream logName;
	logName << "simulation_" << _simulation.getId();
	log_INFO(logName.str(), MPI_Wtime() - _initialTime << " creating agents");

	Engine::Point2D<int> index;
	int totalHumans = 0;
	for(index._x=getBoundaries()._origin._x; index._x<getBoundaries()._origin._x+getBoundaries()._size._x; index._x++)
	{
		for(index._y=getBoundaries()._origin._y; index._y<getBoundaries()._origin._y+getBoundaries()._size._y; index._y++)
		{
			int numHumans = getValue("population", index);
			if(numHumans>0)
			{
				int scaledHumans = 0;
				for(int i=0; i<numHumans; i=i+_config._scale)
				{	
					std::ostringstream oss;
		 			oss << "Human_" << totalHumans+scaledHumans;
					Human * newHuman = new Human(oss.str());
					addAgent(newHuman);
					newHuman->setPosition(index);
					scaledHumans++;
				}
				totalHumans += scaledHumans;
				log_INFO(logName.str(), MPI_Wtime() - _initialTime << " created: " << scaledHumans << " humans at pos: " << index);
				setValue("humans", index, scaledHumans);
			}
		}
	}
	log_INFO(logName.str(), MPI_Wtime() - _initialTime << " " << totalHumans << " humans created");

	for(int i=0; i<_config._numCases; i++)
	{
		std::ostringstream oss;
		oss << "Zombie_" << _indexZombies;
		Zombie * newZombie = new Zombie(oss.str());
		addAgent(newZombie);
		newZombie->setPosition(_config._firstCase);
		_indexZombies++;
	}
	setValue("newCases", _config._firstCase, _config._numCases);
	setValue("zombies", _config._firstCase, _config._numCases);
	log_INFO(logName.str(), MPI_Wtime() - _initialTime << " " << _config._numCases << " zombies created at infection focus: " << _config._firstCase);
}

float Earth::getZombieVirulence() const
{
	return 1.0f;
}

void Earth::addZombie( const Engine::Point2D<int> & position )
{
	_newCases.push_back(position);
}

void Earth::stepEnvironment()
{
	std::stringstream logName;
	logName << "simulation_" << _simulation.getId();
	int totalHumans = 0;
	int totalZombies = 0;

	Engine::Point2D<int> index;
	for(index._x=getOverlapBoundaries()._origin._x; index._x<getOverlapBoundaries()._origin._x+getOverlapBoundaries()._size._x; index._x++)
	{
		for(index._y=getOverlapBoundaries()._origin._y; index._y<getOverlapBoundaries()._origin._y+getOverlapBoundaries()._size._y; index._y++)
		{
			setValue("humans", index, 0);
			setValue("zombies", index, 0);
		}
	}
	for(AgentsList::const_iterator it=_agents.begin(); it!=_agents.end(); it++)
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
	log_INFO(logName.str(), "step: " << _step << " num humans: " << totalHumans << " num zombies: " << totalZombies);
}
	
} // namespace Examples

