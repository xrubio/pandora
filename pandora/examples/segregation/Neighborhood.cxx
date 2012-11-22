
#include "Neighborhood.hxx"
#include "Raster.hxx"
#include "Neighbor.hxx"
#include "Exceptions.hxx"
#include "NeighborConfig.hxx"

namespace Segregation 
{

Neighborhood::Neighborhood( Engine::Simulation & simulation, const NeighborConfig & config ) : World(simulation, std::max(config._neighborDistance, config._maxMovingDistance), false, config._path+"/segregation.h5"), _config(config)					
{
}

Neighborhood::~Neighborhood()
{
}    

void Neighborhood::createRasters()
{
	registerDynamicRaster("base", true);
	getDynamicRaster("base").setInitValues(0, std::numeric_limits<int>::max(), 0);
}

void Neighborhood::createAgents()
{
	int numAgents = _config._size * _config._size * _config._populationDensity;
	int numDimes = numAgents * _config._dimePercentage;
	std::cout << "creating : " << numAgents << " neighbors, dimes: " << numDimes << " and pennies: " << numAgents - numDimes  << std::endl;

	int indexDimes = 0;
	int indexPennies = 0;
	for(int i=0; i<numAgents; i++)
	{
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{
			std::ostringstream oss;
			if(indexDimes<numDimes)
			{
 				oss << "dime_" << indexDimes;
				indexDimes++;

			}
			else
			{
				oss << "pennies_" << indexPennies;
				indexPennies++;
			}
			Neighbor * agent = new Neighbor(oss.str());
			addAgent(agent);
			agent->setFriendlyPercentage( _config._friendlyPercentage );
			agent->setNeighborDistance( _config._neighborDistance );
			agent->setMaxMovingDistance( _config._maxMovingDistance);
			agent->setPosition(getRandomPosition());
		}
	}
}

Engine::Agent * Neighborhood::createAgentFromPackage( const std::string & type, void * package )
{
	Neighbor * formattedPackage = 0;
	try
	{
		formattedPackage = (Neighbor*)package;
	}
	catch(std::exception & e)
	{
		std::cout << "Neighborhood::createAgentFromPackage, wrong cast with exception: " << e.what() << std::endl;
	}
	return (Engine::Agent*)(new Neighbor(*formattedPackage));
}

void * Neighborhood::createPackage( const std::string & type )
{
	std::stringstream oss;
	oss << "Neighborhood::createPackage() NOT implemented!" << std::endl;
	throw Engine::Exception(oss.str());
	return 0;
}

void Neighborhood::stepEnvironment()
{
}

void Neighborhood::registerTypes()
{
	//_types.insert( std::make_pair( "Neighbor", createTypeNeighbor()));
}

MPI_Datatype * Neighborhood::createTypeNeighbor()
{	
	// TODO
	return 0;
	/*
	GujaratHuntGathPackage package;
	
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
	*/
	return NULL;
}

} // namespace Segregation 

