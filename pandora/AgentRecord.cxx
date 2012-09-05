
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

#include <AgentRecord.hxx>
#include <Exceptions.hxx>

#include <iostream>
#include <sstream>

namespace Engine
{

AgentRecord::AgentRecord( const std::string & id, int numSteps ) : _id(id), _numSteps(numSteps)
{
}

AgentRecord::~AgentRecord()
{
}

void AgentRecord::addState( int numStep, const std::string & key, int value )
{
	StatesMap::iterator it = _states.find(key);
	// the key already exists
	if(it==_states.end())
	{
		//_states.insert( std::pair< std::string, ValuesVector >(key, std::vector<int> ));		
		std::vector<int> values;
		values.resize(_numSteps);
		_states.insert( make_pair( key, values ));
		it = _states.find(key);
		/*
		ValuesVector & vector = it->second;
		vector.resize(_numSteps);
		*/
	}
	ValuesVector & vector = it->second;
	vector[numStep] = value;
}

int AgentRecord::getState( int numStep, const std::string & key ) const
{
	StatesMap::const_iterator it = _states.find(key);
	// the key already exists
	if(it==_states.end())
	{
		std::stringstream oss;
		oss << "AgentRecord::getState - searching for unknown key: " << key << " in agent record";
		throw Engine::Exception(oss.str());
	}
	const ValuesVector & vector = it->second;
	return vector.at(numStep);
}

std::string AgentRecord::getCompleteState( int numStep ) const
{
	std::stringstream completeState;
	completeState << "id: " << _id << std::endl;
	completeState << "Agent values:";
	for(StatesMap::const_iterator it=_states.begin(); it!=_states.end(); it++)
	{
		completeState << std::endl << it->first << ": " << it->second[numStep];
	}
	return completeState.str();
}

AgentRecord::StatesMap::const_iterator AgentRecord::beginStates() const
{
	return _states.begin();
}

AgentRecord::StatesMap::const_iterator AgentRecord::endStates() const
{
	return _states.end();
}

} // namespace Engine

