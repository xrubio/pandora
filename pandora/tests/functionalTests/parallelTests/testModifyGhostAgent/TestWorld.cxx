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

#include <TestWorld.hxx>

#include <Point2D.hxx>
#include <Exceptions.hxx>

#include <TestAgent.hxx>

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
}

void TestWorld::stepAgents()
{
	// it is important to think that step agents is executed before agents, so each step must check the actions in the former step
	if(getCurrentStep()!=1 || getId()!=3)
	{
		return;
	}

	for(AgentsList::iterator it=_agents.begin(); it!=_agents.end(); it++)
	{
		TestAgent * agent = (TestAgent*)(*it);
		assert(agent->getFlag()==true);
	}
}

void TestWorld::createAgents()
{
	if(_simulation.getId()==0)
	{
		TestAgent * agent0 = new TestAgent("TestAgent_0");
		Engine::Point2D<int> pos(31,31);
		agent0->setPosition(pos);
		addAgent(agent0);
		return;
	}
	if(_simulation.getId()==3)
	{
		TestAgent * agent1 = new TestAgent("TestAgent_1");
		Engine::Point2D<int> pos(33,33);
		agent1->setPosition(pos);
		addAgent(agent1);
		return;
	}
}

} // namespace Test 

