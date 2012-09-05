
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


Simulation::Simulation( const int & size, const int & numSteps ) : _id(-1), _numTasks(1), _size(size), _numSteps(numSteps), _localRasterSize(0)
{
}

Simulation::~Simulation()
{
}

void Simulation::init()
{
	MPI_Comm_size(MPI_COMM_WORLD, &_numTasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&_id);
	std::cout << "simulation: " << _id << " of: " << _numTasks << " initialized" << std::endl;
	
	_localRasterSize = _size/sqrt(_numTasks);
	if(_localRasterSize%2!=0)
	{
		// TODO fix when matrix not squared
		std::stringstream oss;
		oss << "Simulation::init - local raster size: " << _localRasterSize << " must be divisible by 2";
		throw Exception(oss.str());
	}
	
}
	
const int & Simulation::getId() const
{
	return _id;
}

const int & Simulation::getNumTasks() const
{
	return _numTasks;
}

const int & Simulation::getSize() const
{
	return _size;
}

const int & Simulation::getNumSteps() const
{
	return _numSteps;
}

const int & Simulation::getLocalRasterSize() const
{
	return _localRasterSize;
}

} // namespace Engine

