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

#include "TestAgent.hxx"
#include <Config.hxx>
#include <assert.h>
#include <iostream>
#include <typedefs.hxx>
#include <SpacePartition.hxx>

namespace Test
{

TestWorld::TestWorld( Engine::Config * config, Engine::Scheduler * scheduler ) : World(config, scheduler, true), _testScheduler((Engine::SpacePartition *)scheduler)
{
}

TestWorld::~TestWorld()
{
}

void TestWorld::stepEnvironment()
{
	// it is important to think that step agents is executed before agents, so each step must check the actions in the former step
	if(getCurrentStep()==0)
	{
		return;
	}

	int lastStep = getCurrentStep() - 1;
	bool lastStepWasEven = false;
	if(lastStep%2==0)
	{
		lastStepWasEven = true;
	}	
	
	// only one task, agents does not have any neighbor so always is even until step 32 (where they cross the paths)
	if(getNumTasks()==1)
	{
		return;
	}

	// more than one task, agents turn the state of each other
	for(Engine::AgentsList::iterator it=_agents.begin(); it!=_agents.end(); it++)
	{
		TestAgent * agent = (TestAgent*)(it)->get();
		assert(agent->isEvenTurn()==lastStepWasEven);
	}
	for(Engine::AgentsList::const_iterator it=_testScheduler->beginOverlapAgents(); it!=_testScheduler->endOverlapAgents(); it++)
	{
		const TestAgent * agent = (TestAgent*)(it)->get();
		assert(agent->isEvenTurn()==lastStepWasEven);
	}
}

void TestWorld::createAgents()
{
	if(getId()==0)
	{
		TestAgent * agentVertical1 = new TestAgent("TestAgent_0", false);
		Engine::Point2D<int> pos(31,0);
		addAgent(agentVertical1);
		agentVertical1->setPosition(pos);

		TestAgent * agentHorizontal1 = new TestAgent("TestAgent_2", true);
		Engine::Point2D<int> pos2(0,31);
		addAgent(agentHorizontal1);
		agentHorizontal1->setPosition(pos2);
		return;
	}
	if(getId()==1)
	{
		TestAgent * agentVertical2 = new TestAgent("TestAgent_1", false);
		Engine::Point2D<int> pos(32,0);
		addAgent(agentVertical2);
		agentVertical2->setPosition(pos);
		return;

	}
	if(getId()==2)
	{
		TestAgent * agentHorizontal2 = new TestAgent("TestAgent_3", true);
		Engine::Point2D<int> pos(0,32);
		addAgent(agentHorizontal2);
		agentHorizontal2->setPosition(pos);
		return;
	}
}

} // namespace Test 

