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

#ifndef __SumDataPlot_hxx__
#define __SumDataPlot_hxx__

#include <DataPlot.hxx>

#include <string>
class QWidget;

namespace GUI
{

class SumDataPlot : public DataPlot
{ 
public:
	SumDataPlot( QWidget * parent );
	virtual ~SumDataPlot();
	void calculateValues( const std::string & type, const std::string & state );
};

} // namespace GUI

#endif // __SumDataPlot_hxx__

