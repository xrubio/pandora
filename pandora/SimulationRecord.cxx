
/*
 * Copyright (c) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software; 
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or (at your option) any later version.
 * 
 * Pandora is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <SimulationRecord.hxx>
#include <Exceptions.hxx>
#include <GeneralState.hxx>
#include <RasterLoader.hxx>

#include <hdf5.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <limits>

namespace Engine 
{

std::list<std::string> SimulationRecord::_agentTypes;
std::list<std::string> SimulationRecord::_agentAttributes;

SimulationRecord::SimulationRecord( int resolution, bool gui) : _name("unknown"), _numSteps(0), _loadingStep(0), _resolution(resolution), _gui(gui), _loadingPercentageDone(0.0f), _loadingState("no load")
{	
}

SimulationRecord::~SimulationRecord()
{
}

bool SimulationRecord::loadHDF5( const std::string & fileName, const bool & loadRasters, const bool & loadAgents )
{
	_loadingPercentageDone = 0.0f;
	// TODO it deletes agent records?
	for(AgentTypesMap::iterator it = _types.begin(); it!=_types.end(); it++)
	{
		it->second.clear();
	}
	_types.clear();

	_resources.clear();

	_loadingState = "loading file: "+fileName+"...";
	if(!_gui)
	{
		std::cout << _loadingPercentageDone << "% - " << _loadingState  << std::endl;
	}
	_name = fileName;
	hid_t fileId = H5Fopen(fileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
	if(fileId<0)
	{
		return false;
	}

	hid_t datasetId = H5Dopen(fileId, "global", H5P_DEFAULT); 
	hid_t attributeId = H5Aopen_name(datasetId, "numSteps");
	H5Aread(attributeId, H5T_NATIVE_INT, &_numSteps);
	H5Aclose(attributeId);

	int numStepsToLoad = 1+_numSteps/_resolution;
	// we need this information in order to open agents records
	attributeId = H5Aopen_name(datasetId, "numTasks");	
	int numTasks= 0;
	H5Aread(attributeId, H5T_NATIVE_INT, &numTasks);
	H5Aclose(attributeId);
	if(numTasks==0)
	{
		H5Dclose(datasetId);
		H5Fclose(fileId);
		// TODO throw exception
		return false;
	}

	attributeId = H5Aopen_name(datasetId, "size");	
	_size = 0;
	H5Aread(attributeId, H5T_NATIVE_INT, &_size );
	H5Aclose(attributeId);

	H5Dclose(datasetId);


	_loadingPercentageDone = 10.0f;
	// TODO refactor and split in two different methods
	if(loadRasters)
	{
		// static rasters
		hid_t staticRasterNamesDatasetId = H5Dopen(fileId, "staticRasters", H5P_DEFAULT);

		int numStaticRasters = H5Aget_num_attrs(staticRasterNamesDatasetId);
		for(int i=0; i<numStaticRasters; i++)
		{
			char nameAttribute[256];
			attributeId= H5Aopen_idx(staticRasterNamesDatasetId, i);
			H5Aget_name(attributeId, 256, nameAttribute);
			_staticRasters.insert( make_pair( std::string(nameAttribute), StaticRaster()));
			H5Aclose(attributeId);
		}
		H5Dclose(staticRasterNamesDatasetId);

		_loadingPercentageDone = 15.0f;
		for(StaticRasterMap::iterator it=_staticRasters.begin(); it!=_staticRasters.end(); it++)
		{
			_loadingState = "loading static raster: "+it->first;
			if(!_gui)
			{
				std::cout << _loadingPercentageDone << "% - " << _loadingState << std::endl;
			}
			GeneralState::rasterLoader().fillHDF5Raster(it->second, fileName, it->first );
		}

		// dynamic rasters
		hid_t rasterNamesDatasetId = H5Dopen(fileId, "rasters", H5P_DEFAULT);
		int numRasters = H5Aget_num_attrs(rasterNamesDatasetId);
		if(numRasters!=0)
		{
			for(int i=0; i<numRasters; i++)
			{
				char nameAttribute[256];
				attributeId= H5Aopen_idx(rasterNamesDatasetId, i);
				H5Aget_name(attributeId, 256, nameAttribute);
				_resources.insert( make_pair( std::string(nameAttribute), RasterHistory()));
				getRasterHistory(nameAttribute).resize(numStepsToLoad);
				H5Aclose(attributeId);
			}
			H5Dclose(rasterNamesDatasetId);

			_loadingPercentageDone = 20.0f; // from 20 to 50
			float increase = 30.0f/(numRasters*numStepsToLoad);
			for(RasterMap::iterator it=_resources.begin(); it!=_resources.end(); it++)
			{
				_loadingState = "loading dynamic raster: "+it->first;
				if(!_gui)
				{
					std::cout << _loadingPercentageDone << "% - " << _loadingState << std::endl;
				}
				int maxValue = std::numeric_limits<int>::min();
				int minValue = std::numeric_limits<int>::max();
				for(int i=0; i<=_numSteps; i=i+_resolution)
				{
					std::stringstream line;
					line << "loading raster " << it->first << " - step: " << i << "/" << _numSteps;
					_loadingState = line.str();
					if(!_gui)
					{
						std::cout << _loadingPercentageDone << "% - " << _loadingState << std::endl;
					}

					std::ostringstream oss;
					oss << "/" << it->first << "/step" << i;
					hid_t dset_id = H5Dopen(fileId, oss.str().c_str(), H5P_DEFAULT);
					hid_t dataspaceId = H5Dget_space(dset_id);
					hsize_t dims[2];
					H5Sget_simple_extent_dims(dataspaceId, dims, NULL);
					H5Sclose(dataspaceId);

					int * dset_data = (int*)malloc(sizeof(int)*dims[0]*dims[1]);
					
					// squared	
					Engine::Raster & raster = it->second[i/_resolution];
					raster.resize(Engine::Point2D<int>(dims[0], dims[1]));
					// TODO max value!
					raster.setInitValues(std::numeric_limits<int>::min(),std::numeric_limits<int>::max(), 0);

					H5Dread(dset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset_data);
					H5Dclose(dset_id);

					for(int i=0; i<dims[0]; i++)
					{
						for(int j=0; j<dims[1]; j++)
						{
							int index = i+j*dims[0];
							int value = dset_data[index];
							if(value>raster.getMaxValue())
							{
								raster.setMaxValue(value);
							}
							if(value>maxValue)
							{
								maxValue = value;
							}
							if(value<raster.getMinValue())
							{
								raster.setMinValue(value);
							}
							if(value<minValue)
							{
								minValue = value;
							}
							raster.setMaxValue(Engine::Point2D<int>(i,j), value);
							raster.setValue(Engine::Point2D<int>(i,j), value); 
						}
					}
					free(dset_data);
					_loadingPercentageDone += increase;
				}
				// setting max value for the entire simulation
				for(int i=0; i<=_numSteps; i=i+_resolution)
				{
					Engine::Raster & raster = it->second[i/_resolution];
					raster.setMaxValue(maxValue);
					raster.setMinValue(minValue);
				}
			}
		}
	}
	H5Fclose(fileId);

	_loadingPercentageDone = 50.0f;
	_loadingState = "loading agents";
	if(!_gui)
	{
		std::cout << _loadingPercentageDone << "% - " << _loadingState << std::endl;
	}

	if(loadAgents)
	{
		unsigned int filePos = fileName.find_last_of("/");
		std::string path = fileName.substr(0,filePos+1);
		loadAgentsFiles(path, numStepsToLoad, numTasks);
	}
	_loadingState = "no loading";
	_loadingPercentageDone = 100.0f;
	return true;
}

void SimulationRecord::registerAgentTypes( const hid_t & rootGroup )
{
	H5Literate(rootGroup, H5_INDEX_NAME, H5_ITER_INC, 0, iterateAgentTypes, 0);
	for(std::list<std::string>::iterator it=_agentTypes.begin(); it!=_agentTypes.end(); it++)
	{
		_types.insert( make_pair( *it, AgentRecordsMap()));
	}
}

hssize_t SimulationRecord::registerAgentIds( const hid_t & stepGroup, std::vector<std::string> & indexAgents, AgentRecordsMap & agents )
{
	hid_t datasetId = H5Dopen(stepGroup, "id", H5P_DEFAULT);						
	hid_t stringType = H5Tcopy (H5T_C_S1);
	H5Tset_size(stringType, H5T_VARIABLE);
	
	// get the number of elements
	hid_t stringSpace = H5Dget_space(datasetId);
	hssize_t numElements = H5Sget_simple_extent_npoints(stringSpace);

	if(numElements==0)
	{
		H5Sclose(stringSpace);
		H5Tclose(stringType);
		H5Dclose(datasetId);
		return numElements;
	}

	char ** stringIds = (char **) malloc (numElements * sizeof (char *));	
	H5Dread(datasetId, stringType, H5S_ALL, H5S_ALL, H5P_DEFAULT, stringIds);						
				
	for(int iAgent=0; iAgent<numElements; iAgent++)
	{
		std::string agentName(stringIds[iAgent]);	
		indexAgents.push_back(agentName);
		AgentRecordsMap::iterator it = agents.find(agentName);
		// new agent
		if(it==agents.end())
		{
			agents.insert( make_pair( agentName, new AgentRecord(agentName, (_numSteps/_resolution)+1)));
			it = agents.find(agentName);
		}
		AgentRecord * agentRecord = it->second;
		// the agent exists in _loadingStep
		agentRecord->addState( _loadingStep/_resolution, "exists", true);
	}
	// clean memory
	H5Dvlen_reclaim (stringType, stringSpace, H5P_DEFAULT, stringIds);
	free (stringIds);
	H5Sclose(stringSpace);
	H5Tclose(stringType);
	H5Dclose(datasetId);
	return numElements;
}

void SimulationRecord::updateMinMaxAttributeValues( const std::string & key, int value )
{	
	ValuesMap::iterator itMin = _minAttributeValues.find(key);
	// check if it is minimum value
	if(itMin==_minAttributeValues.end())
	{
		_minAttributeValues.insert( make_pair(key, value));
	}
	else if(value<itMin->second)
	{
		itMin->second = value;
	}
	
	// check maximum value
	ValuesMap::iterator itMax = _maxAttributeValues.find(key);
	if(itMax==_maxAttributeValues.end())
	{
		_maxAttributeValues.insert( make_pair(key, value));
	}
	else if(value>itMax->second)
	{
		itMax->second = value;
	}
}

void SimulationRecord::loadAttributes( const hid_t & stepGroup, hssize_t & numElements, const std::vector<std::string> & indexAgents, AgentRecordsMap & agents )
{
	for(std::list<std::string>::iterator itA=_agentAttributes.begin(); itA!=_agentAttributes.end(); itA++)
	{
		// id already parsed
		if((*itA).compare("id")==0)
		{
			continue;
		}
		hid_t attributeDatasetId = H5Dopen(stepGroup, (*itA).c_str(), H5P_DEFAULT);
		hid_t typeAttribute = H5Dget_type(attributeDatasetId);
		H5T_class_t typeClass = H5Tget_class(typeAttribute);
		if(typeClass== H5T_INTEGER)
		{
			std::vector<int> data;
			data.resize(numElements);
			H5Dread(attributeDatasetId, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &(data.at(0)));
			for(int iAgent=0; iAgent<numElements; iAgent++)
			{
				std::string agentName = indexAgents.at(iAgent);
				AgentRecordsMap::iterator it = agents.find(agentName);
				AgentRecord * agentRecord = it->second;
				agentRecord->addState( _loadingStep/_resolution, *itA, data.at(iAgent));
				updateMinMaxAttributeValues(*itA, data.at(iAgent));
			}
		}
		else if(typeClass== H5T_STRING)
		{
			std::stringstream oss;
			oss << "SimulationRecord::loadHDF5 - loading attribute: " << *itA << " of type string, not yet implemented";
			throw Engine::Exception(oss.str());
		}
		else
		{
			std::stringstream oss;
			oss << "SimulationRecord::loadHDF5 - loading attribute: " << *itA << " of unknown type";
			throw Engine::Exception(oss.str());
		}
		H5Tclose(typeAttribute);
		H5Dclose(attributeDatasetId);
	}
}

void SimulationRecord::loadAgentsFiles( const std::string & path, int numStepsToLoad, int numTasks  )
{
	// we need to store in numsteps the working step, in order to access it from H5Giterate
	if(numStepsToLoad==0 || numTasks==0)
	{
		return;
	}
	for(int i=0; i<numTasks; i++)
	{
		std::ostringstream agentsFileName;
		agentsFileName << path << "agents-" << i<< ".abm";

		// open a file for each original computer node
		hid_t agentsFileId = H5Fopen(agentsFileName.str().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);				
		hid_t rootGroup = H5Gopen(agentsFileId, "/", H5P_DEFAULT);
		
		// get type names
		registerAgentTypes(rootGroup);
		float increase = 50.0f/(numTasks*_types.size()*numStepsToLoad);

		// for each type check the agents in each loaded time step
		// we use _agentTypes because _types contains the whole list of agent types of the entire simulation
		// _agentTypes only has the agents of this particular computer node file
		for(std::list< std::string >::iterator typeAgent=_agentTypes.begin(); typeAgent!=_agentTypes.end(); typeAgent++)
		{
			AgentTypesMap::iterator typeIt = _types.find(*typeAgent);
			for(_loadingStep=0; _loadingStep<=_numSteps; _loadingStep=_loadingStep+_resolution)
			{
				std::stringstream line;
				line << "loading agents of type: " << *typeAgent << " in task: "<< i+1 << "/" << numTasks << " - step: " << _loadingStep << "/" << _numSteps;
				_loadingState = line.str();
				if(!_gui)
				{
					std::cout << _loadingPercentageDone << "% - " << _loadingState << std::endl;
				}
				
				std::ostringstream oss;
				oss << "/" << typeIt->first << "/step" << _loadingStep;

				hid_t stepGroup = H5Gopen(agentsFileId, oss.str().c_str(), H5P_DEFAULT);
				// register the attributes
				H5Literate(stepGroup, H5_INDEX_NAME, H5_ITER_INC, 0, iterateAgentDatasets, 0);
				
				std::vector<std::string> indexAgents;
				hssize_t numElement = registerAgentIds(stepGroup, indexAgents, typeIt->second );
				if(numElement!=0)
				{
					loadAttributes(stepGroup, numElement, indexAgents, typeIt->second);
				}
				_loadingPercentageDone += increase;
				H5Gclose(stepGroup);
				SimulationRecord::_agentAttributes.clear();
			}
		}
		H5Gclose(rootGroup);
		H5Fclose(agentsFileId);
		SimulationRecord::_agentTypes.clear();
	}
}

herr_t SimulationRecord::iterateAgentDatasets( hid_t loc_id, const char * name, const H5L_info_t *linfo, void *opdata )
{
	SimulationRecord::_agentAttributes.push_back(name);	
	return 0;
}

herr_t SimulationRecord::iterateAgentTypes( hid_t loc_id, const char * name, const H5L_info_t *linfo, void *opdata )
{
	SimulationRecord::_agentTypes.push_back(name);	
	return 0;
}
	
SimulationRecord::RasterHistory & SimulationRecord::getRasterHistory( const std::string & key )
{
	RasterMap::iterator it = _resources.find(key);
	// the key does not exists
	if(it==_resources.end())
	{
		std::stringstream oss;
		oss << "SimulationRecord::getRasterHistory - searching for unknown key: " << key << " in resource field histories";
		throw Engine::Exception(oss.str());
	}
	return it->second;
}
	
const SimulationRecord::RasterHistory & SimulationRecord::getRasterHistory( const std::string & key ) const
{
	RasterMap::const_iterator it = _resources.find(key);
	// the key does not exists
	if(it==_resources.end())
	{
		std::stringstream oss;
		oss << "SimulationRecord::getRasterHistory - searching for unknown key: " << key << " in resource field histories";
		throw Engine::Exception(oss.str());
	}
	return it->second;
}

Engine::StaticRaster & SimulationRecord::getRasterTmp( const std::string & key, const int & step )
{
	StaticRasterMap::iterator itS = _staticRasters.find(key);
	// the key does not exists
	if(itS!=_staticRasters.end())
	{
		return itS->second;
	}
	RasterHistory & resourceHistory = getRasterHistory(key);
	int index = step/_resolution;
	if(index<0 || index>=resourceHistory.size())
	{
		std::stringstream oss;
		oss << "SimulationRecord::getRasterTmp - asking for key: " << key << " and step: " << step << "out of bounds, having: " << resourceHistory.size()*_resolution << " steps";
		throw Engine::Exception(oss.str());

	}
	return resourceHistory[index];
}

Engine::Raster & SimulationRecord::getDynamicRaster(  const std::string & key, const int & step )
{
	RasterHistory & resourceHistory = getRasterHistory(key);
	int index = step/_resolution;
	if(index<0 || index>=resourceHistory.size())
	{
		std::stringstream oss;
		oss << "SimulationRecord::getDynamicRaster - asking for key: " << key << " and step: " << step << "out of bounds, having: " << resourceHistory.size()*_resolution << " steps";
		throw Engine::Exception(oss.str());

	}
	return resourceHistory[index];
}

Engine::StaticRaster & SimulationRecord::getStaticRaster( const std::string & key )
{	
	StaticRasterMap::iterator it = _staticRasters.find(key);
	// the key does not exists
	if(it==_staticRasters.end())
	{
		std::stringstream oss;
		oss << "SimulationRecord::getStaticRaster - searching for unknown key: " << key << " in static rasters";
		throw Engine::Exception(oss.str());
	}
	return it->second;
}

const std::string & SimulationRecord::getName() const
{
	return _name;
}

int SimulationRecord::getNumSteps() const
{
	return _numSteps;
}

int SimulationRecord::getResolution() const
{
	return _resolution;
}

bool SimulationRecord::hasAgentType( const std::string & type ) const
{
	AgentTypesMap::const_iterator it = _types.find(type);
	if(it==_types.end())
	{
		return false;
	}
	return true;
}

SimulationRecord::AgentTypesMap::const_iterator SimulationRecord::beginTypes() const
{
	return _types.begin();
}

SimulationRecord::AgentTypesMap::const_iterator SimulationRecord::endTypes() const
{
	return _types.end();
}

SimulationRecord::AgentRecordsMap::const_iterator SimulationRecord::beginAgents( const std::string & type ) const
{
	AgentTypesMap::const_iterator it = _types.find(type);
	if(it==_types.end())
	{	
		std::stringstream oss;
		oss << "SimulationRecord::beginAgents - asking for type " << type;
		throw Engine::Exception(oss.str());
	}	
	return it->second.begin();
}

SimulationRecord::AgentRecordsMap::const_iterator SimulationRecord::endAgents( const std::string & type ) const
{
	AgentTypesMap::const_iterator it = _types.find(type);
	if(it==_types.end())
	{	
		std::stringstream oss;
		oss << "SimulationRecord::endAgents- asking for type " << type;
		throw Engine::Exception(oss.str());
	}	
	return it->second.end();
}

SimulationRecord::AgentRecordsMap::const_iterator SimulationRecord::beginAgents( AgentTypesMap::const_iterator & it ) const
{
	return it->second.begin();
}

SimulationRecord::AgentRecordsMap::const_iterator SimulationRecord::endAgents( AgentTypesMap::const_iterator & it ) const
{
	return it->second.end();
}

SimulationRecord::RasterMap::const_iterator SimulationRecord::beginRasters() const
{
	return _resources.begin();
}

SimulationRecord::RasterMap::const_iterator SimulationRecord::endRasters() const
{
	return _resources.end();
}

SimulationRecord::StaticRasterMap::const_iterator SimulationRecord::beginStaticRasters() const
{
	return _staticRasters.begin();
}

SimulationRecord::StaticRasterMap::const_iterator SimulationRecord::endStaticRasters() const
{
	return _staticRasters.end();
}
		
AgentRecord * SimulationRecord::getAgentAtPosition( int step, const Engine::Point2D<int> & position ) const	
{
	for(AgentTypesMap::const_iterator itType=_types.begin(); itType!=_types.end(); itType++)
	{
		for(AgentRecordsMap::const_iterator it=beginAgents(itType); it!=endAgents(itType); it++)
		{
			AgentRecord * agentRecord = it->second;	
			int x = agentRecord->getState(step, "x");
			int y = agentRecord->getState(step, "y");
			if(x==position._x && y==position._y)
			{
				return agentRecord;
			}
		}
	}
	return 0;
}

double SimulationRecord::getMean( const std::string & type, const std::string & state, int step )
{
	double value = 0;
	int sample = 0;
	AgentTypesMap::iterator itType = _types.find(type);
	if(itType==_types.end())
	{
		std::stringstream oss;
		oss << "SimulationRecord::getMean - asking for state: " << state << " in type " << type;
		throw Engine::Exception(oss.str());
	}
	AgentRecordsMap agents = itType->second;
	for(AgentRecordsMap::iterator it=agents.begin(); it!=agents.end(); it++)
	{
		AgentRecord * agentRecord = it->second;	
		bool exists = agentRecord->getState(step, "exists");
		if(exists)
		{
			int agentValue = agentRecord->getState(step, state);
			if(agentValue!=std::numeric_limits<int>::max())
			{
				value += agentValue;
				sample++;
			}
		}
	}
	value = value/sample;
	return value;
}

double SimulationRecord::getSum( const std::string & type, const std::string & state, int step )
{
	double value = 0;
	
	AgentTypesMap::iterator itType = _types.find(type);
	if(itType==_types.end())
	{
		std::stringstream oss;
		oss << "SimulationRecord::getSum - asking for state: " << state << " in type " << type;
		throw Engine::Exception(oss.str());
	}
	AgentRecordsMap agents = itType->second;
	for(AgentRecordsMap::iterator it=agents.begin(); it!=agents.end(); it++)
	{
		AgentRecord * agentRecord = it->second;	
		bool exists = agentRecord->getState(step, "exists");
		if(exists)
		{
			int agentValue = agentRecord->getState(step, state);	
			if(agentValue!=std::numeric_limits<int>::max())
			{
				value += agentValue;
			}
		}
	}
	return value;
}

void SimulationRecord::registerAgent( hid_t loc_id, const char * name )
{
	hid_t datasetId = H5Dopen(loc_id, name, H5P_DEFAULT);
	std::string agentName(name);

	unsigned int typePos = agentName.find_first_of("_");
	std::string type = agentName.substr(0,typePos);

	AgentTypesMap::iterator typeIt = _types.find(type);
	if(typeIt==_types.end())
	{
		_types.insert( make_pair( type, AgentRecordsMap()));
		typeIt = _types.find(type);
	}
	AgentRecordsMap & agents = typeIt->second;
	AgentRecordsMap::iterator it = agents.find(agentName);
	// new agent
	if(it==agents.end())
	{
		// +1 due to init state, new agent record with size of num steps +1
		agents.insert( make_pair( agentName, new AgentRecord(agentName, (_numSteps/_resolution)+1)));
		it = agents.find(agentName);
	}

	int numAttributes = H5Aget_num_attrs(datasetId);
	for(int i=0; i<numAttributes; i++)
	{
		char nameAttribute[256];
		hid_t attributeId= H5Aopen_idx(datasetId, i);
		H5Aget_name(attributeId, 256, nameAttribute);
		int value = 0;
		H5Aread(attributeId, H5T_NATIVE_INT, &value);

	
		AgentRecord * agentRecord = it->second;
		agentRecord->addState( _loadingStep/_resolution, std::string(nameAttribute), value );
		H5Aclose(attributeId);

		ValuesMap::iterator it = _minAttributeValues.find(nameAttribute);
		if(it==_minAttributeValues.end())
		{
			_minAttributeValues.insert( make_pair(std::string(nameAttribute), value) );
		}
		else if(value<it->second)
		{
			it->second = value;
		}

		it = _maxAttributeValues.find(nameAttribute);
		if(it==_maxAttributeValues.end())
		{
			_maxAttributeValues.insert( make_pair(std::string(nameAttribute), value) );
		}
		else if(value>it->second)
		{
			it->second = value;
		}
	}
	H5Dclose(datasetId);
}

int SimulationRecord::getMinValueForState( const std::string & state )
{
	ValuesMap::iterator it = _minAttributeValues.find(state);
	if(it==_minAttributeValues.end())
	{
		std::stringstream oss;
		oss << "SimulationRecord::getMinValueForState - asking for not registered state: " << state;
		throw Engine::Exception(oss.str());
	}
	return it->second;
}

int SimulationRecord::getMaxValueForState( const std::string & state )
{	
	ValuesMap::iterator it = _maxAttributeValues.find(state);
	if(it==_maxAttributeValues.end())
	{
		std::stringstream oss;
		oss << "SimulationRecord::getMaxValueForState - asking for not registered state: " << state;
		throw Engine::Exception(oss.str());
	}
	return it->second;

}

herr_t SimulationRecord::registerAgentStep( hid_t loc_id, const char *name, void *opdata )
{
	Engine::SimulationRecord * record = (Engine::SimulationRecord*)opdata;
	record->registerAgent(loc_id, name);
    return 0;
}

int SimulationRecord::getSize()
{
	return _size;
}

const float & SimulationRecord::getLoadingPercentageDone() const
{
	return _loadingPercentageDone;
}

const std::string & SimulationRecord::getLoadingState() const
{
	return _loadingState;
}

} // namespace Engine


