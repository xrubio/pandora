
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
#include <vector>

namespace Engine
{
class World;
class RasterLoader;

//! this class is used to load a static raster map. Values can't be modified, and it won't be serialized each time step (only one time)
class StaticRaster
{
protected:
	std::vector< std::vector<int> >_values;

	int _minValue;
	int _maxValue;
public:
	StaticRaster();
	virtual ~StaticRaster();

	// TODO not squared rasters
	//! changes raster size. Parameter 'size' represents the new dimesions for the raster area.
	virtual void resize( const Point2D<int> & size );
	//! Reads the value in the cell located by parameter "position". Returns -1 if "position" is out of the area of the raster.
	virtual const int & getValue( Point2D<int> position ) const;

	//! Returns size of the raster codifying the horizontal and vertical dimensions in a Point2D object. 
	Point2D<int> getSize() const;
	//! Initializes the components of vector '_values' to defaultValue, and to maxValue the ones from vector _maxValue.
	void setDefaultInitValues( int minValue, int maxValue, int defaultValue );	
	//! Assigns the value "value" to the cell located by parameter "position". Does nothing if "position" is out of the area of the raster.
	void setInitValue( Point2D<int> position, int value );
	//! Reads attribute _maxValue.
	const int & getMaxValue() const;
	//! Reads the '_minValue' attribute.
	const int & getMinValue() const;

	float getAvgValue() const;
	void updateMinMaxValues();
	
	friend class RasterLoader;
}; 

} // namespace Engine

#endif // __StaticRaster_hxx__

