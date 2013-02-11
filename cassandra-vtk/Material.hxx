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

#ifndef __Material_hxx__
#define __Material_hxx__

#include <string>
#include <GL/gl.h>
#include <il.h>

#include <Point3D.hxx>

namespace GUI
{

class Material
{
	std::string _textureFileName;

	// texture id inside DevIL
	ILuint _imageId;
	// texture id inside openGL
	GLuint _textureId;

	Engine::Point3D<GLfloat> _ambientColor;
	Engine::Point3D<GLfloat> _diffuseColor;
	Engine::Point3D<GLfloat> _specularColor;

	void loadTextureImage();
	
public:
	Material();
	virtual ~Material();

	void registerTexture();
	void setTextureFileName( const std::string & textureFileName );
	void setAmbientColor( const Engine::Point3D<GLint> & ambientColor );
	void setDiffuseColor( const Engine::Point3D<GLint> & diffuseColor );
	void setSpecularColor( const Engine::Point3D<GLint> & specularColor);

	bool usesTexture();
	void activate();
	void deactivate();
};

} // namespace GUI

#endif // __Material_hxx__

