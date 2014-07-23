
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
#include <Exception.hxx>

#include <sstream>

namespace Engine
{

AgentRecord::AgentRecord( const std::string & id, int numSteps ) : _id(id), _numSteps(numSteps)
{
}

AgentRecord::~AgentRecord()
{
}

void AgentRecord::addInt( int numStep, const std::string & key, int value )
{
	IntAttributesMap::iterator it = _intAttributes.find(key);
	// the key still doesn't exist
	if(it==_intAttributes.end())
	{
		IntValues values;
		values.resize(_numSteps);
		_intAttributes.insert( make_pair( key, values ));
		it = _intAttributes.find(key);
	}
	IntValues& vector = it->second;
	vector[numStep] = value;
}

void AgentRecord::addFloat( int numStep, const std::string & key, float value )
{
	FloatAttributesMap::iterator it = _floatAttributes.find(key);
	// the key still doesn't exist
	if(it==_floatAttributes.end())
	{
		FloatValues values;
		values.resize(_numSteps);
		_floatAttributes.insert( make_pair( key, values ));
		it = _floatAttributes.find(key);
	}
	FloatValues& vector = it->second;
	vector[numStep] = value;
}

void AgentRecord::addStr( int numStep, const std::string & key, const std::string & value )
{
	StrAttributesMap::iterator it = _strAttributes.find(key);
	// the key still doesn't exist
	if(it==_strAttributes.end())
	{
		StrValues values;
		values.resize(_numSteps);
		_strAttributes.insert( make_pair( key, values ));
		it = _strAttributes.find(key);
	}
	StrValues& vector = it->second;
	vector[numStep] = value;
}

int AgentRecord::getInt( int numStep, const std::string & key ) const
{
	IntAttributesMap::const_iterator it = _intAttributes.find(key);
	// the key already exists
	if(it==_intAttributes.end())
	{
		std::stringstream oss;
		oss << "AgentRecord::getInt - searching for unknown key: " << key << " in agent record";
		throw Engine::Exception(oss.str());
	}
	const IntValues & vector = it->second;
	return vector.at(numStep);
}

float AgentRecord::getFloat( int numStep, const std::string & key ) const
{
	FloatAttributesMap::const_iterator it = _floatAttributes.find(key);
	// the key already exists
	if(it==_floatAttributes.end())
	{
		std::stringstream oss;
		oss << "AgentRecord::getFloat - searching for unknown key: " << key << " in agent record";
		throw Engine::Exception(oss.str());
	}
	const FloatValues & vector = it->second;
	return vector.at(numStep);
}

const std::string & AgentRecord::getStr( int numStep, const std::string & key ) const
{
	StrAttributesMap::const_iterator it = _strAttributes.find(key);
	// the key already exists
	if(it==_strAttributes.end())
	{
		std::stringstream oss;
		oss << "AgentRecord::getStr - searching for unknown key: " << key << " in agent record";
		throw Engine::Exception(oss.str());
	}
	const StrValues & vector = it->second;
	return vector.at(numStep);
}

std::string AgentRecord::getCompleteState( int numStep ) const
{
	std::stringstream completeState;
	completeState << "id: " << _id << " pos: " << getInt(numStep, "x") << "/" << getInt(numStep, "y") << std::endl;
	for(IntAttributesMap::const_iterator it=_intAttributes.begin(); it!=_intAttributes.end(); it++)
	{
		const std::string & key = it->first;
		if(key.compare("exists")==0 || key.compare("x")==0 || key.compare("y")==0)
		{
			continue;
		}
		completeState << "\t" << it->first << ": " << it->second[numStep] << std::endl;
	}
    for(FloatAttributesMap::const_iterator it=_floatAttributes.begin(); it!=_floatAttributes.end(); it++)
	{
		const std::string & key = it->first;
		if(key.compare("exists")==0 || key.compare("x")==0 || key.compare("y")==0)
		{
			continue;
		}
		completeState << "\t" << it->first << ": " << it->second[numStep] << std::endl;
	}
    for(StrAttributesMap::const_iterator it=_strAttributes.begin(); it!=_strAttributes.end(); it++)
	{
		const std::string & key = it->first;
		if(key.compare("exists")==0 || key.compare("x")==0 || key.compare("y")==0)
		{
			continue;
		}
		completeState << "\t" << it->first << ": " << it->second[numStep] << std::endl;
	}
	return completeState.str();
}

bool AgentRecord::isInt( const std::string & key ) const
{
    IntAttributesMap::const_iterator it = _intAttributes.find(key);
	if(it==_intAttributes.end())
    {
        return false;
    }
    return true;
}

bool AgentRecord::isFloat( const std::string & key ) const
{ 
    FloatAttributesMap::const_iterator it = _floatAttributes.find(key);
	if(it==_floatAttributes.end())
    {
        return false;
    }
    return true;
}

bool AgentRecord::isStr( const std::string & key ) const
{   
    StrAttributesMap::const_iterator it = _strAttributes.find(key);
	if(it==_strAttributes.end())
    {
        return false;
    }
    return true;
}

} // namespace Engine

