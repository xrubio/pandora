
#include <NeolithicWorld.hxx>
#include <NeolithicConfig.hxx>

#include <GeneralState.hxx>
/*
#include "Raster.hxx"
#include "Exceptions.hxx"
#include "Statistics.hxx"
#include "gdal_priv.h"
*/

namespace Examples 
{

NeolithicWorld::NeolithicWorld( Engine::Simulation & simulation, const NeolithicConfig & config ) : World(simulation, 1, false, config._resultsFile), _config(config)
{
}

NeolithicWorld::~NeolithicWorld()
{
}
/*
void NeolithicWorld::loadFile( const std::string & fileName, const std::string & raster)
{
	// TODO encapsulate GDAL in a class	
	GDALAllRegister();
	GDALDataset * dataset = (GDALDataset *)GDALOpen(fileName.c_str(), GA_ReadOnly );
	
	if(!dataset)
	{
		std::stringstream oss;
		oss << "NeolithicWorld::loadFile - file: " << fileName << " not found";
		throw Engine::Exception(oss.str());
	}

	int size = dataset->GetRasterXSize();
	if(size!=dataset->GetRasterYSize())
	{
		std::stringstream oss;
		oss << "NeolithicWorld::loadFile - file: " << fileName << " does not contain an squared raster. width: " << size << " and height: " << dataset->GetRasterYSize();
		throw Engine::Exception(oss.str());
	}

	if(size!=_simulation.getSize())
	{
		std::stringstream oss;
		oss << "NeolithicWorld::loadFile - file: " << fileName << " with size: " << size << " different from defined size: " << _simulation.getSize() << std::endl;
		throw Engine::Exception(oss.str());
	}

	GDALRasterBand * band = dataset->GetRasterBand(1);
	double minMaxValues[2];
	int hasMin, hasMax;
	minMaxValues[0] = band->GetMinimum(&hasMin);
	minMaxValues[1] = band->GetMaximum(&hasMax);
	if(!(hasMin && hasMax))
	{
		GDALComputeRasterMinMax((GDALRasterBandH)band, TRUE, minMaxValues);
	}

	//std::cout << "max value: " << minMaxValues[1] << std::endl;
	getRaster(raster).setInitValues(0,std::numeric_limits<int>::max());

	float * pafScanline = (float *)CPLMalloc(sizeof(float)*(_localRasterSize*_localRasterSize));
	int worldsPerRow = sqrt(_simulation.getNumTasks());
	int column = _simulation.getId()/worldsPerRow;
	int row = _simulation.getId()%worldsPerRow;		
	band->RasterIO( GF_Read, row*_localRasterSize, column*_localRasterSize, _localRasterSize, _localRasterSize, pafScanline, _localRasterSize, _localRasterSize, GDT_Float32, 0, 0 );
	
	Engine::Point2D<int> index;
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)			
		{
			Engine::Point2D<int> index2(index - _boundaries._origin);
			int value = (int)(pafScanline[_localRasterSize*index2._y+index2._x]);
			setMaxValue(raster, index, value);
			setValue(raster, index, value);
		}
	}
	GDALClose(dataset);
}
*/

void NeolithicWorld::createRasters()
{
	registerStaticRaster("dem", true, eDem);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(eDem), _config._demFile, this);

	registerStaticRaster("initPopulation", true, eInitPopulation);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(eInitPopulation), _config._initPopulationFile, this);

	registerDynamicRaster("population", true, ePopulation);
	registerDynamicRaster("populationBase", true, ePopulationBase);
	registerDynamicRaster("arrivalTime", true, eArrivalTime);
	
	Engine::Point2D<int> index;	
	for(index._x=_overlapBoundaries._origin._x; index._x<_overlapBoundaries._origin._x+_overlapBoundaries._size._x; index._x++)		
	{
		for(index._y=_overlapBoundaries._origin._y; index._y<_overlapBoundaries._origin._y+_overlapBoundaries._size._y; index._y++)			
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

}

