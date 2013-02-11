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

}; // class RasterConfiguration

} // namespace GUI

#endif // __RasterConfiguration_hxx__

