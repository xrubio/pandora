
#include <NeolithicWorld.hxx>
#include <NeolithicConfig.hxx>

#include <GeneralState.hxx>
/*
#include "Raster.hxx"
#include "Exception.hxx"
#include "Statistics.hxx"
#include "gdal_priv.h"
*/

namespace Examples 
{

NeolithicWorld::NeolithicWorld(const NeolithicConfig & config, Engine::Simulation & simulation, Engine::Scheduler * scheduler ) : World(simulation, scheduler, false), _config(config)
{
}

NeolithicWorld::~NeolithicWorld()
{
}

void NeolithicWorld::createRasters()
{
	registerStaticRaster("dem", true, eDem);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(eDem), _config._demFile, getBoundaries());

	registerStaticRaster("initPopulation", true, eInitPopulation);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(eInitPopulation), _config._initPopulationFile, getBoundaries());

	registerDynamicRaster("population", true, ePopulation);
	getDynamicRaster(ePopulation).setDefaultInitValues(0,std::numeric_limits<int>::max(), 0);
	registerDynamicRaster("populationBase", true, ePopulationBase);
	getDynamicRaster(ePopulationBase).setDefaultInitValues(0,std::numeric_limits<int>::max(), 0);
	registerDynamicRaster("arrivalTime", true, eArrivalTime);
	getDynamicRaster(eArrivalTime).setDefaultInitValues(0,std::numeric_limits<int>::max(), 0);

    for(auto index : getBoundaries() )
    {
        setMaxValue(ePopulation, index, _config._saturationDensity);
        setMaxValue(ePopulationBase, index, _config._saturationDensity);
        setMaxValue(eArrivalTime, index, _simulation.getNumSteps()+1);
        // if the point has initial population, set it at maximum
        if(getValue(eInitPopulation, index)!=0)
        {
            setValue(ePopulation, index, _config._saturationDensity);
            setValue(eArrivalTime, index, 1);
        }
        else
        {
            setValue(ePopulation, index, 0);
            setValue(eArrivalTime, index, -1);
        }
	}
}

void NeolithicWorld::stepEnvironment()
{
	reproductionStep();
	if(!_config._mountains && !_config._seaTravel)
	{
		homogeneousDispersionStep();
	}
	else
	{
		nonHomogeneousDispersionStep();
	}
	arrivalCheck();
}

void NeolithicWorld::reproductionStep()
{
    for( auto index : getBoundaries())
    {
        int actualPopulation = getValue(ePopulation, index);
        //int oldPopulation = actualPopulation;

        if(actualPopulation>=(_config._saturationDensity/_config._reproductiveRate))
        {
            actualPopulation = _config._saturationDensity;
        }
        else
        {
            actualPopulation *= _config._reproductiveRate;
        }

        setValue(ePopulationBase, index, actualPopulation);
        setValue(ePopulation, index, 0);
    }
}

void NeolithicWorld::homogeneousDispersionStep()
{
    for( auto index : getBoundaries())
    {
        float actualPopulation = _config._persistence*float(getValue(ePopulationBase, index));
        if(actualPopulation<=0.0f)
        {
            continue;
        }

        float migrationPopulation = ((1-_config._persistence)/4.0f)*float(getValue(ePopulationBase, index));
        
        /*
        if(migrationPopulation>0)
        {
            std::cout << "population migrating from: " << index << " with neighbors: " << numNeighbors << " = " << migrationPopulation << std::endl;
        }
        */

        setValue(ePopulation, index, getValue(ePopulation, index)+actualPopulation);
        
        Engine::Point2D<int> neighborW(index._x-1, index._y);
        if(getBoundaries().contains(neighborW))
        {
            setValue(ePopulation, neighborW, getValue(ePopulation, neighborW)+migrationPopulation);
        }
        
        Engine::Point2D<int> neighborE(index._x+1, index._y);
        if(getBoundaries().contains(neighborE))
        {
            setValue(ePopulation, neighborE, getValue(ePopulation, neighborE)+migrationPopulation);
        }
        
        Engine::Point2D<int> neighborN(index._x, index._y-1);	
        if(getBoundaries().contains(neighborN))
        {
            setValue(ePopulation, neighborN, getValue(ePopulation, neighborN)+migrationPopulation);
        }		
        
        Engine::Point2D<int> neighborS(index._x, index._y+1);	
        if(getBoundaries().contains(neighborS))
        {
            setValue(ePopulation, neighborS, getValue(ePopulation, neighborS)+migrationPopulation);

        }

        /*
        if(actualPopulation>0)
        {
            std::cout << "index: " << index << " generated pop: " << getValue("oldPopulation", index) << " remaining population: " << actualPopulation << " and migrating: " << migrationPopulation <<std::endl;				 
        }
        */
    }
}

