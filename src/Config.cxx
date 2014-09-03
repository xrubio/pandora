
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


#include <Config.hxx>
#include <Exception.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <sstream>
#include <cstring>
#include <tinyxml.h>
#include <boost/lexical_cast.hpp>

namespace Engine
{

Config::Config( const std::string & configFile ) : _doc(0), _root(0), _configFile(configFile)
{
}

Config::Config( const Size<int> & size, const int & numSteps, const std::string & resultsFile, const int & serializeResolution ) : _doc(0), _root(0), _resultsFile(resultsFile), _size(size), _numSteps(numSteps), _serializeResolution(serializeResolution), _configFile("")
{
}

Config::~Config()
{
	if(_doc)
	{	
		delete _doc;
	}
}

TiXmlElement * Config::openTiXml(const std::string & filename)
{
	_doc = new TiXmlDocument(filename.c_str());
	if (!_doc->LoadFile())
	{
		std::stringstream oss;
		oss << "Exception, error while opening config file " << filename << ": " <<
		_doc->ErrorDesc() << " (line " << _doc->ErrorRow() << ")" << std::endl;
		throw Engine::Exception(oss.str());
		return 0;
	}

	TiXmlHandle hDoc(_doc);
	TiXmlHandle hRoot(0);
    
	TiXmlElement *pRoot;
	pRoot = _doc->FirstChildElement( "config" );
	return pRoot;    
}
  
void Config::closeTiXml()
{
	if(!_doc)
	{
		return;
	}
	delete _doc;
	_doc = 0;
    _root = 0;
}
  
void Config::loadBaseParams()
{
    _resultsFile = getParamStr("output", "resultsFile");
    std::string logsDir = getParamStr("output", "logsDir");
	GeneralState::logger().setLogsDir(logsDir);

    _numSteps = getParamInt("numSteps", "value");
    _serializeResolution = getParamInt("numSteps", "serializeResolution");

    _size._width = getParamInt("size", "width");
    _size._height = getParamInt("size", "height");
}

void Config::loadFile()
{
    if(_configFile.empty())
    {
        return;
    }

	_root = openTiXml(_configFile);
    if(!_root)
    {
        return;
    }

    loadBaseParams();
    loadParams();
    closeTiXml();
}

const int & Config::getNumSteps() const
{
	return _numSteps;
}

const int & Config::getSerializeResolution() const
{
	return _serializeResolution;
}


std::string Config::getParamStrFromElem(TiXmlElement* elem, const std::string & attrName)
{
	const std::string * retrievedStr = elem->Attribute( attrName );
	if (!retrievedStr) 
	{
		throw Engine::Exception("[CONFIG]: ERROR - Parameter " + elem->ValueStr() + "." + attrName + " not found!");
	}
	return *retrievedStr;
}
std::string Config::getParamStr( const std::string & elementPath, const std::string & attrName) {
	return getParamStrFromElem(findElement(elementPath), attrName);
}

bool Config::getParamBoolFromElem(TiXmlElement* elem, const std::string & attrName) {
	const std::string value = getParamStrFromElem(elem, attrName);
	return value.compare("yes") == 0 || value.compare("true") == 0 || value.compare("1") == 0;	
}
bool Config::getParamBool( const std::string & elementPath, const std::string & attrName) {
	return getParamBoolFromElem(findElement(elementPath), attrName);
}

int Config::getParamIntFromElem(TiXmlElement* elem, const std::string & attrName) {
	return atoi(getParamStrFromElem(elem, attrName).c_str());
}
int Config::getParamInt( const std::string & elementPath, const std::string & attrName) {
	return getParamIntFromElem(findElement(elementPath), attrName);
}

unsigned Config::getParamUnsignedFromElem(TiXmlElement* elem, const std::string & attrName) {
	return boost::lexical_cast<unsigned>(getParamStrFromElem(elem, attrName).c_str());
}
unsigned Config::getParamUnsigned(const std::string & elementPath, const std::string & attrName) {
	return getParamUnsignedFromElem(findElement(elementPath), attrName);
}

long int Config::getParamLongFromElem(TiXmlElement* elem, const std::string & attrName) {
	return atol(getParamStrFromElem(elem, attrName).c_str());
}
long int Config::getParamLongInt( const std::string & elementPath, const std::string & attrName ) {
	return  getParamLongFromElem(findElement(elementPath), attrName);
}

float Config::getParamFloatFromElem(TiXmlElement* elem, const std::string & attrName) {
	return atof(getParamStrFromElem(elem, attrName).c_str());
}
float Config::getParamFloat( const std::string & elementPath, const std::string & attrName) {
	return  getParamFloatFromElem(findElement(elementPath), attrName);
}

    
TiXmlElement * Config::findElement( const std::string & elementPath )
{
    // split elementPath based on "/" delimiters 
    std::istringstream iss(elementPath);
    std::vector<std::string> tokens; 
    std::string token;
    while(std::getline(iss, token, '/'))
    {
        tokens.push_back(token);
    }

    TiXmlElement * element = _root;
    for(std::vector<std::string>::const_iterator it=tokens.begin(); it!=tokens.end(); it++)
    {
        element = element->FirstChildElement(*it); 
        if(!element)
        {	
            std::stringstream sstr;
		    sstr << "Config::findElement " << elementPath << " is not inside config file";
    		throw Engine::Exception(sstr.str());
        }
    }
    return element;
}

const Size<int> & Config::getSize() const
{
    return _size;
}

} // namespace Engine

