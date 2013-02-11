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

#ifndef __Configuration3D_hxx__
#define __Configuration3D_hxx__

#include <Point3D.hxx>
#include <string>
#include <list>

namespace GUI
{

class Configuration3D
{
	Engine::Point3D<float> _size;

	std::list<std::string> _rasters;
	std::string _dem;	
	std::string _color;

public:
	Configuration3D();
	virtual ~Configuration3D();
	void setRastersList( const std::list<std::string> & rasters );

	void setSize3D( const Engine::Point3D<float> & size );
	const Engine::Point3D<float> & getSize3D() const;
	std::list<std::string>::const_iterator beginRasters() const;
	std::list<std::string>::const_iterator endRasters() const;

	void setRasters( std::string dem, std::string color );
	const std::string getDEMRaster() const;
	const std::string getColorRaster() const;
};

} // namespace GUI

#endif // __Configuration3D_hxx__

