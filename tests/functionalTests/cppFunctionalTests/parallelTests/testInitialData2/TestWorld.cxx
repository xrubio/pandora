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

namespace Test
{

TestWorld::TestWorld( Engine::Config * config, Engine::Scheduler * scheduler ) : World(config, scheduler, true)
{
}

TestWorld::~TestWorld()
{
}

void TestWorld::stepEnvironment()
{
	for( auto index : getBoundaries())
	{
		assert(getValue("testX", index)==index._x);
		assert(getValue("testY", index)==index._y);
	}
}

void TestWorld::createRasters()
{	
	registerDynamicRaster("testX", false);
	registerDynamicRaster("testY", false);
	getDynamicRaster("testX").setInitValues(0,getConfig().getSize()._width, 0);
	getDynamicRaster("testY").setInitValues(0,getConfig().getSize()._height, 0);

	for( auto index : getBoundaries())
	{
	  setMaxValue("testX", index, index._x);
	  setMaxValue("testY", index, index._y);
	}
	updateRasterToMaxValues("testX");
	updateRasterToMaxValues("testY");
}

} // namespace Test 

