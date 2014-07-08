
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
#ifndef __INTERVAL_HXX_
#define __INTERVAL_HXX_

namespace Engine
{

template <typename Type> class Interval
{
public:
	
	Type _min;
	Type _max;
	
	Interval() : _min(0), _max(0)
	{
	}
	
	Interval(Type min, Type max) : _min(min), _max(max)
	{
	}	
	
	inline bool isInside(Type x)
	{
		return (x>=_min) && (x<=_max);
	}
	
	bool intersection(Interval other, Interval & result)
	{	
		if ( _min <= other._min )
		{
			if ( !isInside(other._min) ) return false;
			result._min = other._min;
			if ( isInside(other._max) )
				result._max = other._max;
			else
				result._max = _max;	
			return true;
		}
		if ( !other.isInside(_min) ) return false;
		result._min = _min;
		if ( other.isInside( _max ) )
			result._max = _max;
		else
			result._max = other._max;
		return true;
	}
	
	
	friend std::ostream & operator<<( std::ostream & stream, Interval<Type> & intv )
	{
		return stream << "[" << intv._min << ".." << intv._max << "]";
	}
	
};

}

#endif // Interval.hxx
