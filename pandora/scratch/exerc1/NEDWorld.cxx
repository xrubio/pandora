
#include "NEDWorld.hxx"
#include "Raster.hxx"
#include "Point2D.hxx"
#include "NeandertalGroup.hxx"
#include "SapiensGroup.hxx"
#include "Exceptions.hxx"
#include "Statistics.hxx"
#include "gdal_priv.h"

namespace NED
{

NEDWorld::NEDWorld( Engine::Simulation & simulation
		    , const std::string & path
		    , int numAgents
		    , int minHomeRange, int maxHomeRange
		    , int minSplitChance, int maxSplitChance
		    , int minCannibalism, int maxCannibalism) : World(simulation, maxHomeRange+1, false, path+"/ned.h5")
								, _numAgents(numAgents)
								, _agentKey(0)
								, _minHomeRange(minHomeRange)
								, _maxHomeRange(maxHomeRange)
								, _minSplitChance(minSplitChance)
								, _maxSplitChance(maxSplitChance)
								, _minCannibalism(minCannibalism)
								, _maxCannibalism(maxCannibalism)
								, _resourceType("")
								, _resourceFile("")
								, _peak(-1)
								, _distrib("")
{
	setSearchAgents(true);
	// overlap is maxHomeRange + 1 to allow splits to be in adjacent worlds
}

NEDWorld::NEDWorld( Engine::Simulation & simulation, NEDConfig & c) : World(simulation, c._maxHomeRange+1, false, c._path+"/ned.h5")
								, _numAgents(c._numAgents)
								, _agentKey(0)
								, _minHomeRange(c._minHomeRange)
								, _maxHomeRange(c._maxHomeRange)
								, _minSplitChance(c._minSplitChance)
								, _maxSplitChance(c._maxSplitChance)
								, _minCannibalism(c._minCannibalism)
								, _maxCannibalism(c._maxCannibalism)
								, _resourceType(c._resourceType)
								, _resourceFile(c._resourceFile)
								, _peak(c._peak)
								, _distrib(c._distrib)
{
	setSearchAgents(true);
	// overlap is maxHomeRange + 1 to allow splits to be in adjacent worlds
	// TODO code a function proces config for resources 
	    
}

NEDWorld::~NEDWorld()
{
}

void NEDWorld::createRasters()
{
	registerRaster("resources");
	//registerRaster("cannibalized");
	//getInitialRasters("cannibalized").setInitValues(0,std::numeric_limits<int>::max());
	// at least each cell has value 1
	if (_resourceType == "peak")
	    {
	    peakDistribution(_peak); // TODO substitute 3 by _peak
	    }
	if (_resourceType == "uniform")
	    {
	    randomUniformDistribution();
	    }
	if (_resourceType == "exp")
	    {
	    randomExponentialDistribution();
	    }
	if (_resourceType == "file")
	    {
	    loadFile();
	    }

	//updateRasterToMaxValues("resources");

	// finally we raise resource max to double, in prevision of future grow
	/*
	Engine::Point2D<int> index;
	for(index._x=0; index._x<_matrixSize; index._x++)
	{
		for(index._y=0; index._y<_matrixSize; index._y++)
		{
			getInitialRasters("resources").setMaxValue(index, 1+(getInitialRasters("resources").getMaxValue(index)*2));
		}
	}
	*/
}

void NEDWorld::loadFile()
{

	// TODO encapsulate GDAL in a class	
//	std::string fileName = "resources/fractal2_256_scarce.tiff";
	//std::string fileName = "resources/almenar_altures.tiff";
	
	std::cout << " loading from file " << _resourceFile << std::endl;
	
	std::string fileName = _resourceFile;
//	std::string fileName = "resources/fractal_512_scarce.tiff";
	
	GDALAllRegister();
	GDALDataset * dataset = (GDALDataset *)GDALOpen(fileName.c_str(), GA_ReadOnly );
	
	if(!dataset)
	{
		std::stringstream oss;
		oss << "NEDWorld::loadFile - file: " << fileName << " not found";
		throw Engine::Exception(oss.str());
	}

	int size = dataset->GetRasterXSize();
	if(size!=dataset->GetRasterYSize())
	{
		std::stringstream oss;
		oss << "NEDWorld::loadFile - file: " << fileName << " does not contain an squared raster. width: " << size << " and height: " << dataset->GetRasterYSize();
		throw Engine::Exception(oss.str());
	}

	if(size!=_simulation.getSize())
	{
		std::stringstream oss;
		oss << "NEDWorld::loadFile - file: " << fileName << " with size: " << size << " different from defined size: " << _simulation.getSize() << std::endl;
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
	getRaster("resources").setInitValues(0,std::numeric_limits<int>::max());

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
			setMaxValue("resources", index, value*2); //TODO tune this
			setValue("resources", index, value);
			
			std::cout << "data:" << index << "," << value << std::endl;
		}
	}
	GDALClose(dataset);
}

