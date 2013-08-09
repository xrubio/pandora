/*
 * Copyright (c) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es
 *
 * This file is part of Cassandra.
 * Cassandra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Cassandra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef __ProjectConfiguration_hxx__
#define __ProjectConfiguration_hxx__

#include <string>
#include <map>

class TiXmlElement;

namespace Engine
{
	class SimulationRecord;
}

namespace GUI
{

class AgentConfiguration;
class RasterConfiguration;

class ProjectConfiguration
{
public:	
	typedef std::map<std::string, AgentConfiguration * > AgentsConfigurationMap;
	typedef std::map<std::string, RasterConfiguration * > RastersConfigurationMap;

private:
	static ProjectConfiguration * _instance;
	
	std::string _fileName;

	std::string _simulationFileName;
	int _resolution;

	Engine::SimulationRecord * _simulationRecord;
	
	ProjectConfiguration();

	AgentsConfigurationMap _agentsConfig;
	RastersConfigurationMap _rastersConfig;
	
	void cleanConfigs();

	TiXmlElement * storeAgentsConfigs();
	TiXmlElement * storeRastersConfigs();

	void loadAgentsConfigs( TiXmlElement * agents );
	void loadRastersConfigs( TiXmlElement * rasters );

	// this method created agent and raster configurations based on currently loaded SimulationRecord
	void loadConfigs();

	// this flag is used to keep track whether the user created a project or is just loading a simulation
	bool _projectCreated;
	
	// cleans SimulationRecord and related configurations from existing data
	void cleanSimulationRecord();

public:
	static ProjectConfiguration * instance();
	virtual ~ProjectConfiguration();

	// removes all existing data
	void reset();


	void loadProject( const std::string & fileName );
	// this function is used to store the project to a fileName different from _fileName
	void storeProject( const std::string & fileName );
	// this function is used to store the project to _fileName
	void storeProject();

	void setResolution( const int & resolution );
	int getResolution();

	const std::string & getSimulationFileName();
	void setSimulationFileName( const std::string & simulationFileName );	

	bool alreadySaved();

	Engine::SimulationRecord * getSimulationRecord();
	// returns true if simulation was correctly loaded
	bool loadSimulation();

	void updateAgentConfig( const std::string & type, const AgentConfiguration & config );
	void updateRasterConfig( const std::string & type, const RasterConfiguration & config );
	AgentConfiguration * getAgentConfig( const std::string & type );
	RasterConfiguration * getRasterConfig( const std::string & type );

};

} // namespace GUI

#endif // __ProjectConfiguration_hxx__

