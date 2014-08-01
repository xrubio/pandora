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

#ifndef __RasterConfiguration_hxx__
#define __RasterConfiguration_hxx__

#include <string>
#include <Point3D.hxx>

namespace GUI
{
class ColorSelector;

class RasterConfiguration
{
	ColorSelector * _colorSelector;
	int _minValue;
	int _maxValue;

	bool _transparentEnabled;
	int _transparentValue;

	// raster being used for z coordinates in 3D
	std::string _elevationRaster;
	// meters per cell
	float _cellResolution;
	// deformation of z coordinates
	float _elevationExaggeration;
	// offset (in times the size of the raster)
	Engine::Point3D<float> _offset;

	// quad tree config
	// level of detail
	int _lod;	

	bool _hasElevationRaster;
    
    bool _showValues;
    bool _showBorders;

public:
	RasterConfiguration( const int & minValue = 0, const int & maxValue = 10, bool init = true);
	RasterConfiguration( const RasterConfiguration & prototype );
	virtual ~RasterConfiguration();

	ColorSelector & getColorRamp();
	const ColorSelector & getColorRamp() const;
	void resetColorRamp();

	const int & getMinValue() const;
	const int & getMaxValue() const;

	const bool & isTransparentEnabled() const;
	void setTransparentEnabled( const bool & transparentEnabled );
	const int & getTransparentValue() const;
	void setTransparentValue( const int & transparentValue );
    void showValues( const bool & showValues ) { _showValues = showValues; }
    void showBorders( const bool & showBorders ) { _showBorders = showBorders; }

	void setElevationRaster( const std::string & elevationRaster );
	const std::string & getElevationRaster() const;
	void setCellResolution( float cellResolution);
	float getCellResolution() const;
	void setElevationExaggeration( float elevationExaggeration);
	float getElevationExaggeration() const;

	void setOffset( const Engine::Point3D<float> & offset);
	const Engine::Point3D<float> & getOffset() const;

	void setLOD( int lod );
	int getLOD() const;

	bool hasElevationRaster() const;
    const bool & showValues() const { return _showValues; }
    const bool & showBorders() const { return _showBorders; }
}; // class RasterConfiguration

} // namespace GUI

#endif // __RasterConfiguration_hxx__

