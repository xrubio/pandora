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

#include <Point2D.hxx>
#include <Exception.hxx>
#include <Config.hxx>
#include "TestAgent.hxx"
#include <typedefs.hxx>
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
	// it is important to think that step agents is executed before agents, so each step must check the actions in the former step
	if(getCurrentStep()!=1)
	{
		return;
	}

	for(Engine::AgentsList::iterator it=_agents.begin(); it!=_agents.end(); it++)
	{
		TestAgent * agent = (TestAgent*)(*it).get();
		assert(agent->getFlag()==true);
	}
}

void TestWorld::createAgents()
{
	if(getId()==0)
	{
		TestAgent * agent0 = new TestAgent("TestAgent_0");
		Engine::Point2D<int> pos(31,31);
		addAgent(agent0);
		agent0->setPosition(pos);		
		return;
	}
	if(getId()==3)
	{
		TestAgent * agent1 = new TestAgent("TestAgent_1");
		Engine::Point2D<int> pos(33,33);
		addAgent(agent1);
		agent1->setPosition(pos);
	}
}

} // namespace Test 

