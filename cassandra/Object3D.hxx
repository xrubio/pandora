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

#ifndef __Object3D_hxx__
#define __Object3D_hxx__

#include <vector>
#include <string>

#include <Point2D.hxx>
#include <Point3D.hxx>

#include <Model3D.hxx>

namespace GUI
{
class Material;
class Loader3DS;

class Object3D
{
	// name of the model
	std::string _name; 

	// list of vertexs
	std::vector< Engine::Point3D<float> > _vertexs;
	// for each vertexs, we have a texture coordinate
	std::vector< Engine::Point2D<float> > _textureMapCoordinates;

	// list of polygons. a polygon is defined as 3 indexs pointing towards _vertexs
	std::vector< Engine::Point3D<unsigned short> > _polygons;
	// each polygon is related to the particular material used to paint it.
	std::vector< std::string > _polygonMaterials;
	// each polygon has a normal, that is calculated during loading time
	std::vector< Engine::Point3D<float> > _normals;

	// reference to the complete map of materials for this model
	Model3D::MaterialsMap & _materials;
public:
	Object3D( const std::string & name, Model3D::MaterialsMap & materials);
	virtual ~Object3D();
	
	void setMaterialName( const std::string & name );
	const std::string & getMaterialName();

	void calculateNormals();
	void paint( const Engine::Point3D<float> & modelScale ) const;

	friend class Loader3DS;
	friend class Model3D;
};

} // namespace GUI

#endif // __Object3D_hxx__

