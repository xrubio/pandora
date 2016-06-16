
#include <SequentialSerializer.hxx>
#include <boost/filesystem.hpp>
#include <World.hxx>
#include <Exception.hxx>
#include <Agent.hxx>
#include <Scheduler.hxx>
#include <Logger.hxx>
#include <StaticRaster.hxx>
#include <Config.hxx>

namespace Engine
{

SequentialSerializer::SequentialSerializer( const Scheduler & scheduler ) : _scheduler(scheduler)
{
}

SequentialSerializer::~SequentialSerializer()
{
}

void SequentialSerializer::init( World & world )
{
    _config = &world.getConfig();
	std::stringstream logName;
	logName << "SequentialSerializer_" << _scheduler.getId();
	log_DEBUG(logName.str(), " init sequential serializer");

	// check if directory exists
	unsigned int filePos = _config->getResultsFile().find_last_of("/");
	std::string path = _config->getResultsFile().substr(0,filePos+1);

	// create dir where logs will be stored if it is not already created
	if(!path.empty())
	{
		boost::filesystem::create_directory(path);
	}
	_fileId = H5Fcreate(_config->getResultsFile().c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	// adding a group with global generic data
	hsize_t simpleDimension = 1;
	hid_t globalFileSpace = H5Screate_simple(1, &simpleDimension, NULL);
	hid_t globalDatasetId = H5Dcreate(_fileId, "global", H5T_NATIVE_INT, globalFileSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	hid_t attributeFileSpace = H5Screate_simple(1, &simpleDimension, NULL);
	hid_t attributeId = H5Acreate(globalDatasetId, "numSteps", H5T_NATIVE_INT, attributeFileSpace, H5P_DEFAULT, H5P_DEFAULT);
	H5Awrite(attributeId, H5T_NATIVE_INT, &_config->getNumSteps());
	H5Sclose(attributeFileSpace);
	H5Aclose(attributeId);
	
	attributeFileSpace = H5Screate_simple(1, &simpleDimension, NULL);
	attributeId = H5Acreate(globalDatasetId, "serializerResolution", H5T_NATIVE_INT, attributeFileSpace, H5P_DEFAULT, H5P_DEFAULT);
	H5Awrite(attributeId, H5T_NATIVE_INT, &_config->getSerializeResolution());
	H5Sclose(attributeFileSpace);
	H5Aclose(attributeId);

	attributeFileSpace = H5Screate_simple(1, &simpleDimension, NULL);
	attributeId = H5Acreate(globalDatasetId, "width", H5T_NATIVE_INT, attributeFileSpace, H5P_DEFAULT, H5P_DEFAULT);
	H5Awrite(attributeId, H5T_NATIVE_INT, &_config->getSize()._width);
	H5Sclose(attributeFileSpace);
	H5Aclose(attributeId);

	attributeFileSpace = H5Screate_simple(1, &simpleDimension, NULL);
	attributeId = H5Acreate(globalDatasetId, "height", H5T_NATIVE_INT, attributeFileSpace, H5P_DEFAULT, H5P_DEFAULT);
	H5Awrite(attributeId, H5T_NATIVE_INT, &_config->getSize()._height);
	H5Sclose(attributeFileSpace);
	H5Aclose(attributeId);

	attributeFileSpace = H5Screate_simple(1, &simpleDimension, NULL);
	attributeId= H5Acreate(globalDatasetId, "numTasks", H5T_NATIVE_INT, attributeFileSpace, H5P_DEFAULT, H5P_DEFAULT);
	H5Awrite(attributeId, H5T_NATIVE_INT, &_scheduler.getNumTasks());
	H5Sclose(attributeFileSpace);
	H5Aclose(attributeId);

	log_INFO(logName.str(), _scheduler.getWallTime() << " id: " << _scheduler.getId() << " size: " << _config->getSize() << " num tasks: " << _scheduler.getNumTasks() << " serializer resolution:" << _config->getSerializeResolution() << " and steps: " << _config->getNumSteps());

	// we store the name of the rasters
	hid_t rasterNameFileSpace = H5Screate_simple(1, &simpleDimension, NULL);
	hid_t rasterNameDatasetId = H5Dcreate(_fileId, "rasters", H5T_NATIVE_INT, rasterNameFileSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	// dynamic raster, store every time step
	for(size_t i=0; i<world.getNumberOfRasters(); i++)
	{
		if(!world.rasterExists(i) || !world.rasterToSerialize(i) || !world.isRasterDynamic(i))
		{
			continue;
		}
		attributeFileSpace = H5Screate_simple(1, &simpleDimension, NULL);
		attributeId = H5Acreate(rasterNameDatasetId, world.getRasterName(i).c_str(), H5T_NATIVE_INT, attributeFileSpace, H5P_DEFAULT, H5P_DEFAULT);
		H5Sclose(attributeFileSpace);
		H5Aclose(attributeId);
	}
	H5Dclose(rasterNameDatasetId);

	// static raster, store just at the beginning of the simulation
	rasterNameDatasetId = H5Dcreate(_fileId, "staticRasters", H5T_NATIVE_INT, rasterNameFileSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	for(size_t i=0; i<world.getNumberOfRasters(); i++)
	{
		if(!world.rasterExists(i) || !world.rasterToSerialize(i) || world.isRasterDynamic(i))
		{
			continue;
		}
		attributeFileSpace = H5Screate_simple(1, &simpleDimension, NULL);
		attributeId = H5Acreate(rasterNameDatasetId, world.getRasterName(i).c_str(), H5T_NATIVE_INT, attributeFileSpace, H5P_DEFAULT, H5P_DEFAULT);
		H5Sclose(attributeFileSpace);
		H5Aclose(attributeId);
	}
	H5Dclose(rasterNameDatasetId);
	H5Sclose(rasterNameFileSpace);

	H5Dclose(globalDatasetId);
	H5Sclose(globalFileSpace);

	// color tables
	hid_t colorTableGroupId = H5Gcreate(_fileId, "colorTables", 0, H5P_DEFAULT, H5P_DEFAULT);
	for(size_t i=0; i<world.getNumberOfRasters(); i++)
	{
		if(!world.rasterExists(i) || !world.rasterToSerialize(i))
		{
			continue;
		}
		hsize_t numEntries[2];
		numEntries[0] = world.getStaticRaster(i).getNumColorEntries();		
		numEntries[1] = 4; 
		hid_t fileSpace = H5Screate_simple(2, numEntries, NULL); 
		
		hid_t rasterDatasetId = H5Dcreate(colorTableGroupId, world.getRasterName(i).c_str(), H5T_NATIVE_INT, fileSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		// if not color table continue
		if(numEntries[0] == 0)
		{
			H5Dclose(rasterDatasetId);
			continue;
		}

		hsize_t	offset[2];
    	offset[0] = 0;
	    offset[1] = 0;
		
		hsize_t	stride[2];
		stride[0] = 1;
		stride[1] = 1;
	
		hsize_t count[2];
		count[0] = 1;
		count[1] = 1;

		H5Sselect_hyperslab(fileSpace, H5S_SELECT_SET, offset, stride, count, numEntries);
		int * data = (int *) malloc(sizeof(int)*numEntries[0]*numEntries[1]);

		for(size_t z=0; z<numEntries[0]; z++)
		{
			// red
			size_t index = z*numEntries[1];
			int value = (int)(world.getStaticRaster(i).getColorEntry(z)._r);
			data[index] = value;

			// green
			index++;
			value = (int)(world.getStaticRaster(i).getColorEntry(z)._g);
			data[index] = value;
			
			// blue
			index++;
			value = (int)(world.getStaticRaster(i).getColorEntry(z)._b);
			data[index] = value;

			// alpha
			index++;
			value = (int)(world.getStaticRaster(i).getColorEntry(z)._alpha);
			data[index] = value;
		}
	    // Create property list for collective dataset write.
		hid_t propertyListId = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(propertyListId, H5FD_MPIO_INDEPENDENT);
	    hid_t memorySpace = H5Screate_simple(2, numEntries, NULL);
		H5Dwrite(rasterDatasetId, H5T_NATIVE_INT, memorySpace, fileSpace, propertyListId, data);

		free(data);
		H5Pclose(propertyListId);
	    H5Sclose(memorySpace);	
		H5Sclose(fileSpace);
		H5Dclose(rasterDatasetId);
	}
	H5Gclose(colorTableGroupId);

	// creating a file with the agents of each computer node
	std::ostringstream oss;
	if(!path.empty())
	{
		oss << path << "/";
	}
	oss << "agents-" << _scheduler.getId() << ".abm";

	_agentsFileId = H5Fcreate(oss.str().c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	
	//the real size of the matrix is sqrt(num simulator)*matrixsize	
	hsize_t dimensions[2];
	dimensions[0] = hsize_t(_config->getSize()._height);
	dimensions[1] = hsize_t(_config->getSize()._width);

	// static rasters	
	for(size_t i=0; i<world.getNumberOfRasters(); i++)
	{
		if(!world.rasterExists(i) || !world.rasterToSerialize(i) || world.isRasterDynamic(i))
		{
			continue;
		}
		// TODO 0 o H5P_DEFAULT??
		hid_t rasterGroupId = H5Gcreate(_fileId, world.getRasterName(i).c_str(), 0, H5P_DEFAULT, H5P_DEFAULT);
		hid_t fileSpace = H5Screate_simple(2, dimensions, NULL); 
		hid_t datasetId = H5Dcreate(rasterGroupId, "values", H5T_NATIVE_INT, fileSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5Dclose(datasetId);
		H5Sclose(fileSpace);
		H5Gclose(rasterGroupId);
	}

	// dynamic rasters
	for(size_t i=0; i<world.getNumberOfRasters(); i++)
	{
		if(!world.rasterExists(i) || !world.rasterToSerialize(i) || !world.isRasterDynamic(i))
		{
			continue;
		}	
		hid_t rasterGroupId = H5Gcreate(_fileId, world.getRasterName(i).c_str(), 0, H5P_DEFAULT, H5P_DEFAULT);
		for(int i=0; i<=_config->getNumSteps(); i++)
		{  
			if(i%_config->getSerializeResolution()!=0)
			{
				continue;
			}
			std::ostringstream oss;
			oss << "step" << i;
			hid_t stepFileSpace = H5Screate_simple(2, dimensions, NULL); 
			hid_t stepDatasetId = H5Dcreate(rasterGroupId, oss.str().c_str(), H5T_NATIVE_INT, stepFileSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			H5Dclose(stepDatasetId);
			H5Sclose(stepFileSpace);
		}	
		H5Gclose(rasterGroupId);
	}

	StaticRastersRefMap staticRasters;
	for(size_t i=0; i<world.getNumberOfRasters(); i++)
	{
		if(!world.rasterExists(i) || !world.rasterToSerialize(i))
		{
			continue;
		}

		if(!world.isRasterDynamic(i))
		{
			staticRasters.insert(std::make_pair(world.getRasterName(i), &world.getStaticRaster(i)));
		}
		else
		{
			_dynamicRasters.insert(std::make_pair(world.getRasterName(i), &world.getStaticRaster(i)));
		}
	}
	serializeStaticRasters(staticRasters);
}

void SequentialSerializer::serializeAgents( const int & step, const AgentsList::const_iterator & beginAgents, const AgentsList::const_iterator & endAgents )
{
	int i=0;
	for(AgentsList::const_iterator it=beginAgents; it!=endAgents; it++)
	{
		if(!(*it)->exists())
		{
			continue;
		}
		serializeAgent(it->get(), step, i);
		i++;
	}
	// serialize remaining agents
	finishAgentsSerialization(step);
}

void SequentialSerializer::serializeAgent( Agent * agent, const int & step, int index )
{
	std::string type = agent->getType();
	// new type, must be in _stringAttributes because at least id attribute must exist
	if(_stringAttributes.find(type)==_stringAttributes.end())
	{
		agent->registerAttributes();
		registerType(agent);
	}

	addStringAttribute(type, "id", agent->getId());
	addIntAttribute(type, "x", agent->getPosition()._x);
	addIntAttribute(type, "y", agent->getPosition()._y);
	agent->serialize();

	if(getDataSize(type)>=20000)
	{
		executeAgentSerialization(type, step);
	}
}

void SequentialSerializer::finish()
{
	H5Fclose(_fileId);
	H5Fclose(_agentsFileId);
}

void SequentialSerializer::finishAgentsSerialization( int step)
{
	for(std::map<std::string, int>::iterator it=_agentIndexMap.begin(); it!=_agentIndexMap.end(); it++)
	{
		executeAgentSerialization(it->first, step);
	}
	resetCurrentIndexs();
}

void SequentialSerializer::executeAgentSerialization( const std::string & type, int step)	
{
	std::map<std::string, int>::iterator itI = _agentIndexMap.find(type);

	int currentIndex = itI->second;	

	hsize_t	offset[1];
    offset[0] = currentIndex;
 	
	hsize_t	stride[1];
	stride[0] = 1;
	
	hsize_t count[1];
	count[0] = 1;

	IntAttributesMap::iterator it = _intAttributes.find(type);
	IntMap * attributes = it->second;	
	for(IntMap::iterator itM=attributes->begin(); itM!=attributes->end(); itM++)
	{
		std::vector<int> * data = itM->second;
		// nothing to serialize
		if(data->size()==0)
		{
			return;
		}
		hsize_t	block[1];
		block[0] = data->size();
		
		hsize_t simpleDimension = data->size();
		// TODO es repeteix per cada atribut
		hsize_t newSize[1];
		newSize[0] = currentIndex+data->size();

		std::ostringstream oss;
		oss << type << "/step" << step << "/" << itM->first;
		hid_t datasetId = H5Dopen(_agentsFileId, oss.str().c_str(), H5P_DEFAULT);
		H5Dset_extent( datasetId, newSize);
		hid_t fileSpace = H5Dget_space(datasetId);
		H5Sselect_hyperslab(fileSpace, H5S_SELECT_SET, offset, stride, count, block);
  		hid_t memorySpace = H5Screate_simple(1, &simpleDimension, 0);
		H5Dwrite(datasetId, H5T_NATIVE_INT, memorySpace, fileSpace, H5P_DEFAULT, &(data->at(0)));
		data->clear();

		H5Sclose(memorySpace);
		H5Sclose(fileSpace);
		H5Dclose(datasetId);
	}
    
    FloatAttributesMap::iterator itF = _floatAttributes.find(type);
	FloatMap * attributesF = itF->second;	
	for(FloatMap::iterator itM=attributesF->begin(); itM!=attributesF->end(); itM++)
	{
		std::vector<float> * data = itM->second;
		// nothing to serialize
		if(data->size()==0)
		{
			return;
		}
		hsize_t	block[1];
		block[0] = data->size();
		
		hsize_t simpleDimension = data->size();
		// TODO es repeteix per cada atribut
		hsize_t newSize[1];
		newSize[0] = currentIndex+data->size();

		std::ostringstream oss;
		oss << type << "/step" << step << "/" << itM->first;
		hid_t datasetId = H5Dopen(_agentsFileId, oss.str().c_str(), H5P_DEFAULT);
		H5Dset_extent( datasetId, newSize);
		hid_t fileSpace = H5Dget_space(datasetId);
		H5Sselect_hyperslab(fileSpace, H5S_SELECT_SET, offset, stride, count, block);
  		hid_t memorySpace = H5Screate_simple(1, &simpleDimension, 0);
		H5Dwrite(datasetId, H5T_NATIVE_FLOAT, memorySpace, fileSpace, H5P_DEFAULT, &(data->at(0)));
		data->clear();

		H5Sclose(memorySpace);
		H5Sclose(fileSpace);
		H5Dclose(datasetId);
	}
	
	StringAttributesMap::iterator itS = _stringAttributes.find(type);
	StringMap * attributesS = itS->second;	
	for(StringMap::iterator itM=attributesS->begin(); itM!=attributesS->end(); itM++)
	{
		std::vector<std::string> * data = itM->second;
		hsize_t	block[1];
		block[0] = data->size();
		
		hsize_t simpleDimension = data->size();
		// TODO es repeteix per cada atribut
		hsize_t newSize[1];
		newSize[0] = currentIndex+data->size();
		itI->second = currentIndex+data->size();

		std::ostringstream oss;
		oss << type << "/step" << step << "/" << itM->first;

		hid_t datasetId = H5Dopen(_agentsFileId, oss.str().c_str(), H5P_DEFAULT);
		H5Dset_extent( datasetId, newSize);
		hid_t fileSpace = H5Dget_space(datasetId);
		H5Sselect_hyperslab(fileSpace, H5S_SELECT_SET, offset, stride, count, block);
		hid_t idType = H5Tcopy(H5T_C_S1);
		H5Tset_size (idType, H5T_VARIABLE);
  		hid_t memorySpace = H5Screate_simple(1, &simpleDimension, 0);
		H5Dwrite(datasetId, idType, memorySpace, fileSpace, H5P_DEFAULT, &(data->at(0)));
		data->clear();

		H5Sclose(memorySpace);
		H5Sclose(fileSpace);
		H5Dclose(datasetId);
	}
}

void SequentialSerializer::addStringAttribute( const std::string & type, const std::string & key, const std::string & value )
{
	StringAttributesMap::iterator it = _stringAttributes.find(type);
	StringMap * stringMap = it->second;
	StringMap::iterator itS = stringMap->find(key);
	std::vector<std::string> * stringVector = itS->second;
	stringVector->push_back(value);
}
	
void SequentialSerializer::addIntAttribute( const std::string & type, const std::string & key, int value )
{
	IntAttributesMap::iterator it = _intAttributes.find(type);
	if(it==_intAttributes.end())
	{
		std::stringstream oss;
		oss << "SequentialSerializer::addIntAttribute - looking for unknown agent type: " << type;
		throw Exception(oss.str());
	}
	IntMap * intMap = it->second;
	IntMap::iterator itI = intMap->find(key);
	if(itI==intMap->end())
	{
		std::stringstream oss;
		oss << "SequentialSerializer::addIntAttribute - looking for unknown attribute: " << key << " in agent type: " << type;
		throw Exception(oss.str());
	}
	std::vector<int> * intVector = itI->second;
	intVector->push_back(value);
}

void SequentialSerializer::addFloatAttribute( const std::string & type, const std::string & key, float value )
{
	FloatAttributesMap::iterator it = _floatAttributes.find(type);
	if(it==_floatAttributes.end())
	{
		std::stringstream oss;
		oss << "SequentialSerializer::addFloatAttribute - looking for unknown agent type: " << type;
		throw Exception(oss.str());
	}
	FloatMap * floatMap = it->second;
	FloatMap::iterator itI = floatMap->find(key);
	if(itI==floatMap->end())
	{
		std::stringstream oss;
		oss << "SequentialSerializer::addFloatAttribute - looking for unknown attribute: " << key << " in agent type: " << type;
		throw Exception(oss.str());
	}
	std::vector<float> * floatVector = itI->second;
	floatVector->push_back(value);
}

void SequentialSerializer::registerType( Agent * agent )
{
	std::string type = agent->getType();

	std::stringstream logName;
	logName << "SequentialSerializer_" << _scheduler.getId();

	log_DEBUG(logName.str(), "registering new type: " << type);

	hsize_t simpleDimension = 0;
	hsize_t maxDims[1];
	maxDims[0] = H5S_UNLIMITED;
	hid_t agentFileSpace = H5Screate_simple(1, &simpleDimension, maxDims);
	hid_t agentTypeGroup = H5Gcreate(_agentsFileId, agent->getType().c_str(),  0, H5P_DEFAULT, H5P_DEFAULT);

	hid_t propertyListId = H5Pcreate(H5P_DATASET_CREATE);
	hsize_t chunks = 1;
	H5Pset_chunk(propertyListId, 1, &chunks);

	IntMap * newTypeIntMap = new IntMap;
    FloatMap * newTypeFloatMap = new FloatMap;
	StringMap * newTypeStringMap = new StringMap;

	// create a dataset for each timestep
	for(int i=0; i<=_config->getNumSteps(); i++)
	{
		if(i%_config->getSerializeResolution()!=0)
		{
			continue;
		}

		std::ostringstream oss;
		oss<<"step"<<i;
		hid_t stepGroup = H5Gcreate(agentTypeGroup, oss.str().c_str(),  0, H5P_DEFAULT, H5P_DEFAULT);
		for(Agent::AttributesList::iterator it=agent->beginIntAttributes(); it!=agent->endIntAttributes(); it++)
		{	
			log_DEBUG(logName.str(), "\tnew int attribute: " << *it);
			newTypeIntMap->insert( make_pair(*it, new std::vector<int>() ));
			hid_t idDataset= H5Dcreate(stepGroup, (*it).c_str(), H5T_NATIVE_INT, agentFileSpace, H5P_DEFAULT, propertyListId, H5P_DEFAULT);
            if(idDataset<0)
            {
                std::stringstream ossErr;
        		ossErr << "SequentialSerializer::registerType - dataset not created for agent: " << agent << " step group: " << oss.str() << " and int attribute: " << *it;
        		throw Exception(ossErr.str());
            }   
			H5Dclose(idDataset);
		}      
        
        for(Agent::AttributesList::iterator it=agent->beginFloatAttributes(); it!=agent->endFloatAttributes(); it++)
		{	
			log_DEBUG(logName.str(), "\tnew float attribute: " << *it);
			newTypeFloatMap->insert( make_pair(*it, new std::vector<float>() ));
			hid_t idDataset= H5Dcreate(stepGroup, (*it).c_str(), H5T_NATIVE_FLOAT, agentFileSpace, H5P_DEFAULT, propertyListId, H5P_DEFAULT);    
            if(idDataset<0)
            {
                std::stringstream ossErr;
        		ossErr<< "SequentialSerializer::registerType - dataset not created for agent: " << agent << " step group: " << oss.str() << " and float attribute: " << *it;
        		throw Exception(ossErr.str());
            }
			H5Dclose(idDataset);
		}

		
		hid_t idType = H5Tcopy(H5T_C_S1);
		H5Tset_size (idType, H5T_VARIABLE);
		for(Agent::AttributesList::iterator it=agent->beginStringAttributes(); it!=agent->endStringAttributes(); it++)
		{		
			log_DEBUG(logName.str(), "\tnew string attribute: " << *it);
			newTypeStringMap->insert( make_pair(*it, new std::vector<std::string>() ));
			hid_t idDataset= H5Dcreate(stepGroup, (*it).c_str(), idType, agentFileSpace, H5P_DEFAULT, propertyListId, H5P_DEFAULT);  
            if(idDataset<0)
            {
                std::stringstream ossErr;
        		ossErr<< "SequentialSerializer::registerType - dataset not created for agent: " << agent << " step group: " << oss.str() << " and string attribute: " << *it;
        		throw Exception(ossErr.str());
            }
			H5Dclose(idDataset);
		}
		H5Gclose(stepGroup);
	}
	H5Gclose(agentTypeGroup);
	H5Sclose(agentFileSpace);

	_agentIndexMap.insert( make_pair(type, 0) );
	_intAttributes.insert( make_pair(type, newTypeIntMap));
	_floatAttributes.insert( make_pair(type, newTypeFloatMap));
	_stringAttributes.insert( make_pair(type, newTypeStringMap));
}

int SequentialSerializer::getDataSize( const std::string & type )
{
	StringAttributesMap::iterator it = _stringAttributes.find(type);
	StringMap * stringMap = it->second;
	StringMap::iterator itS = stringMap->find("id");
	return itS->second->size();
}

void SequentialSerializer::resetCurrentIndexs()
{
	for(std::map<std::string, int>::iterator it=_agentIndexMap.begin(); it!=_agentIndexMap.end(); it++)
	{
		it->second = 0;
	}
}

void SequentialSerializer::serializeStaticRasters( const StaticRastersRefMap & staticRasters)
{
	for(StaticRastersRefMap::const_iterator it=staticRasters.begin(); it!=staticRasters.end(); it++)
	{
		std::ostringstream oss;
		oss << "/" << it->first << "/values";
		serializeRaster(*it->second, oss.str());
	}
}

void SequentialSerializer::serializeRaster( const StaticRaster & raster, const std::string & datasetKey )
{
	std::stringstream logName;
	logName << "SequentialSerializer_" << _scheduler.getId();
	log_EDEBUG(logName.str(), "serializing raster: " << datasetKey);

	hid_t dataSetId = H5Dopen(_fileId, datasetKey.c_str(), H5P_DEFAULT);
	hid_t fileSpace = H5Dget_space(dataSetId);

	const hsize_t boundariesWidth = _scheduler.getBoundaries()._size._width;
	const hsize_t boundariesHeight = _scheduler.getBoundaries()._size._height;
 
	hsize_t	block[2];
	block[0] = boundariesHeight;
	block[1] = boundariesWidth;
	
	int * data = (int *) malloc(sizeof(int)*block[0]*block[1]);
	size_t index = 0;
	for(size_t y=0; y<boundariesHeight; y++)
	{
		for(size_t x=0; x<boundariesWidth; x++)
		{	
			log_EDEBUG(logName.str(), "index: " << x << "/" << y << " - " << index);
			data[index] = raster.getValue(Point2D<int>(x,y));
			log_EDEBUG(logName.str(), "value: " << data[index]);
			++index;
		}
	}
    // Create property list for collective dataset write.
	hid_t propertyListId = H5Pcreate(H5P_DATASET_XFER);
	H5Pset_dxpl_mpio(propertyListId, H5FD_MPIO_INDEPENDENT);
    hid_t memorySpace = H5Screate_simple(2, block, NULL);
	H5Dwrite(dataSetId, H5T_NATIVE_INT, memorySpace, fileSpace, propertyListId, data);

	free(data);
	H5Pclose(propertyListId);
    H5Sclose(memorySpace);	
	H5Sclose(fileSpace);
	H5Dclose(dataSetId);
	log_EDEBUG(logName.str(), "serializing raster: " << datasetKey << " done");
}

void SequentialSerializer::serializeRasters(int step)
{
	for(StaticRastersRefMap::const_iterator it=_dynamicRasters.begin(); it!=_dynamicRasters.end(); it++)
	{
		std::ostringstream oss;
		oss << "/" << it->first << "/step" << step;
		serializeRaster(*it->second, oss.str());
	}
}

} // namespace Engine

