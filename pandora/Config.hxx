
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

#ifndef __Config__hxx__ 
#define __Config__hxx__

#include <iostream>
#include <cstdlib>
#include <tinyxml.h>
#include <string>

namespace Engine
{

class Config
{ 

protected:
	TiXmlDocument * _doc;
	// general attributes
	// file where simulation results will be stored
	std::string _resultsFile;
	// number of time steps of the simulation
	int _numSteps;

	void retrieveAttributeMandatory( TiXmlElement* elem, const std::string & attrName, std::string& value );
	void retrieveAttributeOptional( TiXmlElement* elem, const std::string & attrName, std::string& value );
	void retrieveAttributeMandatory( TiXmlElement* elem, const std::string & attrName, int& value );
	void retrieveAttributeOptional( TiXmlElement* elem, const std::string & attrName, int& value );
	void retrieveAttributeMandatory( TiXmlElement* elem, const std::string & attrName, float& value );
	void retrieveAttributeOptional( TiXmlElement* elem, const std::string & attrName, float& value );

public:
	Config();
	virtual ~Config();
	
	TiXmlElement * openTiXml(const std::string & filename);  
	void closeTiXml();
	virtual void extractAttribs(TiXmlElement *pRoot);
	virtual void extractParticularAttribs(TiXmlElement *pRoot) = 0;    
	virtual void serialize(const std::string & filename);
	virtual void deserialize(const std::string & filename);

	friend std::ostream & operator<<( std::ostream & stream, const Config & c)
	{
		return stream << "Config(" << c._resultsFile << "," << c._numSteps << ")"; 
	}

	const int & getNumSteps() const;
  
};

} // namespace Engine

#endif // __Config__hxx__

