
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

namespace Engine
{

// this class encapulated the parameters defined to execute the simulation
class Simulation
{
	int _id;
	int _numTasks;
	int _size;
	int _numSteps;
	int _localRasterSize;
public:
	Simulation( const int & size, const int & numSteps );
	virtual ~Simulation();
	void init();

	const int & getId() const;
	const int & getNumTasks() const;
	const int & getSize() const;
	const int & getNumSteps() const;
	const int & getLocalRasterSize() const;
}; 

} // namespace Engine

#endif // __Simulation_hxx__