void NeolithicWorld::createAgents()
{
}

void NeolithicWorld::stepRasters()
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
	Engine::Point2D<int> index;	
	for(index._x=_overlapBoundaries._origin._x; index._x<_overlapBoundaries._origin._x+_overlapBoundaries._size._x; index._x++)		
	{
		for(index._y=_overlapBoundaries._origin._y; index._y<_overlapBoundaries._origin._y+_overlapBoundaries._size._y; index._y++)			
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

			/*
			if(actualPopulation>0)
			{
				std::cout << index << " has grown from: " << oldPopulation << " to: " << actualPopulation << std::endl;
			}
			*/
			setValue(ePopulationBase, index, actualPopulation);
			setValue(ePopulation, index, 0);
		}
	}
}

void NeolithicWorld::homogeneousDispersionStep()
{
	Engine::Point2D<int> index;	
	for(index._x=_overlapBoundaries._origin._x; index._x<_overlapBoundaries._origin._x+_overlapBoundaries._size._x; index._x++)		
	{
		for(index._y=_overlapBoundaries._origin._y; index._y<_overlapBoundaries._origin._y+_overlapBoundaries._size._y; index._y++)			
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
			if(_overlapBoundaries.isInside(neighborW))
			{
				setValue(ePopulation, neighborW, getValue(ePopulation, neighborW)+migrationPopulation);
			}
			
			Engine::Point2D<int> neighborE(index._x+1, index._y);
			if(_overlapBoundaries.isInside(neighborE))
			{
				setValue(ePopulation, neighborE, getValue(ePopulation, neighborE)+migrationPopulation);
			}
			
			Engine::Point2D<int> neighborN(index._x, index._y-1);	
			if(_overlapBoundaries.isInside(neighborN))
			{
				setValue(ePopulation, neighborN, getValue(ePopulation, neighborN)+migrationPopulation);
			}		
			
			Engine::Point2D<int> neighborS(index._x, index._y+1);	
			if(_overlapBoundaries.isInside(neighborS))
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

}

void NeolithicWorld::disperseTo( const Engine::Point2D<int> & origin, const Engine::Point2D<int> & destination, const int & migrationPopulation )	
{
	if(!_overlapBoundaries.isInside(destination))
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
	if(_overlapBoundaries.isInside(neighborW) && !isMountain(neighborW))
	{
		neighbors++;
	}
	
	Engine::Point2D<int> neighborE(index._x+1, index._y);
	if(_overlapBoundaries.isInside(neighborE) && !isMountain(neighborE))
	{
		neighbors++;
	}

	Engine::Point2D<int> neighborN(index._x, index._y-1);
	if(_overlapBoundaries.isInside(neighborN) && !isMountain(neighborN))
	{
		neighbors++;
	}

	Engine::Point2D<int> neighborS(index._x, index._y+1);
	if(_overlapBoundaries.isInside(neighborS) && !isMountain(neighborS))
	{
		neighbors++;
	}
	return neighbors;
}

void NeolithicWorld::nonHomogeneousDispersionStep()
{
	Engine::Point2D<int> index;	
	for(index._x=_overlapBoundaries._origin._x; index._x<_overlapBoundaries._origin._x+_overlapBoundaries._size._x; index._x++)		
	{
		for(index._y=_overlapBoundaries._origin._y; index._y<_overlapBoundaries._origin._y+_overlapBoundaries._size._y; index._y++)			
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

}
void NeolithicWorld::arrivalCheck()
{
	Engine::Point2D<int> index;	
	for(index._x=_overlapBoundaries._origin._x; index._x<_overlapBoundaries._origin._x+_overlapBoundaries._size._x; index._x++)		
	{
		for(index._y=_overlapBoundaries._origin._y; index._y<_overlapBoundaries._origin._y+_overlapBoundaries._size._y; index._y++)			
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
		if(!_overlapBoundaries.isInside(point))
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

