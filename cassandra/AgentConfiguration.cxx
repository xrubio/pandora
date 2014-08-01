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

#include <AgentConfiguration.hxx>
#include <sstream>
#include <iostream>
#include <Exception.hxx>
#include <Loader3DS.hxx>
#include <Model3D.hxx>
#include <cstdlib>

namespace GUI
{

AgentConfiguration::AgentConfiguration() : _color(rand()%256,rand()%256,rand()%256), _icon(0), _useIcon(false), _fileName2D(""), _size(1.0f), _size3D(1.0f, 1.0f, 1.0f), _fileName3D(""), _model(0), _showValue(false)
{ 
    _color = QColor::fromHsv(rand() % 256, 255, 190);

    std::stringstream oss;
    oss <<getenv("PANDORAPATH") << "/share/cassandra/3dmodels/cube.3ds";
	setFileName3D( oss.str() );
}

AgentConfiguration::AgentConfiguration( const AgentConfiguration & prototype ) : _color(prototype.getColor()), _icon(0), _useIcon(prototype.useIcon()), _fileName2D(prototype.getFileName2D()), _size(prototype.getSize()), _size3D(prototype.getSize3D()), _fileName3D(prototype.getFileName3D()), _model(0), _showValue(prototype.showValue())
{
	if(!_fileName2D.empty())
	{
		_icon = new QIcon(prototype.getIcon());
	}
	
	if(!_fileName3D.empty())
	{
		_model = Loader3DS::instance()->loadModel(_fileName3D);
		_model->setModelScale(_size3D);
	}
}

AgentConfiguration::~AgentConfiguration()
{
	if(_icon)
	{
		delete _icon;
	}
	if(_model)
	{
		delete _model;
	}
}

void AgentConfiguration::setColor( const QColor & color )
{
	_color = color;
}

void AgentConfiguration::setUseIcon( const bool & useIcon )
{
	_useIcon = useIcon;
}

const bool & AgentConfiguration::useIcon() const
{
	return _useIcon;
}

const QColor & AgentConfiguration::getColor() const
{
	return _color;
}

const QIcon & AgentConfiguration::getIcon() const
{
	return *_icon;
}
	
void AgentConfiguration::setSize( const float & size )
{
	_size = size;
}
	
const float & AgentConfiguration::getSize() const
{
	return _size;
}

const Engine::Point3D<float> & AgentConfiguration::getSize3D() const
{
	return _size3D;
}

void AgentConfiguration::setSize3D( const Engine::Point3D<float> & size3D )
{
	_size3D = size3D;
	if(_fileName3D.empty())
	{
		return;
	}
	_model->setModelScale(_size3D);
}

void AgentConfiguration::setFileName3D( const std::string & fileName3D)
{
	// TODO maybe we should wait for 'accept' in agentconfigurator...
	if(_fileName3D.compare(fileName3D)==0)
	{
		return;
	}
	if(!_fileName3D.empty())
	{
		delete _model;
		_model = 0;
	}
	_fileName3D = fileName3D;
	if(_fileName3D.empty())
	{
		return;
	}
	_model = Loader3DS::instance()->loadModel(_fileName3D);
	_model->setModelScale(_size3D);
}

const std::string & AgentConfiguration::getFileName3D() const
{
	return _fileName3D;
}

void AgentConfiguration::setFileName2D( const std::string & fileName2D)
{
	if(_fileName2D.compare(fileName2D)==0)
	{
		return;
	}
	if(!_fileName2D.empty())
	{
		delete _icon;
		_icon = 0;
	}
	_fileName2D = fileName2D;	
	if(_fileName2D.empty())
	{
		return;
	}
	_icon = new QIcon(_fileName2D.c_str());
}

const std::string & AgentConfiguration::getFileName2D() const
{
	return _fileName2D;
}
	
const Model3D & AgentConfiguration::getModel() const
{
	return *_model;
}

} // namespace GUI

