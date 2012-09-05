
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
#include <Exceptions.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <sstream>
#include <cstring>

namespace Engine
{

Config::Config() : _resultsFile(""), _numSteps(0)
{
}

Config::~Config()
{
	if(_doc)
	{	
		delete _doc;
	}
}

void Config::serialize(const std::string & filename)
{
}

TiXmlElement * Config::openTiXml(const std::string & filename)
{
	_doc = new TiXmlDocument(filename.c_str());
	if (!_doc->LoadFile())
	{
		std::stringstream oss;
		oss << "Exception, error while opening config file " << filename << std::endl;
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
}
  
void Config::extractAttribs(TiXmlElement *pRoot)
{
	TiXmlElement *pParm = pRoot->FirstChildElement("output");
	retrieveAttributeMandatory( pParm, "resultsFile", _resultsFile);

	std::string logsDir;
	retrieveAttributeMandatory( pParm, "logsDir", logsDir);
	GeneralState::logger().setLogsDir(logsDir);

	pParm = pRoot->FirstChildElement("numSteps");
	_numSteps = atoi(pParm->Attribute("value"));
    
    extractParticularAttribs(pRoot);
}

void Config::deserialize(const std::string & filename)
{
	TiXmlElement *pRoot = openTiXml(filename);
	if( pRoot )
	{
		extractAttribs(pRoot);    
		// destroy pRoot and pParm
		closeTiXml();
	}
}

const int & Config::getNumSteps() const
{
	return _numSteps;
}

void Config::retrieveAttributeMandatory( TiXmlElement* elem, const std::string & attrName, std::string& value )
{
	const std::string * retrievedStr = elem->Attribute( attrName );
	if (!retrievedStr)
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: Attribute " << elem->ValueStr() << "." << attrName << " not found!" << std::endl;
		throw Engine::Exception(sstr.str());
		return;
	}
	value = *retrievedStr;
}

void Config::retrieveAttributeOptional( TiXmlElement* elem, const std::string & attrName, std::string& value )
{
	const std::string * retrievedStr = elem->Attribute( attrName );
	if(!retrievedStr)
	{
		std::stringstream sstr;
		std::cerr << "[CONFIG]: WARNING: Attribute " << elem->ValueStr() << "." << attrName << " not found!" << std::endl;
		value = "";
		return;	
	}
	value = *retrievedStr;
}

void Config::retrieveAttributeMandatory( TiXmlElement* elem, const std::string & attrName, int& value )
{
	const std::string * retrievedStr = elem->Attribute( attrName );
	if(!retrievedStr)
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: Attribute " << elem->ValueStr() << "." << attrName << " not found!" << std::endl;
		throw Engine::Exception(sstr.str());
	}
	value = atoi(retrievedStr->c_str());
}

void Config::retrieveAttributeOptional( TiXmlElement* elem, const std::string & attrName, int& value )
{	
	const std::string * retrievedStr = elem->Attribute( attrName );
	if(!retrievedStr)
	{
		std::stringstream sstr;
		std::cerr << "[CONFIG]: WARNING: Attribute " << elem->ValueStr() << "." << attrName << " not found!" << std::endl;
		value = 0;
		return;	
	}
	value = atoi(retrievedStr->c_str());

}

void Config::retrieveAttributeMandatory( TiXmlElement* elem, const std::string & attrName, float& value )
{
	const std::string * retrievedStr = elem->Attribute( attrName );
	if(!retrievedStr)
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: Attribute " << elem->ValueStr() << "." << attrName << " not found!" << std::endl;
		throw Engine::Exception(sstr.str());
	}
	value = atof(retrievedStr->c_str());
}

void Config::retrieveAttributeOptional( TiXmlElement* elem, const std::string & attrName, float& value )
{
	const std::string * retrievedStr = elem->Attribute( attrName );
	if(!retrievedStr)
	{
		std::stringstream sstr;
		std::cerr << "[CONFIG]: WARNING: Attribute " << elem->ValueStr() << "." << attrName << " not found!" << std::endl;
		value = 0.0f;
		return;	
	}
	value = atof(retrievedStr->c_str());
}


} // namespace Engine

