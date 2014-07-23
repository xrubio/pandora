
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

#ifndef __AgentRecord_hxx__
#define __AgentRecord_hxx__

#include <map>
#include <vector>
#include <string>

namespace Engine
{

class AgentRecord
{
	typedef std::vector<int> IntValues;
	typedef std::vector<float> FloatValues;
	typedef std::vector<std::string> StrValues;

public:
	typedef std::map<std::string, IntValues > IntAttributesMap;
	typedef std::map<std::string, FloatValues > FloatAttributesMap;
	typedef std::map<std::string, StrValues > StrAttributesMap;

private:
	std::string _id;
	// total number of steps in simulation (not agent life)
	int _numSteps;
	IntAttributesMap _intAttributes;
	FloatAttributesMap _floatAttributes;
	StrAttributesMap _strAttributes;
public:
	AgentRecord( const std::string & id, int numSteps );
	virtual ~AgentRecord();
	
	void addInt( int numStep, const std::string & id, int value );
	void addFloat( int numStep, const std::string & id, float value );
	void addStr( int numStep, const std::string & id, const std::string & value );

	int getInt( int numStep, const std::string & key ) const;
	float getFloat( int numStep, const std::string & key ) const;
	const std::string & getStr( int numStep, const std::string & key ) const;
	std::string getCompleteState( int numStep ) const;

	IntAttributesMap::const_iterator beginInt() const { return _intAttributes.begin(); }
	IntAttributesMap::const_iterator endInt() const { return _intAttributes.end(); }

	FloatAttributesMap::const_iterator beginFloat() const { return _floatAttributes.begin(); }
	FloatAttributesMap::const_iterator endFloat() const { return _floatAttributes.end(); }

	StrAttributesMap::const_iterator beginStr() const { return _strAttributes.begin(); }
	StrAttributesMap::const_iterator endStr() const { return _strAttributes.end(); }

	const std::string & getId() const { return _id; }
	int getNumSteps() const { return _numSteps; }

    bool isInt( const std::string & key ) const;
    bool isFloat( const std::string & key ) const;
    bool isStr( const std::string & key ) const;
};

} // namespace Engine

#endif // __AgentRecord_hxx__

