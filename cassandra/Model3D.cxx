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

#include <Model3D.hxx>

#include <Point2D.hxx>
#include <Point3D.hxx>

#include <Object3D.hxx>
#include <Material.hxx>

#include <iostream>

namespace GUI
{

Model3D::Model3D() : _modelScale(1.0f, 1.0f, 1.0f)
{
}

Model3D::~Model3D()
{

	ObjectsList::iterator it=_objects.begin(); 
	while(it!=_objects.end())
	{
		Object3D * object = *it;
		it = _objects.erase(it);
		delete object;
	}
	
	for(MaterialsMap::iterator it=_materials.begin(); it!=_materials.end(); it++)
	{
		delete it->second;
	}
}

void Model3D::normalizeModel()
{
	float maxValue = 0.0f;
	for(ObjectsList::iterator it=_objects.begin(); it!=_objects.end(); it++)
	{
		Object3D * object = (*it);
		for(int i=0; i<object->_vertexs.size(); i++)
		{
			//std::cout << "vertex: " << i << " - " << object->_vertexs[i] << " of: " << object->_vertexs.size() << std::endl;
			maxValue = std::max(maxValue, std::abs(object->_vertexs[i]._x));
			maxValue = std::max(maxValue, std::abs(object->_vertexs[i]._y));
			maxValue = std::max(maxValue, std::abs(object->_vertexs[i]._z));
		}
	}	
	//std::cout << "max value: " << maxValue << std::endl;
	
	for(ObjectsList::iterator it=_objects.begin(); it!=_objects.end(); it++)
	{
		Object3D * object = (*it);
		for(int i=0; i<object->_vertexs.size(); i++)
		{	
			object->_vertexs[i] = object->_vertexs[i]/maxValue;
		}
	}
}

void Model3D::paint() const
{
	glPushMatrix();
	for(ObjectsList::const_iterator it=_objects.begin(); it!=_objects.end(); it++)
	{
		Object3D * object = (*it);
		object->paint(_modelScale);
	}
	glPopMatrix();
	glFlush();
}
	
void Model3D::registerTextures()
{
	for(MaterialsMap::iterator it=_materials.begin(); it!=_materials.end(); it++)
	{
		Material * material = (Material*)it->second;
		material->registerTexture();
	}
}

Object3D * Model3D::createNewObject( const std::string & name )
{
	// TODO convert objects list into map?
	Object3D * object = new Object3D(name, _materials);
	_objects.push_front(object);
	return object;
}

void Model3D::registerMaterial( Material * material, const std::string & name )
{
	_materials.insert(make_pair(name, material));
}

void Model3D::init()
{
	//std::cout << "model created with " << _objects.size() << " objects and: " << _materials.size() << " materials" << std::endl;
	for(ObjectsList::iterator it=_objects.begin(); it!=_objects.end(); it++)
	{
		Object3D * object = (*it);
		//std::cout << "\tobject: " << object->_name << " num vertexs: " << object->_vertexs.size() << " and num polygons: " << object->_polygons.size() << std::endl;
	}	
	for(MaterialsMap::iterator it=_materials.begin(); it!=_materials.end(); it++)
	{
		//std::cout << "\tmaterial: " << it->first << std::endl;
	}
	normalizeModel();
	registerTextures();
}
	
void Model3D::setModelScale( const Engine::Point3D<float> & modelScale )
{
	_modelScale = modelScale;
}

} // namespace GUI

