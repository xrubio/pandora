
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

#ifndef __Simulation_hxx__
#define __Simulation_hxx__

#include <Size.hxx>

namespace Engine
{

// this class encapulated the parameters defined to execute the simulation
class Simulation
{
	Size<int> _size;
	int _numSteps;

	// this attribute defines how many time steps are executed before serializing one.
	int _serializerResolution;
public:
	Simulation( const Size<int> & size, const int & numSteps, const int & serializerResolution = 1 );
	virtual ~Simulation();

	const Size<int> & getSize() const;
	const int & getNumSteps() const;
	const int & getSerializerResolution() const;
}; 

} // namespace Engine

#endif // __Simulation_hxx__

