/*
 * Copyright (c) 2012
 * COMPUTER APPLICATION IN SCIENCE & ENGINEERING
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
#include "TestAgent.hxx"
#include <assert.h>
#include <sstream>
#include <Config.hxx>
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
    Engine::Agent * baseAgent = getAgent("TestAgent_0");
    if(baseAgent)
    {
        Engine::Point2D<int> position(getCurrentStep(), getCurrentStep());
        assert(position==baseAgent->getPosition());
    }

	baseAgent = getAgent("TestAgent_1");
	if(baseAgent)
	{
		Engine::Point2D<int> position(getConfig().getSize()._width-1-getCurrentStep(), getConfig().getSize()._height-1-getCurrentStep());
		assert(position._x==baseAgent->getPosition()._x);
		assert(position._y==baseAgent->getPosition()._y);
	}
}

void TestWorld::createAgents()
{
	if(getId()==0)
	{
		TestAgent * agent = new TestAgent("TestAgent_0", true);		
		addAgent(agent);
		agent->setPosition(Engine::Point2D<int>(0,0));		
	}
	if(getId()==getNumTasks()-1)
	{
		TestAgent * agent = new TestAgent("TestAgent_1", false);	
		Engine::Point2D<int> pos(getConfig().getSize()._width-1, getConfig().getSize()._height-1);
		addAgent(agent);
		agent->setPosition(pos);		
	}
}

} // namespace Test 

