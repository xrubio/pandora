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

#include <Material.hxx>

#include <iostream>
#include <sstream>

#include <Exception.hxx>

namespace GUI
{

Material::Material() :  _textureFileName("-"), _imageId(0), _textureId(0), _ambientColor(0,0,0), _diffuseColor(0,0,0), _specularColor(0,0,0)
{
}

Material::~Material()
{
	if(_imageId)
	{
		ilDeleteImages(1, &_imageId);
	}
}

void Material::registerTexture()
{
	if(!_imageId)
	{
		return;
	}

	glGenTextures(1,&_textureId);

	//std::cout << "generating textures: " << _textureId << " with image id: " << _imageId << std::endl;
	glBindTexture(GL_TEXTURE_2D, _textureId);
	ilBindImage(_imageId);	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
}

void Material::loadTextureImage()
{
	if(_textureFileName.compare("-")==0)
	{
		return;
	}
	ilInit();
	
	ilGenImages(1, &_imageId);
	ilBindImage(_imageId);

	//std::cout << "loading texture: " << _textureFileName << std::endl;
	std::string path("resources/3dmodels/"+_textureFileName);
	ILboolean result = ilLoadImage( path.c_str() ) ;
	if(!result)
	{
		ILenum error = ilGetError();
		std::stringstream oss;
		oss << "Model3D::loadTextures- error loading texture: " << error << " - " << " with path: " << path;
		throw Engine::Exception(oss.str());
		return;
	}
}

void Material::setTextureFileName( const std::string & textureFileName )
{
	_textureFileName = textureFileName;
	//std::cout << "material with texture file name: " << _textureFileName << std::endl;
	loadTextureImage();
}

bool Material::usesTexture()
{
	return _textureId;
}

void Material::activate()
{
	if(usesTexture())
	{
		glEnable(GL_TEXTURE_2D);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		glBindTexture(GL_TEXTURE_2D, _textureId);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
	glMaterialfv( GL_FRONT, GL_AMBIENT, &_ambientColor._x);	
	glMaterialfv( GL_FRONT, GL_DIFFUSE, &_diffuseColor._x);
	glMaterialfv( GL_FRONT, GL_SPECULAR, &_specularColor._x);
}

void Material::deactivate()
{
	if(usesTexture())
	{
		glDisable(GL_TEXTURE_2D);
	}
}

void Material::setAmbientColor( const Engine::Point3D<GLint> & ambientColor )
{
	_ambientColor._x = float(ambientColor._x)/255.0f;
	_ambientColor._y = float(ambientColor._y)/255.0f;
	_ambientColor._z = float(ambientColor._z)/255.0f;
}

void Material::setDiffuseColor( const Engine::Point3D<GLint> & diffuseColor )
{
	_diffuseColor._x = float(diffuseColor._x)/255.0f;
	_diffuseColor._y = float(diffuseColor._y)/255.0f;
	_diffuseColor._z = float(diffuseColor._z)/255.0f;
}

void Material::setSpecularColor( const Engine::Point3D<GLint> & specularColor)
{
	_specularColor._x = float(specularColor._x)/255.0f;
	_specularColor._y = float(specularColor._y)/255.0f;
	_specularColor._z = float(specularColor._z)/255.0f;
}

} // namespace GUI

