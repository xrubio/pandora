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

#include <Simulation.hxx>
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
	if(_simulation.getNumTasks()==1)
	{
		assert(_agents.size()==2);
		assert(_overlapAgents.size()==0);
		return;
	}
	if(_simulation.getId()==0)
	{
		if(_step<32)
		{
			assert(_agents.size()==2);
			assert(_overlapAgents.size()==2);
		}
		else if(_step<36)
		{
			assert(_agents.size()==0);
			assert(_overlapAgents.size()==4);
		}
		else
		{
			assert(_agents.size()==0);
			assert(_overlapAgents.size()==0);
		}
		return;
	}
	if(_simulation.getId()==1)
	{
		if(_step<28)
		{
			assert(_agents.size()==1);
			assert(_overlapAgents.size()==1);
		}
		else if(_step<36)
		{
			assert(_agents.size()==1);
			assert(_overlapAgents.size()==3);
		}
		else
		{
			assert(_agents.size()==1);
			assert(_overlapAgents.size()==1);
		}
		return;
	}
	if(_simulation.getId()==2)
	{	
		if(_step<28)
		{
			assert(_agents.size()==1);
			assert(_overlapAgents.size()==1);
		}
		else if(_step<36)
		{
			assert(_agents.size()==1);
			assert(_overlapAgents.size()==3);
		}
		else
		{
			assert(_agents.size()==1);
			assert(_overlapAgents.size()==1);
		}
		return;
	}
	if(_simulation.getId()==3)
	{
		if(_step<28)
		{
			assert(_agents.size()==0);
			assert(_overlapAgents.size()==0);
		}
		else if(_step<32)
		{
			assert(_agents.size()==0);
			assert(_overlapAgents.size()==4);
		}
		else
		{
			assert(_agents.size()==2);
			assert(_overlapAgents.size()==2);
		}
	}
}

void TestWorld::createAgents()
{
	if(_simulation.getId()==0)
	{
		TestAgent * agentVertical1 = new TestAgent("TestAgent_0", false);
		Engine::Point2D<int> pos(31,0);
		agentVertical1->setPosition(pos+_boundaries._origin);
		addAgent(agentVertical1);

		TestAgent * agentHorizontal1 = new TestAgent("TestAgent_2", true);
		Engine::Point2D<int> pos2(0,31);
		agentHorizontal1->setPosition(pos2+_boundaries._origin);
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

