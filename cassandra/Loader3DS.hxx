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

#ifndef __Loader3DS_hxx__
#define __Loader3DS_hxx__

#include <string>
#include <fstream>

#include <Point3D.hxx>

namespace GUI
{

class Model3D;
class Object3D;

// this class is used to load a model generated with 3D Studio Max into Cassandra (as an agent)
// it is constructed as a singleton
class Loader3DS
{
	static Loader3DS * _instance;

	// this pointer is used to load a new model. it is not responsible of deleting it
	Model3D * _model;

	// helper to construct object
	Object3D * _object;

	// utility functions
	std::string loadName( std::ifstream & file );
	Engine::Point3D<int> loadColor( std::ifstream & file );
	void skipChunk( const int & length, std::ifstream & file );

	void loadVertexs( std::ifstream & file );
	void loadTextureMapCoordinates( std::ifstream & file );
	void loadPolygons( std::ifstream & file );

	void loadFaceMaterials(std::ifstream & file );

	void loadMaterial( std::ifstream & file, unsigned short & id, int & length );
protected:
	Loader3DS();	

public:
	static Loader3DS * instance();
	virtual ~Loader3DS();
	Model3D * loadModel( const std::string & file );
}; 

} // namespace GUI

#endif // __Loader3DS_hxx__

