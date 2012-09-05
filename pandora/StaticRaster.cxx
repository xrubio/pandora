
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

#include <StaticRaster.hxx>
#include <Exceptions.hxx>
#include <World.hxx>
#include <limits>

namespace Engine
{

StaticRaster::StaticRaster() : _minValue(std::numeric_limits<int>::max()), _maxValue(std::numeric_limits<int>::min())
{
}

StaticRaster::~StaticRaster()
{
}

void StaticRaster::resize( const Point2D<int> & size )
{
	_values.resize(size._x);
	// TODO explore how to improve performance with STL algorithms
	// for_each(_valuesField.begin(),_valuesField.end(),resize(_matrixSize));
	for(int i=0; i<size._x; i++)
	{
		_values[i].resize(size._y);
	}
}

void StaticRaster::setDefaultInitValues( int minValue, int maxValue, int defaultValue )
{
	_minValue = minValue;
	_maxValue = maxValue;
	for(int i=0; i<_values.size(); i++)
	{
		for(int j=0; j<_values[i].size(); j++)
		{		
			setInitValue(Point2D<int>(i,j), defaultValue);			
		}
	}
}

const int & StaticRaster::getValue( Point2D<int> position ) const
{
	if(position._x<0 || position._x>=_values.size())
	{
		std::stringstream oss;
		oss << "StaticRaster::getValue - " << position << " x out of bounds: " << _values.size();
		throw Exception(oss.str());
		return -1;
	}
	if(position._y<0 || position._y>=_values[position._x].size())
	{
		std::stringstream oss;
		oss << "StaticRaster::getValue - " << position << " y out of bounds: " << _values.size() << "/" << _values[position._x].size();
		throw Exception(oss.str());
		return -1;
	}
	return _values[position._x][position._y];
}

void StaticRaster::setInitValue( Point2D<int> position, int value )
{
	if(value>_maxValue)
	{
		std::stringstream oss;
		oss << "Raster::setInitValue - value: " << value << " bigger than max value: " << _maxValue << " at position: " << position;
		throw Exception(oss.str());

		return;
	}
	if(position._x<0 || position._x>=_values.size())
	{
		std::stringstream oss;
		oss << "Raster::setInitValue - " << position << " x out of bounds: " << _values.size();
		throw Exception(oss.str());

		return;
	}
	if(position._y<0 || position._y>=_values[position._x].size())
	{
		std::stringstream oss;
		oss << "Raster::setInitValue - " << position << " y out of bounds: " << _values.size() << "/" << _values[position._x].size();
		throw Exception(oss.str());
		return;
	}
	_values[position._x][position._y] = value;
}

Point2D<int> StaticRaster::getSize() const
{
	if(_values.size()==0)
	{
		return Point2D<int>(0,0);
	}
	return Point2D<int>(_values.size(), _values[0].size());
}

const int & StaticRaster::getMinValue() const
{
	return _minValue;
}

const int & StaticRaster::getMaxValue() const
{
	return _maxValue;
}

float StaticRaster::getAvgValue() const
{
	if(_values.size()==0) 
	{
		return 0.0f;
	}
	float norm = 0.0f;
	float avg = 0.0f;

	for(int i=0; i<_values.size(); i++)
	{
		for(int j=0; j<_values[i].size(); j++)
		{		
			norm += 1.0;
			avg += _values[i][j];
		}
	}
	return avg / norm;
}

void StaticRaster::updateMinMaxValues()
{
	_minValue = std::numeric_limits<int>::max();
	_maxValue = std::numeric_limits<int>::min();
	for(int i=0; i<_values.size(); i++)
	{
		for(int j=0; j<_values[i].size(); j++)
		{
			_minValue = std::min(_minValue, _values[i][j]);
			_maxValue = std::max(_maxValue, _values[i][j]);
		}
	}
}

} // namespace Engine