void NeolithicWorld::disperseTo( const Engine::Point2D<int> & origin, const Engine::Point2D<int> & destination, const int & migrationPopulation )	
{
	if(!getBoundaries().contains(destination))
	{
		return;
	}

	if(isSea(destination))
	{
		travelBySea(origin, destination, migrationPopulation);
	}
	else if(!isMountain(destination))
	{
		if(!_config._seaTravel && !_config._mountains)
		{
			setValue(ePopulation, destination, getValue(ePopulation, destination)+migrationPopulation);
		}
		else
		{
			// if non homogeneous we need to avoid adding more population than max density (due to dividing between 1/2/3 neighbors)
			int finalPopulation = std::min(getValue(ePopulation, destination)+migrationPopulation, (int)_config._saturationDensity);
			setValue(ePopulation, destination, finalPopulation);
		}
	}
}

int NeolithicWorld::countNeighbors( const Engine::Point2D<int> & index )
{
	int neighbors = 0;
	
	Engine::Point2D<int> neighborW(index._x-1, index._y);
	if(getBoundaries().contains(neighborW) && !isMountain(neighborW))
	{
		neighbors++;
	}
	
	Engine::Point2D<int> neighborE(index._x+1, index._y);
	if(getBoundaries().contains(neighborE) && !isMountain(neighborE))
	{
		neighbors++;
	}

	Engine::Point2D<int> neighborN(index._x, index._y-1);
	if(getBoundaries().contains(neighborN) && !isMountain(neighborN))
	{
		neighbors++;
	}

	Engine::Point2D<int> neighborS(index._x, index._y+1);
	if(getBoundaries().contains(neighborS) && !isMountain(neighborS))
	{
		neighbors++;
	}
	return neighbors;
}

void NeolithicWorld::nonHomogeneousDispersionStep()
{
    for( auto index : getBoundaries())
	{
        float actualPopulation = _config._persistence*float(getValue(ePopulationBase, index));
        if(actualPopulation<=0.0f)
        {
            continue;
        }

        float numNeighbors = countNeighbors(index);
        float migrationPopulation = ((1-_config._persistence)/numNeighbors)*float(getValue(ePopulationBase, index));
        
        /*
        if(migrationPopulation>0)
        {
            std::cout << "population migrating from: " << index << " with neighbors: " << numNeighbors << " = " << migrationPopulation << std::endl;
        }
        */
        int realPopulation = std::min(getValue(ePopulation, index)+(int)actualPopulation, (int)_config._saturationDensity);
        setValue(ePopulation, index, realPopulation);
        
        Engine::Point2D<int> neighborW(index._x-1, index._y);
        disperseTo(index, neighborW, migrationPopulation);

        Engine::Point2D<int> neighborE(index._x+1, index._y);
        disperseTo(index, neighborE, migrationPopulation);

        Engine::Point2D<int> neighborN(index._x, index._y-1);	
        disperseTo(index, neighborN, migrationPopulation);

        Engine::Point2D<int> neighborS(index._x, index._y+1);	
        disperseTo(index, neighborS, migrationPopulation);
        
        /*
        if(actualPopulation>0)
        {
            std::cout << "index: " << index << " generated pop: " << getValue("oldPopulation", index) << " remaining population: " << actualPopulation << " and migrating: " << migrationPopulation <<std::endl;				 
        }
        */
    }

}
void NeolithicWorld::arrivalCheck()
{
    for( auto index : getBoundaries())
    {
        int newPopulation = getValue(ePopulation, index);
        /*
        if(newPopulation>0)
        {
            std::cout << "index: " << index << " past pop: " << pastPopulation << " new pop: " << newPopulation << " saturation density: " << _saturationDensity << std::endl;
        }
        */
        int threshold = 0.9f*_config._saturationDensity;
        int arrived = getValue(eArrivalTime, index);
        if(arrived==-1 && newPopulation>=threshold)
        {
            //std::cout << "neolithic arrived to: " << index << std::endl;
            setValue(eArrivalTime, index, _step+1);
        }
    }
}

