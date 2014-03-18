
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

#ifndef __Rectangle_hxx__
#define __Rectangle_hxx__

#include <sstream>
#include <cmath>

#include <cstdlib>

#include <Interval.hxx> 
#include <Point2D.hxx>
#include <Size.hxx>

#include <vector>

namespace Engine
{
// 2D Rectangle
template <typename Type> class Rectangle
{
public:
	// top left point
	Point2D<Type> _origin;
	Size<Type> _size;

	Rectangle() : _origin(-1,-1), _size(-1,-1)
	{
	}

	Rectangle( const Size<Type> & size, const Point2D<Type> & origin = Point2D<Type>(0,0)) : _origin(origin), _size(size)
	{
	}

	Rectangle( const Type & left, const Type & top, const Type & right, const Type & bottom ) : _origin(left, top), _size(1+right-left, 1+bottom-top)
	{
	}

	Rectangle( const Interval<Type> & OX, const Interval<Type> & OY)
	{		
		this->intervals2Rectangle( OX, OY);		
	}
	
	bool contains( const Point2D<Type> & point ) const
	{
		if(point._x < _origin._x || point._x >= _origin._x+_size._width)
		{
			return false;
		}
		if(point._y < _origin._y || point._y >= _origin._y+_size._height)
		{
			return false;
		}
		return true;
	}

	void intervals2Rectangle( const Interval<Type> & iOX, const Interval<Type> & iOY)
	{		
		// Point2D origin = Point2D(iOX._min,iOY._min);		
		_origin._x = iOX._min;
		_origin._y = iOY._min;
		
		// Point2D size = Point2D(iOX._max – iOX._min +1, iOY._max – iOY._min +1);	
		_size._width = iOX._max - iOX._min +1;
		_size._height = iOY._max - iOY._min +1;		
	}	
	
	bool intersection(const Rectangle<Type> other, Rectangle<Type> & result)
	{	
		// Interval caracterization for Rectangle "this"
		Interval<Type> R1_OX(_origin._x,_origin._x + _size._width -1);		
		Interval<Type> R1_OY(_origin._y,_origin._y + _size._height -1);
		
		// Interval caracterization for Rectangle "other"
		Interval<Type> R2_OX(other._origin._x,other._origin._x + other._size._width -1);		
		Interval<Type> R2_OY(other._origin._y,other._origin._y + other._size._height -1);	

		Interval<Type> Intersection_OX;
		if (! R1_OX.intersection(R2_OX,Intersection_OX) )
		{
			return false;
		}
		
		
		Interval<Type> Intersection_OY;
		if (! R1_OY.intersection(R2_OY,Intersection_OY) )
		{
			return false;
		}

		result.intervals2Rectangle(Intersection_OX,Intersection_OY);
		
		return true;	
	}
	
	
	friend std::ostream & operator<<( std::ostream & stream, const Rectangle<Type> & rectangle )
	{
		return stream << "rect:" << rectangle._origin << "->" << Point2D<Type>(rectangle._origin._x+rectangle._size._width, rectangle._origin._y+rectangle._size._height) - 1;
	}

	Rectangle<Type> & operator=( const Rectangle<Type> & rectangle)
	{
		_origin = rectangle._origin;
		_size = rectangle._size;
		return *this;
	}

	Rectangle<Type> clone() const
	{
		return Rectangle<int>(_origin, _size);
	}
	
	bool operator==( const Rectangle<Type> & rectangle ) const
	{
		return _origin==rectangle._origin && _size==rectangle._size;
	}
	
	bool operator!=( const Rectangle<Type> & rectangle ) const
	{
		return !((*this)==rectangle);
	}

public:	
	class iterator
	{	
		int _initialX;
		Point2D<int> _data;
		int _width;
	public:
		iterator( const Point2D<int> origin, int width ) : _initialX(origin._x), _data(origin), _width(width)
		{
		}
		Point2D<int> & operator*()
		{
			return _data;
		}

		iterator operator++()
		{
			if(_data._x<(_initialX+_width-1))
			{
				_data._x++;
			}
			else
			{
				_data._x = _initialX;
				_data._y++;
			}
			return *this;
		}

		bool operator!=( const iterator & other )
		{
			return _data != other._data;
		}
	};

	std::vector<int> _foo;	
	
	iterator begin()
	{
		return iterator(_origin, _size._width); 
	}
	
	iterator end()
	{
		return iterator(_origin+Point2D<int>(0,_size._height), _size._width); 
	}
};

} // namespace Engine

#endif // __Rectangle_hxx__

