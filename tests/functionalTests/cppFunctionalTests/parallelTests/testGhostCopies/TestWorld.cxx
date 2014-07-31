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
	if(getNumTasks()==1)
	{
		assert(_agents.size()==2);
		assert(_testScheduler->getNumberOfOverlapAgents()==0);
		return;
	}
	if(getId()==0)
	{
		if(_step<32)
		{
			assert(_agents.size()==2);
			assert(_testScheduler->getNumberOfOverlapAgents()==2);
		}
		else if(_step<36)
		{
			assert(_agents.size()==0);
			assert(_testScheduler->getNumberOfOverlapAgents()==4);
		}
		else
		{
			assert(_agents.size()==0);
			assert(_testScheduler->getNumberOfOverlapAgents()==0);
		}
		return;
	}
	if(getId()==1)
	{
		if(_step<28)
		{
			assert(_agents.size()==1);
			assert(_testScheduler->getNumberOfOverlapAgents()==1);
		}
		else if(_step<36)
		{
			assert(_agents.size()==1);
			assert(_testScheduler->getNumberOfOverlapAgents()==3);
		}
		else
		{
			assert(_agents.size()==1);
			assert(_testScheduler->getNumberOfOverlapAgents()==1);
		}
		return;
	}
	if(getId()==2)
	{	
		if(_step<28)
		{
			assert(_agents.size()==1);
			assert(_testScheduler->getNumberOfOverlapAgents()==1);
		}
		else if(_step<36)
		{
			assert(_agents.size()==1);
			assert(_testScheduler->getNumberOfOverlapAgents()==3);
		}
		else
		{
			assert(_agents.size()==1);
			assert(_testScheduler->getNumberOfOverlapAgents()==1);
		}
		return;
	}
	if(getId()==3)
	{
		if(_step<28)
		{
			assert(_agents.size()==0);
			assert(_testScheduler->getNumberOfOverlapAgents()==0);
		}
		else if(_step<32)
		{
			assert(_agents.size()==0);
			assert(_testScheduler->getNumberOfOverlapAgents()==4);
		}
		else
		{
			assert(_agents.size()==2);
			assert(_testScheduler->getNumberOfOverlapAgents()==2);
		}
	}
}

void TestWorld::createAgents()
{
	if(getId()==0)
	{
		TestAgent * agentVertical1 = new TestAgent("TestAgent_0", false);
		Engine::Point2D<int> pos(31,0);
		addAgent(agentVertical1);
		agentVertical1->setPosition(pos+getBoundaries()._origin);

		TestAgent * agentHorizontal1 = new TestAgent("TestAgent_2", true);
		Engine::Point2D<int> pos2(0,31);
		addAgent(agentHorizontal1);
		agentHorizontal1->setPosition(pos2+getBoundaries()._origin);
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
	}
}

} // namespace Test 

