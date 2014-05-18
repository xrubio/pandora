/*
 * Copyright (c) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es
 *
 * This file is part of Cassandra.
 * Cassandra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Cassandra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <Loader3DS.hxx>
#include <Exception.hxx>

#include <Model3D.hxx>
#include <Object3D.hxx>

#include <Point2D.hxx>
#include <Point3D.hxx>
#include <Material.hxx>

#include <iostream>

namespace GUI
{

Loader3DS * Loader3DS::_instance = 0;

Loader3DS * Loader3DS::instance()
{
	if(!_instance)
	{
		_instance = new Loader3DS;
	}
	return _instance;
}

Loader3DS::Loader3DS() : _model(0), _object(0)
{
	//std::cout << "loader constructor" << std::endl;
}

Loader3DS::~Loader3DS()
{
	// TODO is it even called??
	//std::cout << "loader destructor" << std::endl;
}

Model3D * Loader3DS::loadModel( const std::string & path )
{
	_model = new Model3D();
	std::cout << "loading 3d model: " << path << std::endl;
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if(file.fail())
	{
		std::stringstream oss;
		oss << "Loader3DS::loadModel - error loading model in path: " << path;
		throw Engine::Exception(oss.str());
		return 0;
	}
	unsigned short id;
	int length;
	bool newChunk = true;
	std::string token("");

	while(file.good())
	{
		if(newChunk)
		{
			file.read((char*)&id, sizeof(id));
			file.read((char*)&length, sizeof(length));
		}
		//std::cout << "id chunk: " << id << " with length: " << length << " and tellg: " << file.tellg() << std::endl;
		newChunk = true;
		switch(id)
		{
			// main chunk
			case 19789:
				//std::cout << "main chunk" << std::endl;
			break;
			
			// 3d editor chunk
			case 15677:
				//std::cout << "3d editor" << std::endl;
			break;

			case 16384:
				token = loadName(file);
				//std::cout << "new object with name: " << token << std::endl;
				_object = _model->createNewObject(token);
			break;

			// triangular mesh			
			case 16640:
				//std::cout << "\ttriangular mesh" << std::endl;				
			break;

			case 16656:
				//std::cout << "\tload vertexs" << std::endl;
				loadVertexs(file);
			break;

			case 16704:
				//std::cout << "\tload textures" << std::endl;
				loadTextureMapCoordinates(file);
			break;
			
			case 16672:
				//std::cout << "\tload polygons" << std::endl;
				loadPolygons(file);
			break;

			case 16688:
				//std::cout << "\tface materials: " << std::endl;
				// TODO more than one material used in a given object (list of polygons -> materials)
				loadFaceMaterials(file);
			break;

			case 45055:
				//std::cout << "mat entry" << std::endl;
				loadMaterial(file, id, length);
				newChunk = false;
			break;
		
			default:
				//std::cout << "unknown chunk: " << id << " with length: " << length << std::endl;
				skipChunk(length, file);
			break;
		}
	}
	file.close();
	//std::cout << "file: " << path << " successfully loaded" << std::endl;
	_model->init();
	return _model;
}

std::string Loader3DS::loadName( std::ifstream & file )
{
	std::string name("");
	for(int i=0; i<20; i++)
	{
		char token = file.get();
		if(token=='\0')
		{
			return name;
		}
		name.push_back(token);
	}
	return name;
}

void Loader3DS::loadVertexs( std::ifstream & file )
{		
	//std::cout << "vertex list" << std::endl;
	short size = 0;
	file.read((char*)&size, sizeof(size));
	_object->_vertexs.resize(size);
	//std::cout << "loading: " << object->_vertexs.size() << " vertexs" << std::endl;

	file.read((char*)&_object->_vertexs[0], _object->_vertexs.size()*sizeof(_object->_vertexs[0]));
	/*
	for(int i=0; i<_object->_vertexs.size(); i++)
	{
		std::cout << "vertex: " << i << " - " << _object->_vertexs[i] << " of: " << _object->_vertexs.size() << std::endl;
	}
	*/
	//std::cout << "\tloaded: " << _object->_vertexs.size() << " vertexs" << std::endl;
}



