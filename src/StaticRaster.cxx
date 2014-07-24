
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
#include <Exception.hxx>
#include <World.hxx>
#include <limits>
#include <iostream>

namespace Engine
{

StaticRaster::StaticRaster() : _minValue(std::numeric_limits<int>::max()), _maxValue(std::numeric_limits<int>::min()), _hasColorTable(false)
{
}

StaticRaster::~StaticRaster()
{
}

bool StaticRaster::operator==(const StaticRaster& other) const {
	return _minValue == other._minValue &&
	       _maxValue == other._maxValue &&
	       _values == other._values &&
	       _hasColorTable == other._hasColorTable &&
	       _colorTable == other._colorTable;
}

bool StaticRaster::operator!=(const StaticRaster& other) const {
	return !(*this == other);
}

void StaticRaster::resize( const Size<int> & size )
{
	_values.resize(size._width);
	for(int i=0; i<size._width; i++)
	{
		_values[i].resize(size._height);
	}
}

const int & StaticRaster::getValue( const Point2D<int>& position ) const
{
	if(position._x<0 || position._x>=_values.size())
	{
		std::stringstream oss;
		oss << "StaticRaster::getValue - " << position << " x out of bounds: " << _values.size();
		throw Exception(oss.str());
	}
	if(position._y<0 || position._y>=_values[position._x].size())
	{
		std::stringstream oss;
		oss << "StaticRaster::getValue - " << position << " y out of bounds: " << _values.size() << "/" << _values[position._x].size();
		throw Exception(oss.str());
	}
	return _values[position._x][position._y];
}

Size<int> StaticRaster::getSize() const
{
	if(_values.size()==0)
	{
		return Size<int>(0,0);
	}
	return Size<int>(_values.size(), _values[0].size());
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

	for(size_t i=0; i<_values.size(); i++)
	{
		for(size_t j=0; j<_values[i].size(); j++)
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
	for(size_t i=0; i<_values.size(); i++)
	{
		for(size_t j=0; j<_values[i].size(); j++)
		{
			_minValue = std::min(_minValue, _values[i][j]);
			_maxValue = std::max(_maxValue, _values[i][j]);
		}
	}
}

void StaticRaster::setColorTable( bool hasColorTable, int size )
{
	_hasColorTable = hasColorTable;
	if(_colorTable.size()!=0)
	{
		_colorTable.clear();
	}
	_colorTable.resize(size);
}

void StaticRaster::addColorEntry( int index, short r, short g, short b, short alpha )
{
	if(!_hasColorTable)
	{	
		std::stringstream oss;
		oss << "StaticRaster::addColorEntry - adding color entry without color table";
		throw Exception(oss.str());
		return;
	}
	if(index>=_colorTable.size())
	{
		std::stringstream oss;
		oss << "StaticRaster::addColorEntry - index: " << index << " out of bounds with size: " << _colorTable.size();
		throw Exception(oss.str());
		return;
	}
	ColorEntry entry;
	entry._r = r;
	entry._g = g;
	entry._b = b;
	entry._alpha = alpha;
	_colorTable.at(index) = entry;
}
	
int StaticRaster::getNumColorEntries() const
{
	return _colorTable.size();
}

ColorEntry StaticRaster::getColorEntry(int index ) const
{
	if(!_hasColorTable)
	{	
		std::stringstream oss;
		oss << "StaticRaster::getColorEntry - getting color entry without color table";
		throw Exception(oss.str());
	}
	if(index>=_colorTable.size())
	{
		std::stringstream oss;
		oss << "StaticRaster::getColorEntry - index: " << index << " out of bounds with size: " << _colorTable.size();
		throw Exception(oss.str());
	}
	return _colorTable.at(index);
}

bool StaticRaster::hasColorTable() const
{
	return _hasColorTable;
}

} // namespace Engine

