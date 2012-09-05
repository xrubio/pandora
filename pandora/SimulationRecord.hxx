
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

#include <Raster.hxx>
#include <StaticRaster.hxx>
#include <AgentRecord.hxx>
#include <Point2D.hxx>

namespace Engine 
{

class SimulationRecord
{
public:
	// TODO programar funció d'equivalencia a agent record i convertir en llista
	typedef std::map<std::string, AgentRecord * > AgentRecordsMap;
	typedef std::map<std::string, int > ValuesMap;
	typedef std::vector<Engine::Raster> RasterHistory;
	typedef std::map<std::string, RasterHistory> RasterMap;
	typedef std::map<std::string, Engine::StaticRaster> StaticRasterMap;
	typedef std::map<std::string, AgentRecordsMap > AgentTypesMap;
private:

	std::string _name;
	RasterMap _resources;
	StaticRasterMap _staticRasters;
	AgentTypesMap _types;
	int _numSteps;
	// the step currently being loaded
	int _loadingStep;
	int _resolution;

	// we need to know min and max values for each state in order to paint agents 
	ValuesMap _minAttributeValues;
	ValuesMap _maxAttributeValues;

	int _size;

	// we need this function in order to be called by H5Giterate. It must be static to match the C call signature
	static herr_t registerAgentStep( hid_t loc_id, const char *name, void *opdata );

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
public:
	SimulationRecord( int resolution = 1, bool gui = true );
	virtual ~SimulationRecord();

	// the real method, called from registerAgentStep
	void registerAgent( hid_t loc_id, const char * name );
	bool loadHDF5( const std::string & fileName, const bool & loadRasters=true, const bool & loadAgents=true);
	
	RasterHistory & getRasterHistory( const std::string & key );
	const RasterHistory & getRasterHistory( const std::string & key ) const;
	Engine::Raster & getDynamicRaster( const std::string & key, const int & step );
	Engine::StaticRaster & getStaticRaster( const std::string & key );
	Engine::StaticRaster & getRasterTmp( const std::string & key, const int & step );

	const std::string & getName() const;
	int getNumSteps() const;
	int getResolution() const;	
	
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

	AgentRecord * getAgentAtPosition( int step, const Engine::Point2D<int> & position ) const;
	
	// TODO make a different class
	double getMean( const std::string & type, const std::string & state, int step );
	double getSum( const std::string & type, const std::string & state, int step );
	
	int getMinValueForState( const std::string & state );
	int getMaxValueForState( const std::string & state );
	int getSize();
	const float & getLoadingPercentageDone() const;
	const std::string & getLoadingState() const;
};

} // namespace Engine 

#endif // __SimulationRecord_hxx__

