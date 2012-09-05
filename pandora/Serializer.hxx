
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

#ifndef __Serializer_hxx__
#define __Serializer_hxx__

#include <hdf5.h>
#include <string>
#include <map>
#include <vector>

namespace Engine
{

class Simulation;
class Agent;
class StaticRaster;
class Raster;
class World;

class Serializer
{
	typedef std::map< std::string, Raster> RastersMap;
	typedef std::map< std::string, StaticRaster> StaticRastersMap;

	typedef std::map< std::string, std::vector<int> * > IntMap;
	typedef std::map< std::string, std::vector<std::string> * > StringMap;
	typedef std::map< std::string, IntMap * > IntAttributesMap;
	typedef std::map< std::string, StringMap * > StringAttributesMap;

	std::string _resultsFile;

	hid_t _agentsFileId;
	hid_t _fileId;
	// this id is used to track the data set of the agent being serialized
	hid_t _currentAgentDatasetId;

	void serializeRaster( StaticRaster & raster, World & world, const std::string & datasetKey );

	// register the type of agent into the data structures _agentIndexsMap, _stringAttributes and _intAttributes and create HDF5 structures
	void registerType( Agent * agent, World & world );
	// list of agent types registered until this moment

	IntAttributesMap _intAttributes;
	StringAttributesMap _stringAttributes;

	std::map<std::string, int> _agentIndexMap;
	
	void executeAgentSerialization( const std::string & type, int step, World & world);
	void resetCurrentIndexs();

	int getDataSize( const std::string & type );

public:
	Serializer();
	virtual ~Serializer();

	void init( Simulation & simulation, StaticRastersMap & staticRasters, RastersMap & rasters, World & world );
	void finish();

	void serializeAgent( Agent * agent, const int & step, World & world, int index);
	void serializeAttribute( const std::string & name, const int & value );
	void serializeRaster( const std::string & key, Raster & raster, World & world, const int & step );
	void serializeStaticRaster( const std::string & key, StaticRaster & raster, World & world );
	void finishAgentsSerialization( int step, World & world);
	
	void addStringAttribute( const std::string & type, const std::string & key, const std::string & value );
	void addIntAttribute( const std::string & type, const std::string & key, int value );

	void setResultsFile( const std::string & resultsFile ) { _resultsFile = resultsFile; }

};

} // namespace Engine

#endif // __Serializer_hxx__

