
#include "Site.hxx"
#include "Archaeologist.hxx"

#include "Raster.hxx"
#include "Point2D.hxx"
#include "Exceptions.hxx"
#include "Statistics.hxx"
#include "Rectangle.hxx"

#include <cmath>

namespace BattleSim 
{
	
Site::Site( const std::string & path, const std::string & battlefieldFile, int distance, bool horizontalMovement ) : World(1, 0, 0, true, path+"/excavatedBattlefield.h5"), _distance(distance), _horizontalMovement(horizontalMovement)
{
	setSearchAgents(true);
	_record.loadHDF5(battlefieldFile, true, false);

	_globalRasterSize = _record.getSize();
	_globalBoundaries = Engine::Rectangle<int>(Engine::Point2D<int>(0,0), Engine::Point2D<int>(_globalRasterSize, _globalRasterSize));
}

Site::~Site()
{
}

void Site::createRasters()
{
	registerRaster("originalRecord");
	registerRaster("degradedRecord");
	registerRaster("impacts");
	registerRaster("collectedBullets");
	registerRaster("exploredRasters");
	
	// copying data
	Engine::Raster & originalVirtualRecord = _record.getRaster("originalRecord", 0);
	Engine::Point2D<int> index;
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)			
		{
			setMaxValue("originalRecord", index, originalVirtualRecord.getValue(index));
			setValue("originalRecord", index, originalVirtualRecord.getValue(index));
		}
	}
	Engine::Raster & degradedVirtualRecord = _record.getRaster("degradedRecord", 0);
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)			
		{
			setMaxValue("degradedRecord", index, degradedVirtualRecord.getValue(index));
			setValue("degradedRecord", index, degradedVirtualRecord.getValue(index));
		}
	}
	Engine::Raster & originalImpacts = _record.getRaster("impacts", 0);
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)			
		{
			setMaxValue("impacts", index, originalImpacts.getValue(index));
			setValue("impacts", index, originalImpacts.getValue(index));
		}
	}

	// TODO carregar collectedBullets si existeix
}

void Site::createAgents()
{
	int numArchaeologists = _globalRasterSize/_distance;
	//std::cout << " num archaeos: " << numArchaeologists << " with global raster size: " << _globalRasterSize << " and distance: " << _distance << std::endl;
	for(int i=0; i<numArchaeologists; i++)
	{
		Engine::Point2D<int> pos(0,i*_distance);
		if(!_horizontalMovement)
		{
			pos = Engine::Point2D<int>(i*_distance, 0);
		}

		if(_boundaries.isInside(pos))
		{
			std::ostringstream oss;
			oss << "archaeologist_" << i;
			Archaeologist * newArchaeologist = new Archaeologist(oss.str(), _horizontalMovement, 100);
			newArchaeologist->setPosition(pos);
			//std::cout << " creating archaeologist: " << newArchaeologist << std::endl;
			addAgent(newArchaeologist);
		}
	}
}

Engine::Agent * Site::createAgentFromPackage( const std::string & type, void * package )
{
	ArchaeologistPackage * formattedPackage = 0;
	try
	{
		formattedPackage = (ArchaeologistPackage *)package;
	}
	catch(std::exception & e)
	{
		std::cout << "Site::createAgentFromPackage, wrong cast with exception: " << e.what() << std::endl;
	}
	return (Engine::Agent*)(new Archaeologist(*formattedPackage));
}

void * Site::createPackage( const std::string & type )
{
	return new ArchaeologistPackage;
}

void Site::registerTypes()
{
	_types.insert( std::make_pair( "archaeologist", registerArchaeologist()));
}

MPI_Datatype * Site::registerArchaeologist()
{
	ArchaeologistPackage package;
	int numVariables = 6;
	// we will send id, x and y
	int blockLengths[numVariables];
	MPI_Datatype typelist[numVariables];
	// 100 chars
	blockLengths[0] = 100;
	typelist[0] = MPI_CHAR;

	for(int i=1; i<numVariables; i++)
	{
		blockLengths[i] = 1;
		typelist[i] = MPI_INT;
	}
	MPI_Aint displacements[numVariables];
	displacements[0] = 0;

	MPI_Aint startAddress;
	MPI_Aint address;
	MPI_Address(package._id, &startAddress); 

	MPI_Address(&package._position._x, &address);
	displacements[1] = address-startAddress;
	MPI_Address(&package._position._y, &address);
	displacements[2] = address-startAddress;
	MPI_Address(&package._bulletsGathered, &address);
	displacements[3] = address-startAddress;
	MPI_Address(&package._horizontalMovement, &address);
	displacements[4] = address-startAddress;
	MPI_Address(&package._skill, &address);
	displacements[5] = address-startAddress;

	MPI_Datatype * newDataType = new MPI_Datatype;
	MPI_Type_struct(numVariables, blockLengths, displacements, typelist, newDataType);
	MPI_Type_commit(newDataType);
	return newDataType;
}

} // namespace BattleSim

