/*
 * Copyright (c) 2014
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software; 
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or (at your option) any later version.
 * 
 * Pandora is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef __Size_hxx__
#define __Size_hxx__

#include <sstream>
#include <cmath>

namespace Engine
{

template <typename Type> class Size 
{
public:
	Type _width;
	Type _height;

	Size() : _width(0), _height(0)
	{
	}

	Size( const Type & width, const Type & height ) : _width(width), _height(height)
	{
	}

	Size<Type> clone() const
	{
		return Size<Type>(_width, _height);
	}

	friend std::ostream & operator<<( std::ostream & stream, const Size<Type> & size )
	{
		return stream << size._width << "/" << size._height;
	}
	
	bool isEqual( const Size<Type> & size, const double & delta = 0.0001 ) const
	{
		return distance(size)<=delta;
	}

	//! Euclidean Distance between sizes
	double distance( const Size<Type> & size) const
	{
		double diffWidth = _width-size._width;
		double diffHeight = _height-size._height;
		return sqrt(diffWidth*diffWidth+diffHeight*diffHeight);
	}

	bool operator==( const Size<Type> & size ) const
	{
		return isEqual(size);
	}
	
	bool operator!=( const Size<Type>& size) const
	{
		return !isEqual(size);
	}

	// global arithmethic operations
	Size<Type> operator+( const Type & value ) const
	{
		return Size<Type>(_width+value,_height+value);
	}
	Size<Type> operator-( const Type & value ) const
	{
		return Size<Type>(_width-value,_height-value);
	}
	Size<Type> operator*( const Type & value) const
	{
		return Size<Type>(_width*value,_height*value);
	}
	Size<Type> operator/( const Type & value) const
	{
		return Size<Type>(_width/value,_height/value);
	}

	// point to point operations
	Size<Type> operator+( const Size<Type> & point ) const
	{
		return Size<Type>(_width+point._width,_height+point._height);
	}	
	Size<Type> operator-( const Size<Type> & point ) const
	{
		return Size<Type>(_width-point._width,_height-point._height);
	}	
	Size<Type> operator*( const Size<Type> & point ) const
	{
		return Size<Type>(_width*point._width,_height*point._height);
	}	
	Size<Type> operator/( const Size<Type> & point ) const
	{
		return Size<Type>(_width/point._width,_height/point._height);
	}	

	
};

} // namespace Engine

#endif // __Size_hxx__

