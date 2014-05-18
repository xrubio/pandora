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

#ifndef __ColorSelector_hxx__
#define __ColorSelector_hxx__

#include <QColor>
#include <Point3D.hxx>

namespace GUI
{

// ColorSelector follows the Strategy design pattern to allow multiple choices of the way rasters are painted in OpenGL

class ColorSelector
{
public:
	ColorSelector()
	{
	}

	virtual ~ColorSelector()
	{
	}

	virtual const QColor & getColor( const int & value ) const = 0;
	virtual unsigned int getNumIntervals() const = 0;
	virtual const int & getMinValue() const = 0;
	virtual const int & getMaxValue() const = 0;
	virtual const int & getBreak( const int & index ) const = 0;
	virtual void addBreak( const int & value ) = 0;

	virtual ColorSelector * copy() const = 0;
	virtual void setColor( const QColor & color, const int & index ) = 0;
};

} // namespace GUI

#endif // __ColorSelector_hxx__

