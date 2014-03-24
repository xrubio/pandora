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
	if(_simulation.getNumTasks()==1)
	{
		return;
	}

	// more than one task, agents turn the state of each other
	for(AgentsList::iterator it=_agents.begin(); it!=_agents.end(); it++)
	{
		TestAgent * agent = (TestAgent*)(*it);
		assert(agent->isEvenTurn()==lastStepWasEven);
	}
	for(AgentsList::iterator it=_overlapAgents.begin(); it!=_overlapAgents.end(); it++)
	{
		TestAgent * agent = (TestAgent*)(*it);
		assert(agent->isEvenTurn()==lastStepWasEven);
	}
}

void TestWorld::createAgents()
{
	if(_simulation.getId()==0)
	{
		TestAgent * agentVertical1 = new TestAgent("TestAgent_0", false);
		Engine::Point2D<int> pos(31,0);
		agentVertical1->setPosition(pos);
		addAgent(agentVertical1);
		TestAgent * agentHorizontal1 = new TestAgent("TestAgent_2", true);
		Engine::Point2D<int> pos2(0,31);
		agentHorizontal1->setPosition(pos2);
		addAgent(agentHorizontal1);
		return;
	}
	if(_simulation.getId()==1)
	{
		TestAgent * agentVertical2 = new TestAgent("TestAgent_1", false);
		Engine::Point2D<int> pos(0,0);
		agentVertical2->setPosition(pos+_boundaries._origin);
		addAgent(agentVertical2);
		return;

	}
	if(_simulation.getId()==2)
	{
		TestAgent * agentHorizontal2 = new TestAgent("TestAgent_3", true);
		Engine::Point2D<int> pos(0,0);
		agentHorizontal2->setPosition(pos+_boundaries._origin);
		addAgent(agentHorizontal2);
		return;
	}
}

} // namespace Test 