void NEDWorld::createPeak( const Engine::Point2D<int> & seed )
{
	std::cout << "creating peak at: " << seed._x << "/" << seed._y << std::endl;
	int dispersion = 5;
	Engine::Point2D<int> index;
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)
		{
			int dist = seed.distance(index);
			int sugarValue = 5 - dist/dispersion;
			//std::cout << _simulation.getId() << " checking global pos: " << index << " value: " << sugarValue << std::endl;
			if(getMaxValue("resources", index)<sugarValue)
			{
				//std::cout << _simulation.getId()<< " setting value of: " << index << " value: " << sugarValue  << std::endl;
				setMaxValue("resources", index, sugarValue);
			}
		}
	}
}

void NEDWorld::peakDistribution( int numOfPeaks )
{
	/*
	Engine::Point2D<int> index;	
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)
		{
			setMaxValue("resources", index, 10);
		}
	}
	*/

	for(int i=0; i<numOfPeaks; i++)
	{
		createPeak(Engine::Point2D<int>(_statistics.getUniformDistValue(0, _simulation.getSize()), _statistics.getUniformDistValue(0, _simulation.getSize())));
	}
}

void NEDWorld::randomUniformDistribution()
{	
	getRaster("resources").setInitValues(0,5);
	Engine::Point2D<int> index;
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)
		{
			int value = _statistics.getUniformDistValue(0,5);
			setMaxValue("resources",index, value);
			setValue("resources", index, value);
		}
	}
}

void NEDWorld::randomExponentialDistribution()
{	
	getRaster("resources").setInitValues(0,5);
	Engine::Point2D<int> index;
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)
		{
			int value = (int)round(_statistics.getExponentialDistValue(0, 5));
			setMaxValue("resources",index, value);
		}
	}
}

void NEDWorld::createAgents()
{
	for(int i=0; i<_numAgents; i++)
	{ 
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{
			std::ostringstream oss;
 			oss << "neandertal_" << i;
			NeandertalGroup * group;// = new NeandertalGroup(oss.str());
			bool located = false;
			// TODO clean all around that loop and the second creator call
			while(!located)
			{
				Engine::Point2D<int> bornPlace(_statistics.getUniformDistValue(0,_boundaries._size._x-1), _statistics.getUniformDistValue(0,_boundaries._size._y-1));
				bornPlace += _boundaries._origin;
				if(checkPosition(bornPlace) && _boundaries.isInside(bornPlace))
				{
				// TODO create correctly the group
				//group->_...
				group = new NeandertalGroup(oss.str()
				, bornPlace // position
				, true // exists
				, this // world
				, 10// resources
				, 1 // population
				, 1 // homeRange
				, 0 // cannibalism
				, 0 // stepsUsingCannibalism
				, 0 // age
				, 0 // splitChance
				, 0 // children
				, 25 // minForkResources
				, 10 // splitWaste
				, 10 // maxAge
				, 21 // minFoodToEat
				, 1 // minFoodToMove
				);

				located = true;
				}
			}

			//group->setPopulation((int)round(_statistics.getUniformDistValue(0,10)));
			//group->setPopulation(1);// 1 agent = 1 individual
			//group->setHomeRange((int)round(_statistics.getUniformDistValue(_minHomeRange, _maxHomeRange)));
			//group->setSplitChance((int)round(_statistics.getUniformDistValue(_minSplitChance, _maxSplitChance)));
			//group->setCannibalism((int)round(_statistics.getUniformDistValue(_minCannibalism, _maxCannibalism)));
			std::cout << _simulation.getId() << " new neandertal: " << group << std::endl;
			// TODO test a world with no agents!! 
			addAgent(group); 
		}
	}
}
	