void Loader3DS::loadTextureMapCoordinates( std::ifstream & file )
{	
	//std::cout << "texture map coordinates" << std::endl;
	short size = 0;
	file.read((char*)&size, sizeof(size));
	_object->_textureMapCoordinates.resize(size);
	//std::cout << "loading: " << _object->_textureMapCoordinates.size() << " texture map coordinates" << std::endl;
	file.read((char*)&_object->_textureMapCoordinates[0], _object->_textureMapCoordinates.size()*sizeof(_object->_textureMapCoordinates[0]));
	/*
	for(int i=0; i<_object->_textureMapCoordinates.size(); i++)
	{
		std::cout << "texture map coordinates: " << i << " of: " << _object->_textureMapCoordinates.size() << " - " << _object->_textureMapCoordinates[i] << std::endl;		
	}
	*/
}

void Loader3DS::loadPolygons( std::ifstream & file )
{
	short size = 0;
	//std::cout << "polygons" << std::endl;
	file.read((char*)&size, sizeof(size));
	_object->_polygons.resize(size);
	_object->_polygonMaterials.resize(size);
	//std::cout << "loading: " << _object->_polygons.size() << " polygons" << std::endl;
	// we can't load the entire chunk in one action, as we have to disregard the fourth values
	for(unsigned int i=0; i<_object->_polygons.size(); i++)
	{
		file.read((char*)&_object->_polygons[i], sizeof(_object->_polygons[i]));
		// we skip flag faces info
		file.seekg(file.tellg()+std::streampos(sizeof(short)));
		//std::cout << "polygon: " << i << " of : " << _object->_polygons.size() << " - " << _object->_polygons[i] << std::endl;
	}
	_object->calculateNormals();
	//std::cout << "\tloaded: " << _object->_polygons.size() << " polygons" << std::endl;
}

void Loader3DS::skipChunk( const int & length, std::ifstream & file )
{
	// in case of a chunk that does not interest us, we skip its length (minus 6 for header)
	file.seekg(file.tellg()+std::streampos(length-6));
}

void Loader3DS::loadFaceMaterials(std::ifstream & file )
{
	std::string name = loadName(file);
	short size = 0;
	file.read((char*)&size, sizeof(size));
	//file.read(&size, sizeof(size));
	//std::cout << "\tmaterial: " << name << " used by: " << size << " polygons" << std::endl;

	for(int i=0; i<size; i++)
	{
		short face = 0;
		file.read((char*)&face, sizeof(face));
		_object->_polygonMaterials[face] = name;
		//std::cout << "\t\tface: " << face << std::endl;
	}
	//std::cout << "finished face materials" << std::endl;
}

Engine::Point3D<int> Loader3DS::loadColor( std::ifstream & file )
{
	unsigned short id = 0;
	int length = 0;
	file.read((char*)&id, sizeof(id));
	file.read((char*)&length, sizeof(length));
	//std::cout << "new id: " << id << " and length: " << length << std::endl;

	unsigned char token = 0;
	Engine::Point3D<int> color;
	switch(id)
	{
		// char rgb
		case 17:
			file.read((char*)&token, sizeof(token));
			color._x = int(token);
			file.read((char*)&token, sizeof(token));
			color._y = int(token);
			file.read((char*)&token, sizeof(token));
			color._z = int(token);
		break;
		default:
			std::stringstream oss;
			oss << "Loader3DS::loadColor - unrecognized color id chunk: " << id << " with length: " << length;
			throw Engine::Exception(oss.str());
		break;
	}
	return color;
}

void Loader3DS::loadMaterial( std::ifstream & file, unsigned short & id, int & length )
{
	Material * material = new Material;

	std::string token("");

	while(file.good())
	{
		file.read((char*)&id, sizeof(id));
		file.read((char*)&length, sizeof(length));
		//std::cout << "id chunk: " << id << " with length: " << length << " and tellg: " << file.tellg() << std::endl;
	
		switch(id)
		{
			case 40960:
				token = loadName(file);
				//std::cout << "new material with name: " << token << std::endl;
				_model->registerMaterial(material, token);
			break;
			case 41472:
				//std::cout << "map name: " << std::endl;
			break;
			case 41728:
				material->setTextureFileName(loadName(file));
			break;

			case 40976:
				material->setAmbientColor(loadColor(file));
				break;
			
			case 40992:
				material->setDiffuseColor(loadColor(file));
				break;

			case 41008:
				material->setSpecularColor(loadColor(file));
				break;
			case 41024:
			case 41025:
			case 41040:
			case 41042:
			case 41043:
			case 41092:
			case 41095:
				//std::cout << "id chunk for material: " << id << " with length: " << length << std::endl;
				skipChunk(length, file);
			break;

			default:
				//std::cout << "material finished, with id: " << id << " and length: " << length << std::endl;
				return;
			break;
		}
	}
}

} // namespace GUI

