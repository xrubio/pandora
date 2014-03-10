
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

#include <Simulation.hxx>
#include <mpi.h>
#include <iostream>
#include <Exceptions.hxx>
#include <cmath>
#include <sstream>

namespace Engine
{


Simulation::Simulation( const Point2D<int> & size, const int & numSteps, const int & serializerResolution ) : _size(size), _numSteps(numSteps), _serializerResolution(serializerResolution)
{
}

Simulation::~Simulation()
{
}

const Point2D<int> & Simulation::getSize() const
{
	return _size;
}

const int & Simulation::getNumSteps() const
{
	return _numSteps;
}

const int & Simulation::getSerializerResolution() const
{
	return _serializerResolution;
}

} // namespace Engine

