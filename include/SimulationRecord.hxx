
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

#ifndef __SimulationRecord_hxx__
#define __SimulationRecord_hxx__

#include <vector>
#include <string>
#include <list>
#include <map>
#include <hdf5.h>

#include <DynamicRaster.hxx>
#include <StaticRaster.hxx>
#include <AgentRecord.hxx>
#include <Point2D.hxx>
#include <Size.hxx>

namespace Engine 
{

class SimulationRecord
{
public:
	typedef std::map<std::string, AgentRecord * > AgentRecordsMap;
	
    typedef std::map<std::string, int > IntAttributesMap;	
	typedef std::map<std::string, float > FloatAttributesMap;	
	typedef std::map<std::string, std::string > StrAttributesMap;	

	typedef std::vector<DynamicRaster> RasterHistory;
	typedef std::map<std::string, RasterHistory> RasterMap;
	typedef std::map<std::string, StaticRaster> StaticRasterMap;
	typedef std::map<std::string, AgentRecordsMap > AgentTypesMap;
	typedef std::vector<AgentRecord *> AgentRecordsVector;
private:

	std::string _name;
	RasterMap _resources;
	StaticRasterMap _staticRasters;
	AgentTypesMap _types;
	int _numSteps;
	// the step currently being loaded
	int _loadingStep;

	// resolution of loaded data
	int _loadedResolution;
	// resolution of serializedData
	int _serializedResolution;

	// we need to know min and max values for each numerical attribute in order to paint agents 
	IntAttributesMap _minIntValues;
	IntAttributesMap _maxIntValues;
    FloatAttributesMap _minFloatValues;
    FloatAttributesMap _maxFloatValues;

	Size<int> _size;

	// we need this function in order to be called by H5Giterate. It must be static to match the C call signature
	//static herr_t registerAgentStep( hid_t loc_id, const char *name, void *opdata );

	static herr_t iterateAgentTypes( hid_t loc_id, const char * name, const H5L_info_t *linfo, void *opdata );
	static herr_t iterateAgentDatasets( hid_t loc_id, const char * name, const H5L_info_t *linfo, void *opdata );

	// false if loading without gui
	bool _gui;
	float _loadingPercentageDone;
	std::string _loadingState;
	static std::list< std::string > _agentTypes;
	static std::list< std::string > _agentAttributes;

	// loads the agents of simulation 
	void loadAgentsFiles( const std::string & path, int numStepsToLoad, int numTasks  );
	// registers the complete list of agent types into SimulationRecord
	void registerAgentTypes( const hid_t & rootGroup );
	// looks for the list of agents present at a given time step (defined by stepGroyp) and type (defined in agents)
	hssize_t registerAgentIds( const hid_t & stepGroup, std::vector<std::string> & indexAgents, AgentRecordsMap & agents );
	// loads the attributes of the agents present at a given time step (defined by stepGroyp) and type (defined in agents)
	void loadAttributes( const hid_t & stepGroup, hssize_t & numElements, const std::vector<std::string> & indexAgents, AgentRecordsMap & agents );
	// updates min/max values checking value for the attribute key
	void updateMinMaxAttributeValues( const std::string & key, int value );
	void updateMinMaxAttributeValues( const std::string & key, float value );
public:
	SimulationRecord( int loadedResolution = 1, bool gui = true );
	virtual ~SimulationRecord();

	// the real method, called from registerAgentStep
	//void registerAgent( hid_t loc_id, const char * name );
	bool loadHDF5( const std::string & fileName, const bool & loadRasters=true, const bool & loadAgents=true);
	
	RasterHistory & getRasterHistory( const std::string & key );
	const RasterHistory & getRasterHistory( const std::string & key ) const;
	DynamicRaster & getDynamicRaster( const std::string & key, const int & step );
	StaticRaster & getStaticRaster( const std::string & key );
	StaticRaster & getRasterTmp( const std::string & key, const int & step );

	const std::string & getName() const;
	int getNumSteps() const;
	int getSerializedResolution() const;	
	int getLoadedResolution() const;	
	// the final resolution is the result of multiplying serialized x loaded resolution
	int getFinalResolution() const;
	
	AgentRecordsMap::const_iterator beginAgents( const std::string & type ) const;
	AgentRecordsMap::const_iterator endAgents( const std::string & type) const;

	AgentRecordsMap::const_iterator beginAgents( AgentTypesMap::const_iterator & it ) const;
	AgentRecordsMap::const_iterator endAgents( AgentTypesMap::const_iterator & it ) const;

	bool hasAgentType( const std::string & type ) const;
	AgentTypesMap::const_iterator beginTypes() const;
	AgentTypesMap::const_iterator endTypes() const;

	RasterMap::const_iterator beginRasters() const;
	RasterMap::const_iterator endRasters() const;

	StaticRasterMap::const_iterator beginStaticRasters() const;
	StaticRasterMap::const_iterator endStaticRasters() const;

	AgentRecordsVector getAgentsAtPosition( int step, const Point2D<int> & position ) const;
	
	double getMean( const std::string & type, const std::string & attribute, int step );
	double getSum( const std::string & type, const std::string & attribute, int step );
	
	int getMinInt( const std::string & attribute);
	int getMaxInt( const std::string & attribute);
    float getMinFloat( const std::string & attribute);
	float getMaxFloat( const std::string & attribute);

	const Size<int> & getSize() const;
	const float & getLoadingPercentageDone() const;
	const std::string & getLoadingState() const;
};

} // namespace Engine 

#endif // __SimulationRecord_hxx__

