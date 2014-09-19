
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

#include <cstdlib>
#include <string>
#include <Size.hxx>

class TiXmlDocument;
class TiXmlElement;

namespace Engine
{

class Config
{ 

protected:
	TiXmlDocument * _doc;
    TiXmlElement * _root;
	// general attributes
	// file where simulation results will be stored
	std::string _resultsFile;

    // Space of simulation
    Size<int> _size;
	// number of time steps of the simulation
	int _numSteps;
	// number of steps to execute before serializing the state of the simulation
	int _serializeResolution;
	// xml config file (if it exists)
	std::string _configFile;


    TiXmlElement * findElement( const std::string & elementPath );
	
	TiXmlElement * openTiXml(const std::string & filename);  
	void closeTiXml();
	void loadBaseParams();
public:
    // minimalist constructor (xml file not necessary)
	Config( const Size<int> & size = Size<int>(0,0), const int & numSteps = 1, const std::string & resultsFile = "data/results.h5", const int & serializerResolution = 1 );
    // xml-based constructor
	Config( const std::string & configFile ); 
	virtual ~Config();
	void loadFile();

	friend std::ostream & operator<<( std::ostream & stream, const Config & c)
	{
		return stream << "Config(" << c._resultsFile << "," << c._numSteps << ")"; 
	}

	const Size<int> & getSize() const;
	const int & getNumSteps() const;
	const int & getSerializeResolution() const;
	const std::string & getResultsFile() const{return _resultsFile; }
	virtual void loadParams(){};    
  
	std::string getParamStrFromElem(TiXmlElement* elem, const std::string & attrName);
    std::string getParamStr( const std::string & elementPath, const std::string & attrName);
	
	int getParamIntFromElem(TiXmlElement* elem, const std::string & attrName);
	int getParamInt( const std::string & elementPath, const std::string & attrName);
	
	unsigned getParamUnsignedFromElem(TiXmlElement* elem, const std::string & attrName);
	unsigned getParamUnsigned(const std::string & elementPath, const std::string & attrName);
	
	long int getParamLongFromElem(TiXmlElement* elem, const std::string & attrName);
	long int getParamLongInt( const std::string & elementPath, const std::string & attrName);
	
	float getParamFloatFromElem(TiXmlElement* elem, const std::string & attrName);
	float getParamFloat( const std::string & elementPath, const std::string & attrName);
	
	bool getParamBoolFromElem(TiXmlElement* elem, const std::string & attrName);
	bool getParamBool( const std::string & elementPath, const std::string & attrName);
};

} // namespace Engine

#endif // __Config__hxx__

