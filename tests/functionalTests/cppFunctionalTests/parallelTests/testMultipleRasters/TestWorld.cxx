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

#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <Exception.hxx>
#include <Config.hxx>

#include <assert.h>
#include <iostream>
#include <string>
#include <sstream>
#include <limits>

namespace Test
{

TestWorld::TestWorld( Engine::Config * config, Engine::Scheduler * scheduler, const int & numRasters ) : World(config, scheduler, true), _numRasters(numRasters) 
{
}

TestWorld::~TestWorld()
{
}

void TestWorld::createRasters()
{	
	for(int i=0; i<_numRasters; i++)
	{
		std::stringstream logName;
		logName << "raster_" << i;
		registerDynamicRaster(logName.str(), true, i);
		getDynamicRaster(i).setInitValues(0, std::numeric_limits<int>::max(), i);
	}
}

} // namespace Test 


