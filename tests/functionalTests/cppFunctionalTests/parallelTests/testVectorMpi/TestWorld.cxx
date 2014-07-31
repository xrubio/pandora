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

#include <Exception.hxx>
#include <TestAgent.hxx>
#include <assert.h>
#include <iostream>
#include <Config.hxx>

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
	TestAgent * agent = (TestAgent*)getAgent("TestAgent_0");
	if(agent)
	{
		std::vector<int> & vectorInt = agent->getVectorInt();
		std::vector<float> & vectorFloat = agent->getVectorFloat();
		for(int i=0; i<200; i++)
		{	
			assert(vectorInt[i] == i);
		}
		for(int i=0; i<100; i++)
		{
			assert(vectorFloat[i] == i+0.5f);
		}
	}

	agent = (TestAgent*)getAgent("test_1");
	if(agent)
	{
		std::vector<int> & vectorInt = agent->getVectorInt();
		std::vector<float> & vectorFloat = agent->getVectorFloat();
		for(int i=0; i<300; i++)
		{	
			assert(vectorInt[i] == 2*i);
		}
		for(int i=0; i<50; i++)
		{
			assert(vectorFloat[i]-(2*i+0.3f)<=0.001f);
		}
	}
}

void TestWorld::createAgents()
{
	if(getId()==0)
	{
		TestAgent * agent = new TestAgent("TestAgent_0", true);
		addAgent(agent);
		agent->setPosition(Engine::Point2D<int>(00,0));
		agent->getVectorInt().resize(200);
		agent->getVectorFloat().resize(100);
		for( int i=0; i<200; i++)
		{
			agent->getVectorInt()[i] = i;
		}
		for(int i=0; i<100; i++)
		{
			agent->getVectorFloat()[i] = i+0.5f;
		}		
	}
	if(getId()==getNumTasks()-1)
	{
		TestAgent * agent = new TestAgent("test_1", false);
		addAgent(agent);
		Engine::Point2D<int> pos(getConfig().getSize()._width-1, getConfig().getSize()._height-1);
		agent->setPosition(pos);
		agent->getVectorInt().resize(300);
		agent->getVectorFloat().resize(50);
		for( int i=0; i<300; i++)
		{
			agent->getVectorInt()[i] = 2*i;
		}
		for(int i=0; i<50; i++)
		{
			agent->getVectorFloat()[i] = 2*i+0.3f;
		}		
		
		return;
	}
}

} // namespace Test 

