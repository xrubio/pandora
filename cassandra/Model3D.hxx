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

#ifndef __Model3D_hxx__
#define __Model3D_hxx__

#include <string>
#include <map>
#include <list>

#include <Point3D.hxx>

namespace GUI
{
class Object3D;
class Material;

class Model3D
{
public:
	typedef std::map<std::string, Material * > MaterialsMap;
private:
	typedef std::list<Object3D *> ObjectsList;

	ObjectsList _objects;
	MaterialsMap _materials;

	// this method normalizes all the object of the model, in order to have a Model3D of size 1
	void normalizeModel();

	Engine::Point3D<float> _modelScale;
	void registerTextures();
public:
	Model3D();
	virtual ~Model3D();

	void paint() const;

	// this method registers a new object inside Model, and returns pointer (to fill it)
	Object3D * createNewObject( const std::string & name );

	// register Material into MaterialsMap _materials. 
	// As we don't know the name of the material when we create the material, it can't be created and registered at the same time
	void registerMaterial( Material * material, const std::string & name );

	// this method is called after loading to execute initialization actions
	void init();
	void setModelScale( const Engine::Point3D<float> & modelScale );
};

} // namespace GUI

#endif // __Model3D_hxx__

