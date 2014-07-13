
/*
 * Copyright (c) 2012
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

#ifndef __Point3D_hxx__
#define __Point3D_hxx__

#include <sstream>
#include <cmath>

namespace Engine
{

template <typename Type> class Point3D
{
public:
	Type _x;
	Type _y;
	Type _z;

	Point3D()
	{
		_x = 0;
		_y = 0;
	}

	Point3D( const Type & x, const Type & y, const Type & z)
	{
		_x = x;
		_y = y;
		_z = z;
	}
	friend std::ostream & operator<<( std::ostream & stream, const Point3D<Type> & point )
	{
		return stream << point._x << "/" << point._y << "/" << point._z;
	}

	//! Point Comparaison : Equality within an cube around the caller object.
	/*!
	The param "point" is considered equal if it falls in the cube ((_x-delta,_y-delta,_z-delta)..(_x+delta,_y+delta,_z+delta)).
	Assuming ((a,b,c)..(d,e,f)) a cube with top left corner (a,b,c) and bottom right corner (d,e,f).
	*/
	bool isEqual( const Point3D<Type> & point, const Type & delta = 0 )
	{
		if(std::abs(_x-point._x)>delta)
		{
			return false;
		}
		if(std::abs(_y-point._y)>delta)
		{
			return false;
		}
		if(std::abs(_z-point._z)>delta)
		{
			return false;
		}
		return true;
	}
	
	Point3D<Type> & operator=( const Point3D<Type> & point)
	{
		_x = point._x;
		_y = point._y;
		_z = point._z;

		return *this;
	}	
	
	// global arithmethic operations
	Point3D<Type> operator+( const Type & value ) const
	{
		return Point3D<Type>(_x+value,_y+value,_z+value);
	}
	Point3D<Type> operator-( const Type & value ) const
	{
		return Point3D<Type>(_x-value,_y-value,_z-value);
	}
	Point3D<Type> operator*( const Type & value) const
	{
		return Point3D<Type>(_x*value,_y*value,_z*value);
	}
	Point3D<Type> operator/( const Type & value) const
	{
		return Point3D<Type>(_x/value,_y/value,_z/value);
	}

	// point to point operations
	Point3D<Type> operator+( const Point3D<Type> & point ) const
	{
		return Point3D<Type>(_x+point._x,_y+point._y,_z+point._z);
	}	
	Point3D<Type> operator-( const Point3D<Type> & point ) const
	{
		return Point3D<Type>(_x-point._x,_y-point._y,_z-point._z);
	}	
	Point3D<Type> operator*( const Point3D<Type> & point ) const
	{
		return Point3D<Type>(_x*point._x,_y*point._y,_z*point._z);
	}	
	Point3D<Type> operator/( const Point3D<Type> & point ) const
	{
		return Point3D<Type>(_x/point._x,_y/point._y,_z/point._z);
	}	

	//! Euclidean Distance between points.
	Type distance( const Point3D<Type> & point) const
	{
		//Type valueX = std::abs(_x-point._x);
		//Type valueY = std::abs(_y-point._y);
		//Type valueZ = std::abs(_z-point._z);
		Type valueX = (_x-point._x);
		Type valueY = (_y-point._y);
		Type valueZ = (_z-point._z);
		return sqrt(valueX*valueX + valueY*valueY + valueZ*valueZ);
	}
	
	//! TODO : describe "Normalize"
	Point3D<float> normalize() const
	{
		Point3D<float> normalized;
		float length = sqrt(_x*_x + _y*_y + _z*_z);
		if(length!=0.0f)
		{
			length = 1.0f/length;
			normalized._x = (float)_x * length;
			normalized._y = (float)_y * length;
			normalized._z = (float)_z * length;
		}
		return normalized;
	}
	
	
	//! TODO : describe "crossProduct"
	Point3D<Type> crossProduct( const Point3D<Type> & vector ) const
	{
		Point3D<Type> result;
		result._x = _y*vector._z - _z*vector._y;
		result._y = _z*vector._x - _x*vector._z;
		result._z = _x*vector._y - _y*vector._x;
		return result;
	}	
	
	Type dot( const Point3D<Type> & point ) const
	{
		return _x*point._x + _y*point._y + _z*point._z;
	}
};

} // namespace Engine

#endif // __Point3D_hxx__

