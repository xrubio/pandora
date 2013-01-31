
#include "RemoveBulletWorld.hxx"

#include "Raster.hxx"
#include "Point2D.hxx"
#include "Exceptions.hxx"
#include "Statistics.hxx"
#include "Rectangle.hxx"
#include "SimulationRecord.hxx"

#include <cmath>

namespace BattleSim 
{

RemoveBulletWorld::RemoveBulletWorld( const std::string & path, const std::string & battlefieldFile, int percentage ) : World(0, 0, 0, false, path+"/degradedBattlefield.h5"), _percentage(percentage)
{
	_record.loadHDF5(battlefieldFile, true, false);
	_globalRasterSize = _record.getSize();
	_globalBoundaries = Engine::Rectangle<int>(Engine::Point2D<int>(0,0), Engine::Point2D<int>(_globalRasterSize, _globalRasterSize));
}

RemoveBulletWorld::~RemoveBulletWorld()
{
}

void RemoveBulletWorld::createRasters()
{
	registerRaster("originalRecord");
	registerRaster("degradedRecord");
	registerRaster("impacts");

	Engine::Raster & originalVirtualRecord = _record.getRaster("virtualRecord", _record.getNumSteps());
	// copiar dades 
	Engine::Point2D<int> index;
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)			
		{
			setMaxValue("originalRecord", index, originalVirtualRecord.getValue(index));
			setValue("originalRecord", index, originalVirtualRecord.getValue(index));
		}
	}
	
	Engine::Raster & originalImpacts = _record.getRaster("impacts", _record.getNumSteps());
	// copiar dades 
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)			
		{
			setMaxValue("impacts", index, originalImpacts.getValue(index));
			setValue("impacts", index, originalImpacts.getValue(index));
		}
	}
	
	// execute removal
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)			
		{
			if(_statistics.getUniformDistValue(0, 100)<_percentage)
			{
				setMaxValue("degradedRecord", index, 0);
				setValue("degradedRecord", index, 0);
			}
			else
			{
				setMaxValue("degradedRecord", index, originalVirtualRecord.getValue(index));
				setValue("degradedRecord", index, originalVirtualRecord.getValue(index));
			}
		}
	}
}

void RemoveBulletWorld::createAgents()
{
}

Engine::Agent * RemoveBulletWorld::createAgentFromPackage( const std::string & type, void * package )
{
	return 0;
}

void * RemoveBulletWorld::createPackage( const std::string & type )
{
	return 0;
}

void RemoveBulletWorld::registerTypes()
{
}
	
} // namespace BattleSim

