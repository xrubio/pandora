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

#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <Exception.hxx>

#include "TestAgentA.hxx"
#include "TestAgentB.hxx"
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
	if(getId()==2 || getId()==3)
	{
		return;
	}
	if(getNumTasks()==1)
	{
		assert(_agents.size()==2);
	}
	else if(getNumTasks()==4)
	{
		if(getId()==0)
		{
			if(_step<32)
			{
				assert(_agents.size()==2);
				assert(_testScheduler->getNumberOfOverlapAgents()==0);
			}
			else if(_step<36)
			{
				assert(_agents.size()==0);
				assert(_testScheduler->getNumberOfOverlapAgents()==2);
			}
			else
			{
				assert(_agents.size()==0);
				assert(_testScheduler->getNumberOfOverlapAgents()==0);
			}
		}
		else if(getId()==1)
		{
			if(_step<28)				
			{
				assert(_agents.size()==0);
				assert(_testScheduler->getNumberOfOverlapAgents()==0);
			}
			else if(_step<32)
			{
				assert(_agents.size()==0);
				assert(_testScheduler->getNumberOfOverlapAgents()==2);
			}
			else
			{
				assert(_agents.size()==2);
				assert(_testScheduler->getNumberOfOverlapAgents()==0);
			}
		}
	}
	if(_agents.size()!=0)
	{
		TestAgentA * agentA = (TestAgentA*)getAgent("TestAgentA_1");
		assert(agentA->isType("TestAgentA")==true);
		assert(agentA->isType("TestAgentB")==false);
		assert(agentA->getTestValueA()==10);
		assert(agentA->getTestId().compare("test agent A")==0);

		TestAgentB * agentB = (TestAgentB*)getAgent("TestAgentB_1");
		assert(agentB->isType("TestAgentA")==false);
		assert(agentB->isType("TestAgentB")==true);	
		assert(agentB->getTestValueB()==5);
	}
}

void TestWorld::createAgents()
{
	if(getId()==0)
	{
		TestAgentA * agent = new TestAgentA("TestAgentA_1");
		addAgent(agent);
		agent->setPosition(Engine::Point2D<int>(0,10));		
		
		TestAgentB * agent2 = new TestAgentB("TestAgentB_1");
		addAgent(agent2);
		agent2->setPosition(Engine::Point2D<int>(0,20));
	}
}

} // namespace Test 

