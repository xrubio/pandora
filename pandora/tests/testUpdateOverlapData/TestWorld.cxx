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

#include "Raster.hxx"
#include "Point2D.hxx"
#include "Exceptions.hxx"

#include "TestAgent.hxx"

#include <assert.h>
#include <iostream>

namespace Test
{

TestWorld::TestWorld( const Engine::Simulation & sim ) : World(sim, 4, true, "data/test.h5")
{
}

TestWorld::~TestWorld()
{
}

void TestWorld::createRasters()
{	
	registerDynamicRaster("test", false);
	getDynamicRaster("test").setInitValues(0, 64, 0);
}

void TestWorld::stepRaster( const std::string & key, Engine::Raster & raster)
{
	for(int i=0; i<_step; i++)
	{
		Engine::Point2D<int> pos(i,i);
		if(_overlapBoundaries.isInside(pos))
		{
			assert(getValue("test", pos)==i);
		}
	}
}
void TestWorld::createAgents()
{
	if(_simulation.getId()==0)
	{
		TestAgent * agent = new TestAgent("TestAgent_0");
		agent->setPosition(Engine::Point2D<int>(0,0));
		addAgent(agent);
		return;
	}
}

} // namespace Test 

