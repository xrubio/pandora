
#include "RemoveBulletWorld.hxx"

#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <Exception.hxx>
#include <Statistics.hxx>
#include <Rectangle.hxx>
#include <SimulationRecord.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>

#include "RemoveBulletConfig.hxx"

namespace BattleSim 
{

RemoveBulletWorld::RemoveBulletWorld( RemoveBulletConfig * config, Engine::Scheduler * scheduler ) : World(config, scheduler, false)
{
}

RemoveBulletWorld::~RemoveBulletWorld()
{
}

void RemoveBulletWorld::createRasters()
{
    const RemoveBulletConfig & removeConfig = (const RemoveBulletConfig &)getConfig();

	_record.loadHDF5(removeConfig._battleFile, true, false);
	registerDynamicRaster("originalRecord", true);
	registerDynamicRaster("degradedRecord", true);
	registerDynamicRaster("impacts", true);
	getDynamicRaster("originalRecord").setInitValues(0,std::numeric_limits<int>::max(), 0);
	getDynamicRaster("degradedRecord").setInitValues(0,std::numeric_limits<int>::max(), 0);
	getDynamicRaster("impacts").setInitValues(0,std::numeric_limits<int>::max(), 0);

    Engine::DynamicRaster & originalVirtualRecord = _record.getDynamicRaster("virtualRecord", _record.getNumSteps());
	Engine::DynamicRaster & originalImpacts = _record.getDynamicRaster("impacts", _record.getNumSteps());
    for( auto index : getBoundaries())
	{
    	setValue("originalRecord", index, originalVirtualRecord.getValue(index));
    	setValue("impacts", index, originalImpacts.getValue(index));		
        if(Engine::GeneralState::statistics().getUniformDistValue()<removeConfig._lossProbability)
        {
            setValue("degradedRecord", index, 0);
        }
        else
        {
    	    setValue("degradedRecord", index, originalVirtualRecord.getValue(index));		
        }
    }
}

} // namespace BattleSim

