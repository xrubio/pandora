
#include <analysis/AgentHDFtoSHP.hxx>
#include <AgentRecord.hxx>
#include <SimulationRecord.hxx>
#include <ogrsf_frmts.h>
#include <Exception.hxx>
#include <sstream>
#include <boost/filesystem.hpp>
#include <iomanip>

namespace PostProcess 
{

AgentHDFtoSHP::AgentHDFtoSHP( const Engine::Point2D<int> & origin, float resolution, const std::string & srs, int numStep ) : Output(";"), _origin(origin), _resolution(resolution), _srs(srs), _numStep(numStep), _definitionComplete(false)
{
}

AgentHDFtoSHP::~AgentHDFtoSHP()
{
}

void AgentHDFtoSHP::preProcess(const Engine::SimulationRecord & , const std::string & outputFile)
{
	// check if directory exists
	unsigned int filePos = outputFile.find_last_of("/");
	std::string path = outputFile.substr(0,filePos+1);
	if(path.compare("")!=0)
	{
		// create dir where data will be stored if it is not already created
		boost::filesystem::create_directory(path);
	}
	// delete the file if it already exists
	boost::filesystem::remove(outputFile);

	std::string driverName("ESRI Shapefile");
	OGRRegisterAll();	
	
	OGRSFDriver * driver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(driverName.c_str());
	if(!driver)
	{
		std::stringstream oss;
		oss << "AgentHDFtoSHP::preProcess - driver: " << driverName << " not available.";
		throw Engine::Exception(oss.str());
		return;
	}
	_dataSource = driver->CreateDataSource(outputFile.c_str(), 0);
	if(!_dataSource)
	{	
		std::stringstream oss;
		oss << "AgentHDFtoSHP::preProcess - unable to create new data source for file: " << outputFile;
		throw Engine::Exception(oss.str());
		return;
	}
	OGRSpatialReference * projection = 0;
	if(_srs.compare("no specified")!=0)
	{
		projection = new OGRSpatialReference(_srs.c_str());
	}
	_layer = _dataSource->CreateLayer("base", projection, wkbPoint, 0);
	if(!_layer)
	{
		std::stringstream oss;
		oss << "AgentHDFtoSHP::preProcess - unable to create layer for data source in file: " << outputFile;
		throw Engine::Exception(oss.str());
		return;
	}

	// TODO write all the data
	// id
	OGRFieldDefn fieldId( "id", OFTString);
	fieldId.SetWidth(32);
    if( _layer->CreateField( &fieldId) != OGRERR_NONE )
    {
		std::stringstream oss;
		oss << "AgentHDFtoSHP::preProcess - unable to create field id for file: " << outputFile;
		throw Engine::Exception(oss.str());
		return;
    }
	if(_numStep==-1)
	{
		OGRFieldDefn fieldNumStep("num step", OFTInteger);
		fieldNumStep.SetWidth(32);
	    if( _layer->CreateField( &fieldNumStep) != OGRERR_NONE )
    	{
			std::stringstream oss;
			oss << "AgentHDFtoSHP::preProcess - unable to create field num step for file: " << outputFile;
			throw Engine::Exception(oss.str());
			return;
    	}
	}
	if(projection)
	{
		delete projection;
	}
}

void AgentHDFtoSHP::createFeature( const Engine::AgentRecord & agentRecord, int timeStep, bool storeTimeStep )
{
    /* TODO
	// if the agent was not alive in this num step don't export it
	if(agentRecord.getInt(timeStep, "exists")==0)
	{
		return;
	}
	// id
	OGRFeature * feature = OGRFeature::CreateFeature(_layer->GetLayerDefn());
	feature->SetField("id", agentRecord.getId().c_str());

	if(storeTimeStep)
	{
		feature->SetField("num step", timeStep);
	}
	// list of data
	for(Engine::AgentRecord::StatesMap::const_iterator it=agentRecord.beginStates(); it!=agentRecord.endStates(); it++)
	{
		const std::string & key = it->first;

		// id, exists, x and y are already managed
		if(key.compare("id")==0 || key.compare("x")==0 || key.compare("y")==0 || key.compare("exists")==0)
		{
			continue;
		}

		//std::cout << "setting field: " << getFieldNameFromString(key) << " state: " <<  agentRecord.getState(timeStep, key) << " for key: " << key << std::endl;
		feature->SetField(getFieldNameFromString(key).c_str(), agentRecord.getState(timeStep, key));
	}

	// position
	OGRPoint point;
	float newX = _origin._x+_resolution*agentRecord.getState(timeStep, "x");	
	float newY = _origin._y-_resolution*agentRecord.getState(timeStep, "y");
	//std::cout << std::setprecision(10) << agentRecord.getState(timeStep, "x") << "/" << agentRecord.getState(timeStep, "y") << " to: " << newX << "/" << newY << std::endl;
	point.setX(newX);
	point.setY(newY);
	feature->SetGeometry(&point);
	
	// register feature in layer
	if(_layer->CreateFeature(feature)!=OGRERR_NONE)
	{	
		std::stringstream oss;
		oss << "AgentHDFtoSHP::preProcess - unable to create feature for agent with id: " << agentRecord.getId();
		throw Engine::Exception(oss.str());
		return;
	}
	OGRFeature::DestroyFeature(feature);
    */
	return;
}

std::string AgentHDFtoSHP::getFieldNameFromString( const std::string & fieldName )
{
	if(fieldName.size()<=10)
	{
		return fieldName;
	}

	std::string shortFieldName = fieldName;
	shortFieldName.resize(10);

	// strip white spaces at the end of the string
	for(int i=9; i>=0; i--)
	{
		if(shortFieldName.at(i)==' ')
		{
			shortFieldName.erase(i);
		}
		else
		{
			break;
		}
	}
	return shortFieldName;
}

void AgentHDFtoSHP::defineFields( const Engine::AgentRecord & agentRecord )
{
    /* TODO
	for(Engine::AgentRecord::StatesMap::const_iterator it=agentRecord.beginStates(); it!=agentRecord.endStates(); it++)
	{
		const std::string & key = it->first;		
		// id, exists, x and y are already managed
		if(key.compare("id")==0 || key.compare("x")==0 || key.compare("y")==0 || key.compare("exists")==0)
		{
			continue;
		}

		// TODO different types
		OGRFieldDefn newField(getFieldNameFromString(key).c_str(), OFTInteger);
		newField.SetWidth(32);
		if( _layer->CreateField( &newField) != OGRERR_NONE )
    	{
			std::stringstream oss;
			oss << "AgentHDFtoSHP::defineFields - unable to create field: " << getFieldNameFromString(key);
			throw Engine::Exception(oss.str());
			return;
    	}
	}
	_definitionComplete = true;
    */
}

void AgentHDFtoSHP::computeAgent( const Engine::AgentRecord & agentRecord )
{
	// check if definition is complete
	if(!_definitionComplete)
	{
		defineFields(agentRecord);
	}

	if(_numStep!=-1)
	{
		createFeature(agentRecord, _numStep, false);
		return;
	}

	for(int i=0; i<agentRecord.getNumSteps(); i++)
	{
		createFeature(agentRecord, i, true);
	}
}

void AgentHDFtoSHP::postProcess(const Engine::SimulationRecord & , const std::string & )
{
	OGRDataSource::DestroyDataSource(_dataSource);
}

std::string AgentHDFtoSHP::getName() const
{
	return "Shapefile parser";
}


} // namespace PostProcess

