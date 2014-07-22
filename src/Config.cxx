
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

std::string Config::getParamStr( const std::string & elementPath, const std::string & attrName)
{
    TiXmlElement * elem = findElement(elementPath);
	const std::string * retrievedStr = elem->Attribute( attrName );
	if (!retrievedStr)
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: Param " << elem->ValueStr() << "." << attrName << " not found!" << std::endl;
		throw Engine::Exception(sstr.str());
		return "error";
	}
	return *retrievedStr;
}

bool Config::getParamBool( const std::string & elementPath, const std::string & attrName)
{
    TiXmlElement * elem = findElement(elementPath);
	const std::string * retrievedStr = elem->Attribute( attrName );
	if (!retrievedStr)
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: Param " << elem->ValueStr() << "." << attrName << " not found!" << std::endl;
		throw Engine::Exception(sstr.str());
		return false;
	}
	if(!retrievedStr->compare("yes") || !retrievedStr->compare("true") || !retrievedStr->compare("1"))
	{
        return true;
	}
    return false;
}

int Config::getParamInt( const std::string & elementPath, const std::string & attrName)
{
    TiXmlElement * elem = findElement(elementPath);
	const std::string * retrievedStr = elem->Attribute( attrName );
	if(!retrievedStr)
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: Param " << elem->ValueStr() << "." << attrName << " not found!" << std::endl;
		throw Engine::Exception(sstr.str());
	}
	return atoi(retrievedStr->c_str());
}

long int Config::getParamLongInt( const std::string & elementPath, const std::string & attrName )
{
    TiXmlElement * elem = findElement(elementPath);
	const std::string * retrievedStr = elem->Attribute( attrName );
	if(!retrievedStr)
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: Param " << elem->ValueStr() << "." << attrName << " not found!" << std::endl;
		throw Engine::Exception(sstr.str());
	}
	return atol(retrievedStr->c_str());
}

float Config::getParamFloat( const std::string & elementPath, const std::string & attrName)
{
    TiXmlElement * elem = findElement(elementPath);
	const std::string * retrievedStr = elem->Attribute( attrName );
	if(!retrievedStr)
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: Param " << elem->ValueStr() << "." << attrName << " not found!" << std::endl;
		throw Engine::Exception(sstr.str());
	}
	return atof(retrievedStr->c_str());
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
    }
    return element;
}

const Size<int> & Config::getSize() const
{
    return _size;
}

} // namespace Engine

