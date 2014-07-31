/*
 * Copyright (c) 2014
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

#ifndef __SequentialSerializer_hxx__
#define __SequentialSerializer_hxx__

#include <hdf5.h>
#include <string>
#include <map>
#include <typedefs.hxx>

namespace Engine
{

class Scheduler;
class World;
class Config;
class StaticRaster;

/** Serializer class that stores simulation data with standard HDF5 interface 
  * It can be used for any Scheduler that does not use non-shared memory distribution
  */
class SequentialSerializer
{
	typedef std::map< std::string, StaticRaster *> StaticRastersRefMap;
	typedef std::map< std::string, std::vector<int> * > IntMap;
	typedef std::map< std::string, std::vector<float> * > FloatMap;
	typedef std::map< std::string, std::vector<std::string> * > StringMap;
	typedef std::map< std::string, IntMap * > IntAttributesMap;
	typedef std::map< std::string, FloatMap * > FloatAttributesMap;
	typedef std::map< std::string, StringMap * > StringAttributesMap;

	const Scheduler & _scheduler;
    const Config * _config;

	hid_t _fileId;
	hid_t _agentsFileId;
	StaticRastersRefMap _dynamicRasters;

	StringAttributesMap _stringAttributes;
	IntAttributesMap _intAttributes;
	FloatAttributesMap _floatAttributes;
	std::map<std::string, int> _agentIndexMap;

	void executeAgentSerialization( const std::string & type, int step);
	void serializeAgent( Agent * agent, const int & step, int index);
	void finishAgentsSerialization( int step);
	// register the type of agent into the data structures _agentIndexsMap, _stringAttributes and _intAttributes and create HDF5 structures
	void registerType( Agent * agent);
	int getDataSize( const std::string & type );
	void resetCurrentIndexs();
	void serializeRaster( const StaticRaster & raster, const std::string & datasetKey );

public:
	SequentialSerializer( const Scheduler & scheduler );
	virtual ~SequentialSerializer();
	void init( World & world );
	void finish();
	
	void addStringAttribute( const std::string & type, const std::string & key, const std::string & value );
	void addIntAttribute( const std::string & type, const std::string & key, int value );
	void addFloatAttribute( const std::string & type, const std::string & key, float value );

	void serializeAgents( const int & step, const AgentsList::const_iterator & beginAgents, const AgentsList::const_iterator & endAgents);
	void serializeStaticRasters( const StaticRastersRefMap & staticRasters);
	void serializeRasters(int step);

};

} // namespace Engine

#endif // __SequentialSerializer_hxx__

