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
#include <Config.hxx>
#include <Exception.hxx>
#include "TestAgent.hxx"
#include <assert.h>
#include <iostream>

namespace Test
{

TestWorld::TestWorld( Engine::Config * config, Engine::Scheduler * scheduler ) : World(config, scheduler, false)
{
}

TestWorld::~TestWorld()
{
}

void TestWorld::stepEnvironment()
{

	if(getNumTasks()==1)
	{
		Engine::Agent * agent = getAgent("TestAgent_0");
		Engine::Agent * agent2 = getAgent("TestAgent_1");

		// euclidean distance between position (31,31) and (25,25) is sqrt (6*6+6*6) = 8.48528137
		float euclideanDistance = sqrt(6*6+6*6);

		assert(countNeighbours(agent, 7, "TestAgent")==0);
		assert(countNeighbours(agent, 8, "TestAgent")==0);
		assert(countNeighbours(agent, euclideanDistance, "TestAgent")==1);
		assert(countNeighbours(agent, 9, "TestAgent")==1);

		Engine::AgentsVector neighbors = getNeighbours(agent, euclideanDistance , "TestAgent");
		Engine::Agent * neighbor = neighbors.at(0).get();
		assert(neighbors.size()==1);
		assert(neighbor->getId().compare("TestAgent_1")==0);

		// with 1 single task you should be able to see the entire set of agents
		assert(countNeighbours(agent,1000, "TestAgent")==2);
		assert(countNeighbours(agent,1000)==500);
		return;
	}
	if(getId()==0)
	{
		Engine::Agent * agent = getAgent("TestAgent_0");
		std::cout << "dist 7: " << countNeighbours(agent,7, "TestAgent") << " dist 8: " << countNeighbours(agent,8, "TestAgent") << " 9: " << countNeighbours(agent, 9, "TestAgent") << " all: " << countNeighbours(agent, 1000, "TestAgent") << " --" <<  countNeighbours(agent, 1000) << std::endl;
		assert(countNeighbours(agent,8, "TestAgent")==0);
		assert(countNeighbours(agent,9, "TestAgent")==3);
	}
	
}

void TestWorld::createAgents()
{
	if(getId()==0)
	{
		TestAgent * agent = new TestAgent("TestAgent_0");
		addAgent(agent);
		agent->setPosition(Engine::Point2D<int>(63,63));		
		
		TestAgent * agent2 = new TestAgent("TestAgent_1");
		addAgent(agent2);
		agent2->setPosition(Engine::Point2D<int>(57,57));
		
		TestAgent * agent3 = new TestAgent("TestAgent_2");
		addAgent(agent3);
		agent3->setPosition(Engine::Point2D<int>(0,0));		

		// we will create several agents in order to test whether selection by type is working
		for(int i=2; i<500; i++)
		{	
			std::ostringstream oss;
			oss << "foo_" << i;
			TestAgent * agentN = new TestAgent(oss.str());
			addAgent(agentN);
			agentN->setRandomPosition();
		}
		return;
	}
	if(getId()==1)
	{
		TestAgent * agent = new TestAgent("TestAgent_3");
		addAgent(agent);
		agent->setPosition(Engine::Point2D<int>(69,57));		
		
		TestAgent * agent2 = new TestAgent("TestAgent_4");
		addAgent(agent2);
		agent2->setPosition(Engine::Point2D<int>(127,0));		

		return;
	}
	if(getId()==3)
	{
		TestAgent * agent = new TestAgent("TestAgent_5");
		addAgent(agent);
		agent->setPosition(Engine::Point2D<int>(69,69));		
		
		TestAgent * agent2 = new TestAgent("TestAgent_6");
		addAgent(agent2);
		agent2->setPosition(Engine::Point2D<int>(127,128));		
	}


}

} // namespace Test 

