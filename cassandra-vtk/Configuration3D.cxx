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

#include <Configuration3D.hxx>

namespace GUI
{

Configuration3D::Configuration3D(): _dem("unknown"), _color("unknown")
{
	_size._x = 1.0f;
	_size._y = 1.0f;
	_size._z = 1.0f;
}

Configuration3D::~Configuration3D()
{
}

void Configuration3D::setRastersList( const std::list<std::string> & rasters )
{
	_rasters.clear();
	_rasters = rasters;
	if(_rasters.size()!=0)
	{
		_dem = *(_rasters.begin());
		_color = *(_rasters.begin());
	}
	else
	{
		_dem = "unknown";
		_color = "unknown";
	}
}

void Configuration3D::setSize3D( const Engine::Point3D<float> & size )
{
	_size = size;
}

const Engine::Point3D<float> & Configuration3D::getSize3D() const
{
	return _size;
}

std::list<std::string>::const_iterator Configuration3D::beginRasters() const
{
	return _rasters.begin();
}

std::list<std::string>::const_iterator Configuration3D::endRasters() const
{
	return _rasters.end();
}
	
void Configuration3D::setRasters( std::string dem, std::string color )
{
	_dem = dem;
	_color = color;
}

const std::string Configuration3D::getDEMRaster() const
{
	return _dem;
}

const std::string Configuration3D::getColorRaster() const
{
	return _color;
}

} // namespace GUI

