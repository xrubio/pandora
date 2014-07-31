
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

#ifndef __StaticRaster_hxx__
#define __StaticRaster_hxx__

#include <Point2D.hxx>
#include <Size.hxx>
#include <vector>

namespace Engine
{
class World;
class RasterLoader;

struct ColorEntry
{
	short _r;
	short _g;
	short _b;
	short _alpha;
	
	bool operator==(const ColorEntry& other) const {
		return _r == other._r && _g == other._g && _b == other._b && _alpha == other._alpha;
	}
	bool operator!=(const ColorEntry& other) const { return !(*this == other); }
};

//! this class is used to load a static raster map. Values can't be modified, and it won't be serialized each time step (only one time)
class StaticRaster
{
protected:
	std::vector< std::vector<int> >_values;

	int _minValue;
	int _maxValue;

	bool _hasColorTable;
	std::vector< ColorEntry > _colorTable;
public:
	StaticRaster();
	virtual ~StaticRaster();
	
	//! Equality operators
	bool operator==(const StaticRaster& other) const;
	bool operator!=(const StaticRaster& other) const;

	//! changes raster size. Parameter 'size' represents the new dimesions for the raster area.
	virtual void resize( const Size<int> & size );
	//! Reads the value in the cell located by parameter "position". Returns -1 if "position" is out of the area of the raster.
	virtual const int & getValue( const Point2D<int>& position ) const;

	//! Returns size of the raster codifying the horizontal and vertical dimensions in a Size object. 
	virtual Size<int> getSize() const;
	//! Reads attribute _maxValue.
	virtual const int & getMaxValue() const;
	//! Reads the '_minValue' attribute.
	const int & getMinValue() const;

	float getAvgValue() const;
	void updateMinMaxValues();

	void setColorTable( bool hasColorTable, int size);
	void addColorEntry( int index, short r, short g, short b, short alpha );
	int getNumColorEntries() const;
	bool hasColorTable() const;
	ColorEntry getColorEntry(int index ) const;
	
	friend class RasterLoader;
}; 

} // namespace Engine

#endif // __StaticRaster_hxx__

