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

// this test checks if a modification in a ghost copy of an agent is correctly updated in the real agent
// An agent in 0 world modifies a boolean value of a ghost copy in 3 world; next step a test is made in order to check that the change was updated

int main(int argc, char *argv[])
{
	try
	{	
		Test::TestWorld world(new Engine::Config(Engine::Size<int>(64,64), 2), world.useSpacePartition(4));
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

