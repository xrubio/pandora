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

#include "TestWorld.hxx"
#include <Exception.hxx>
#include <Config.hxx>

#include <iostream>
#include <cstdlib>

// this test checks if agents can live in the same cell without problems
// two agents are created, in a diagonal (from 0,0 to 63,63).
// Each agent moves one space each turn until they finish in the opposite corner of the map
// In each time step the position of both agents is checked
int main(int argc, char *argv[])
{
	try
	{
		Test::TestWorld world(new Engine::Config(Engine::Size<int>(64,64), 64), world.useSpacePartition(1));
		world.initialize(argc, argv);
		world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
		return -1;
	}
	return 0;
}

