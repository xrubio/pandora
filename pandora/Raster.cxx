
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

#include <Raster.hxx>
#include <RasterLoader.hxx>
#include <Exceptions.hxx>

#include <sstream>
#include <iostream>
#include <limits>

namespace Engine
{

Raster::Raster()
{
}

Raster::~Raster()
{
}

void Raster::resize( const Point2D<int> & size )
{
	StaticRaster::resize(size);
	_maxValues.resize(size._x);
	// TODO explore how to improve performance with STL algorithms
	// for_each(_valuesField.begin(),_valuesField.end(),resize(_matrixSize));
	for(int i=0; i<size._x; i++)
	{
		_maxValues[i].resize(size._y);
	}
}

void Raster::updateRasterIncrement()
{
	for(int i=0; i<_values.size(); i++)
	{
		for(int j=0; j<_values[i].size(); j++)
		{
			if(_values.at(i).at(j) < _maxValues[i][j])
			{
				_values.at(i).at(j)++;
			}
		}
	}
}

void Raster::updateRasterToMaxValues()
{
	// std::copy is deep oriented.
	std::copy(_maxValues.begin(), _maxValues.end(), _values.begin());
}

int Raster::getMaxValueAt( Point2D<int> position )
{
	if(position._x<0 || position._x>=_maxValues.size())
	{
		std::stringstream oss;
		oss << "Raster::getMaxValueAt - " << position << " x out of bounds: " << _maxValues.size();
		throw Exception(oss.str());
		return -1;
	}
	if(position._y<0 || position._y>=_maxValues[position._x].size())
	{
		std::stringstream oss;
		oss << "Raster::getMaxValueAt - " << position << " y out of bounds: " << _maxValues.size() << "/" << _maxValues[position._x].size();
		throw Exception(oss.str());
		return -1;
	}	
	return _maxValues[position._x][position._y];
}

void Raster::setValue( Point2D<int> position, int value )
{
	if(value>_maxValue)
	{
		std::stringstream oss;
		oss << "Raster::setValue - value: " << value << " bigger than max value: " << _maxValue << " at position: " << position;
		throw Exception(oss.str());

		return;
	}
	if(position._x<0 || position._x>=_values.size())
	{
		std::stringstream oss;
		oss << "Raster::setValue - " << position << " x out of bounds: " << _values.size();
		throw Exception(oss.str());

		return;
	}
	if(position._y<0 || position._y>=_values[position._x].size())
	{
		std::stringstream oss;
		oss << "Raster::setValue - " << position << " y out of bounds: " << _values.size() << "/" << _values[position._x].size();
		throw Exception(oss.str());
		return;
	}
	if(value>_maxValues[position._x][position._y])
	{
		std::stringstream oss;
		oss << "Raster::setValue - value: " << value << " bigger than max value: " << _maxValues[position._x][position._y] << " at position: " << position;
		throw Exception(oss.str());
	}
	_values[position._x][position._y] = value;
}

void Raster::setMaxValue( Point2D<int> position, int value )
{
	if(value>_maxValue)
	{
		std::stringstream oss;
		oss << "Raster::setMaxValue - value: " << value << " bigger than max value: " << _maxValue << " at pos: " << position;
		throw Exception(oss.str());

		return;
	}

	if(position._x<0 || position._x>=_maxValues.size())
	{
		std::stringstream oss;
		oss << "Raster::setMaxValue- " << position << " x out of bounds: " << _maxValues.size();
		throw Exception(oss.str());

		return;
	}
	if(position._y<0 || position._y>=_maxValues[position._x].size())
	{
		std::stringstream oss;
		oss << "Raster::setMaxValue- " << position << " y out of bounds: " << _maxValues.size() << "/" << _maxValues[position._x].size();
		throw Exception(oss.str());
		return;
	}	
	_maxValues[position._x][position._y] = value;
}

void Raster::setMaxValue( const int & maxValue )
{
	_maxValue = maxValue;
}

void Raster::setMinValue( const int & minValue )
{
	_minValue = minValue;
}

void Raster::updateCurrentMinMaxValues()
{
	_currentMinValue = std::numeric_limits<int>::max();
	_currentMaxValue = std::numeric_limits<int>::min();
	for ( unsigned i = 0; i < _values.size(); i++  )
		for ( unsigned j = 0; j < _values[i].size(); j++ )
		{
			_currentMaxValue = ( _values[i][j] > _currentMaxValue ? _values[i][j] : _currentMaxValue );
			_currentMinValue = ( _values[i][j] < _currentMinValue ? _values[i][j] : _currentMinValue );
		}
}

void Raster::setInitValues( int minValue, int maxValue, int defaultValue )
{
	_minValue = _currentMinValue = minValue;
	_maxValue = _currentMaxValue = maxValue;
	for(int i=0; i<_values.size(); i++)
	{
		for(int j=0; j<_values[i].size(); j++)
		{
			setMaxValue(Point2D<int>(i,j), _maxValue);
			setValue(Point2D<int>(i,j), defaultValue);			
		}
	}
}

} // namespace Engine

