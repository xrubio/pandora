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

#ifndef __DefaultColorSelector_hxx__
#define __DefaultColorSelector_hxx__

#include <vector>
#include <ColorSelector.hxx>

namespace GUI
{

// default selector, the choice is made based on a scale used in Digital Elevation Models
class DefaultColorSelector : public ColorSelector
{
	// the numbers that function as "breakers" between intervals	
	std::vector<int> _intervals;
	// colors of the ramp
	std::vector<QColor> _colors;

public:
	DefaultColorSelector( const int & minValue, const int & maxValue );
	DefaultColorSelector( const DefaultColorSelector & prototype );
	virtual ~DefaultColorSelector();

	void addBreak(  const int & value );
	const QColor & getColor( const int & value ) const;

	unsigned int getNumIntervals() const;
	const int & getMinValue() const;
	const int & getMaxValue() const;
	const int & getInterval( const int & index ) const;
	const int & getBreak( const int & index ) const;
	void setColor( const QColor & color, const int & index );
	ColorSelector * copy() const;
};

} // namespace GUI

#endif // __DefaultColorSelector_hxx__