void NEDWorld::stepRaster( const std::string & key)
{
	/*
	if(key=="cannibalized")
	{
		Engine::Point2D<int> index;
		for(index._x=initialPosition._x; index._x<initialPosition._x+matrixSize; index._x++)
		{
			for(index._y=initialPosition._y; index._y<initialPosition._y+matrixSize; index._y++)
			{
				raster.setResource(index, 0);
			}
		}
		return;
	}
	*/
	
//std::cerr << "step" << std::endl;
	
	if(key.compare("resources")==0)
	{
		Engine::Point2D<int> index;
		for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
		{
			for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)			
			{
				int value = getValue(key, index);				
				int maxValue = getMaxValue(key, index);
				if(value == 0)
				    {
				    value = _statistics.getUniformDistValue(0,maxValue/4); //TODO use maxValue/3 as upperbound???
				    }
				else if (value < maxValue)
				    {
				    Engine::Point2D<int> neighIndex;
				    int s  = 0; // sum of values for neighbours
				    int nn = 0; // num neighbours
				    // Evaluate surroundings. 
				    // Near resources conditionate resources in the cell 
				    for(neighIndex._x = index._x-1; neighIndex._x <= index._x+1; neighIndex._x++)
					for(neighIndex._y = index._y-1; neighIndex._y <= index._y+1; neighIndex._y++)
					    {
					    if ((neighIndex._x != index._x || neighIndex._y != index._y) && getOverlapBoundaries().isInside(neighIndex))
						{
						s += getValue(key, neighIndex);
						nn++;
						}
					    }
					    
				    // TODO : s in 0..S1 -> value--; s in S1..S2 -> value++; s in S2..+inf -> value--; 
				    /*if ( 0 <= s <= 3 ) 
					{
					value--;
					}
				    else if ( 3 < s < 8)
					{
					value++;
					}
				    else if ( 8 <= s )
					{
					value--;
					}*/
				    s = s/nn;	
				    if (s > value) 
					{
					value = _statistics.getUniformDistValue(value,(s+value)/2);					
					}
				    
				    //value = _statistics.getUniformDistValue(s,maxValue);
				    }
				if (value > maxValue) 
				  {
				  value = maxValue;
				  }
				setValue(key, index, value);
				/*if(value <= 0)
				  {
				  std::stringstream oss_exc;
				  oss_exc << "zero >= value : " << value;
				  throw Engine::Exception(oss_exc.str());    
				  }*/
			}
		}
	}
}

void NEDWorld::stepAgents()
{

	if(_step!=1000)	//TODO ¿¿en l'instant 1000 generes un poblacio?? 
	{
		return;
	}

	for(int i=0; i<_numAgents; i++)
	{ 
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{
			std::ostringstream oss;
 			oss << "sapiens_" << i;
			SapiensGroup * group = new SapiensGroup(oss.str());
			bool located = false;
			while(!located)
			{
				Engine::Point2D<int> pos(_statistics.getUniformDistValue(0,_boundaries._size._x-1), _statistics.getUniformDistValue(0,_boundaries._size._y-1));
				pos += _boundaries._origin;
				if(checkPosition(pos) && _boundaries.isInside(pos))
				{
					group->setPosition(pos);
					located = true;
				}
			}
			group->setPopulation((int)round(_statistics.getUniformDistValue(0,10)));
			group->setHomeRange((int)round(_statistics.getUniformDistValue(_minHomeRange, _maxHomeRange)));
			group->setSplitChance((int)round(_statistics.getUniformDistValue(_minSplitChance, _maxSplitChance)));
			std::cout << _simulation.getId() << " new agent: " << group << std::endl;
			addAgent(group); 
		}
	}
}

Engine::Agent * NEDWorld::createAgentFromPackage( const std::string & type, void * package )
{
	if(type.compare("neandertal")==0)
	{
		NeandertalGroupPackage * formattedPackage = 0;
		try
		{
			formattedPackage = (NeandertalGroupPackage*)package;
		}
		catch(std::exception & e)
		{
			std::cout << "NEDWorld::createAgentFromPackage, wrong cast with exception: " << e.what() << std::endl;
		}
		return (Engine::Agent*)(new NeandertalGroup(*formattedPackage));
	}
	else if(type.compare("sapiens")==0)
	{
		SapiensGroupPackage * formattedPackage = 0;
		try
		{
			formattedPackage = (SapiensGroupPackage*)package;
		}
		catch(std::exception & e)
		{
			std::cout << "NEDWorld::createAgentFromPackage, wrong cast with exception: " << e.what() << std::endl;
		}
		return (Engine::Agent*)(new SapiensGroup(*formattedPackage));
	}
	else
	{	
		std::stringstream oss;
		oss << "NEDWorld::createAgentFromPackage - unknown agent type: " << type;
		throw Engine::Exception(oss.str());
	}
}

