
#include "Survey.hxx"
#include "SurveyConfig.hxx"
#include "Archaeologist.hxx"

#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <Exception.hxx>
#include <Rectangle.hxx>

namespace BattleSim 
{
	
Survey::Survey( SurveyConfig * config, Engine::Scheduler * scheduler ) : World(config, scheduler, true)
{
}

Survey::~Survey()
{
}

void Survey::createRasters()
{
    const SurveyConfig & surveyConfig = (const SurveyConfig&)getConfig();

	_record.loadHDF5(surveyConfig._battleFile, true, false);
	registerDynamicRaster("originalRecord", true);
	registerDynamicRaster("degradedRecord", true);
	registerDynamicRaster("impacts", true);
	registerDynamicRaster("collectedBullets", true);
	registerDynamicRaster("exploredRasters", true);
	getDynamicRaster("originalRecord").setInitValues(0,std::numeric_limits<int>::max(), 0);
	getDynamicRaster("degradedRecord").setInitValues(0,std::numeric_limits<int>::max(), 0);
	getDynamicRaster("impacts").setInitValues(0,std::numeric_limits<int>::max(), 0);
	getDynamicRaster("collectedBullets").setInitValues(0,std::numeric_limits<int>::max(), 0);
	getDynamicRaster("exploredRasters").setInitValues(0,std::numeric_limits<int>::max(), 0);

    // copy of data
    Engine::DynamicRaster & originalRecord = _record.getDynamicRaster("originalRecord", _record.getNumSteps());
	Engine::DynamicRaster & impacts = _record.getDynamicRaster("impacts", _record.getNumSteps());
	Engine::DynamicRaster & degradedRecord = _record.getDynamicRaster("degradedRecord", _record.getNumSteps());
    for( auto index : getBoundaries())
    {
        setValue("originalRecord", index, originalRecord.getValue(index));
    	setValue("impacts", index, impacts.getValue(index));		
    	setValue("degradedRecord", index, degradedRecord.getValue(index));		
    }
}

void Survey::createAgents()
{
    const SurveyConfig & surveyConfig = (const SurveyConfig&)getConfig();

	int numArchaeologists = getConfig().getSize()._width/surveyConfig._distance;
    if(!surveyConfig._horizontal)
    {
        numArchaeologists = getConfig().getSize()._height/surveyConfig._distance;
    }

	for(int i=0; i<numArchaeologists; i++)
	{
		Engine::Point2D<int> pos(0,i*surveyConfig._distance);
		if(!surveyConfig._horizontal)
		{
			pos = Engine::Point2D<int>(i*surveyConfig._distance, 0);
		}

		if(getBoundaries().contains(pos))
		{
			std::ostringstream oss;
			oss << "Archaeologist_" << i;
			Archaeologist * newArchaeologist = new Archaeologist(oss.str(), 100);
			addAgent(newArchaeologist);
			newArchaeologist->setPosition(pos);
		}
	}
}

void Survey::stepEnvironment()
{
}

} // namespace BattleSim

