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

#include <TestWorld.hxx>

#include <Exceptions.hxx>
#include <TestAgent.hxx>
#include <assert.h>
#include <sstream>

namespace Test
{

TestWorld::TestWorld( Engine::Simulation & simulation ) : World( simulation, 1, true, "data/test.h5")
{
}

TestWorld::~TestWorld()
{
}

void TestWorld::stepAgents()
{
	Engine::Agent * baseAgent = getAgent("TestAgent_0");
	if(baseAgent)
	{
		Engine::Point2D<int> position(getCurrentStep(), getCurrentStep());
		assert(position._x==baseAgent->getPosition()._x);
		assert(position._y==baseAgent->getPosition()._y);
	}
	
	baseAgent = getAgent("TestAgent_1");
	if(baseAgent)
	{
		Engine::Point2D<int> position(_simulation.getSize()-1-getCurrentStep(), _simulation.getSize()-1-getCurrentStep());
		assert(position._x==baseAgent->getPosition()._x);
		assert(position._y==baseAgent->getPosition()._y);
	}
}

void TestWorld::createRasters()
{
}

void TestWorld::createAgents()
{
	if(_simulation.getId()==0)
	{
		TestAgent * agent = new TestAgent("TestAgent_0", true);		
		agent->setPosition(Engine::Point2D<int>(0,0));
		addAgent(agent);
	}
	if(_simulation.getId()==_simulation.getNumTasks()-1)
	{
		TestAgent * agent = new TestAgent("TestAgent_1", false);	
		Engine::Point2D<int> pos(_simulation.getSize()-1, _simulation.getSize()-1);
		agent->setPosition(pos);
		addAgent(agent);
	}
}

} // namespace Test 