void * NEDWorld::createPackage( const std::string & type )
{
	if(type.compare("neandertal")==0)
	{
		return new NeandertalGroupPackage;
	}
	else if(type.compare("sapiens")==0)
	{
		return new SapiensGroupPackage;
	}
	std::stringstream oss;
	oss << "NEDWorld::createPackage - unknown agent type: " << type;
	throw Engine::Exception(oss.str());
	return 0;
}

void NEDWorld::registerTypes()
{
	_types.insert( std::make_pair( "neandertal", createTypeNeandertal()));
	_types.insert( std::make_pair( "sapiens", createTypeSapiens()));
}

MPI_Datatype * NEDWorld::createTypeNeandertal()
{
	NeandertalGroupPackage package;
	
	int blockLengths[16];
	MPI_Datatype typelist[16];

	// block lengths and types
	// 32 chars
	blockLengths[0] = 32;
	typelist[0] = MPI_CHAR;	
	for(int i=1; i<16; i++)
	{
		blockLengths[i] = 1;
		typelist[i] = MPI_INT;
	}

	MPI_Aint displacements[16];
	displacements[0] = 0;

	MPI_Aint startAddress;
	MPI_Aint address;
	MPI_Address(package._id, &startAddress); 

	MPI_Address(&package._position._x, &address);
	displacements[1] = address-startAddress;
	MPI_Address(&package._position._y, &address);
	displacements[2] = address-startAddress;
	MPI_Address(&package._resources, &address);
	displacements[3] = address-startAddress;
	MPI_Address(&package._exists, &address);

	displacements[4] = address-startAddress;
	MPI_Address(&package._population, &address);
	displacements[5] = address-startAddress;
	MPI_Address(&package._homeRange, &address);
	displacements[6] = address-startAddress;
	MPI_Address(&package._cannibalism, &address);

	displacements[7] = address-startAddress;
	MPI_Address(&package._stepsUsingCannibalism, &address);
	displacements[8] = address-startAddress;
	MPI_Address(&package._age, &address);
	displacements[9] = address-startAddress;
	MPI_Address(&package._splitChance, &address);
	displacements[10] = address-startAddress;
	MPI_Address(&package._children, &address);

	displacements[11] = address-startAddress;
	MPI_Address(&package._minForkResources, &address);
	displacements[12] = address-startAddress;
	MPI_Address(&package._splitWaste, &address);
	displacements[13] = address-startAddress;
	MPI_Address(&package._maxAge, &address);
	displacements[14] = address-startAddress;
	MPI_Address(&package._minFoodToEat, &address);
	displacements[15] = address-startAddress;
	MPI_Address(&package._minFoodToMove, &address);
	
	MPI_Datatype * newDataType = new MPI_Datatype;
	MPI_Type_struct(11, blockLengths, displacements, typelist, newDataType);
	MPI_Type_commit(newDataType);
	return newDataType;
}

MPI_Datatype * NEDWorld::createTypeSapiens()
{
	SapiensGroupPackage package;
	
	int blockLengths[9];
	MPI_Datatype typelist[9];

	// block lengths and types
	// 32 chars
	blockLengths[0] = 32;
	typelist[0] = MPI_CHAR;	
	for(int i=1; i<9; i++)
	{
		blockLengths[i] = 1;
		typelist[i] = MPI_INT;
	}

	MPI_Aint displacements[9];
	displacements[0] = 0;

	MPI_Aint startAddress;
	MPI_Aint address;
	MPI_Address(package._id, &startAddress); 

	MPI_Address(&package._position._x, &address);
	displacements[1] = address-startAddress;
	MPI_Address(&package._position._y, &address);
	displacements[2] = address-startAddress;
	MPI_Address(&package._resources, &address);
	displacements[3] = address-startAddress;
	MPI_Address(&package._exists, &address);

	displacements[4] = address-startAddress;
	MPI_Address(&package._population, &address);
	displacements[5] = address-startAddress;
	MPI_Address(&package._homeRange, &address);

	displacements[6] = address-startAddress;
	MPI_Address(&package._age, &address);
	displacements[7] = address-startAddress;
	MPI_Address(&package._splitChance, &address);
	displacements[8] = address-startAddress;
	MPI_Address(&package._children, &address);

	MPI_Datatype * newDataType = new MPI_Datatype;
	MPI_Type_struct(9, blockLengths, displacements, typelist, newDataType);
	MPI_Type_commit(newDataType);
	return newDataType;
}


long int NEDWorld::getNewKey()
{
	return _agentKey++;
}

} // namespace NED