bool NeolithicWorld::isMountain( const Engine::Point2D<int> & index )
{
	if(getValue(eDem,index)>_config._heightThreshold)
	{
		return true;
	}
	return false;
}

bool NeolithicWorld::isSea( const Engine::Point2D<int> & index )
{
	if(getValue(eDem,index)==-32768)
	{
		return true;
	}
	return false;
}

void NeolithicWorld::travelBySea( const Engine::Point2D<int> & origin, const Engine::Point2D<int> & destination, const int & migrationPopulation )
{
	//std::cout << "travelling from: " << origin <<  " to destination: " << destination << " BEGIN" << std::endl;
	std::vector<Engine::Point2D<int> > destinations;

	std::list<Engine::Point2D<int> > candidates;
	std::list<Engine::Point2D<int> > checked;
	Engine::Point2D<int> neighborW(destination._x-1, destination._y);
	candidates.push_back(neighborW);
	Engine::Point2D<int> neighborE(destination._x+1, destination._y);
	candidates.push_back(neighborE);
	Engine::Point2D<int> neighborN(destination._x, destination._y-1);
	candidates.push_back(neighborN);
	Engine::Point2D<int> neighborS(destination._x, destination._y+1);
	candidates.push_back(neighborS);

	while(!candidates.empty())
	{
		Engine::Point2D<int> point = candidates.front();
		candidates.pop_front();
	
		if(std::find(checked.begin(), checked.end(), point)!=checked.end())
		{
			//std::cout << "candidate: " << point << " already checked" << std::endl;
			continue;
		}
		checked.push_back(point);
		//std::cout << "checking: " << point << std::endl;
		if(!getBoundaries().contains(point))
		{
			//std::cout << "candidate: " << point << " out of bounds" << std::endl;
			continue;
		}

		if(isSea(point))
		{
			//std::cout << "candidate: " << point << " is sea" << std::endl;
			Engine::Point2D<int> nW(point._x-1, point._y);
			Engine::Point2D<int> nE(point._x+1, point._y);
			Engine::Point2D<int> nN(point._x, point._y-1);
			Engine::Point2D<int> nS(point._x, point._y+1);

			candidates.push_back(nW);
			candidates.push_back(nE);
			candidates.push_back(nN);
			candidates.push_back(nS);
		}
		else if((!isMountain(point)) && !(origin.isEqual(point)) && (origin.distance(point)<=_config._seaTravelDistance))
		{
			//std::cout << "candidate: " << point << " added to destinations" << std::endl;
			destinations.push_back(point);
		}
	}
	//std::cout << "travelling from: " << origin <<  " to destination: " << destination << " ENDED with neighbors: " << destinations.size() << std::endl;

	int migrationToDestination = (float)migrationPopulation/(float)destinations.size();
	for(int i=0; i<destinations.size(); i++)
	{
		int realPopulation = std::min(getValue(ePopulation, destinations[i])+migrationToDestination, (int)_config._saturationDensity);
		setValue(ePopulation, destinations[i], realPopulation);
	}
}

} // namespace Examples 

