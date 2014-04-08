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

#include <Exceptions.hxx>
#include <TestAgent.hxx>
#include <assert.h>
#include <iostream>

namespace Test
{

TestWorld::TestWorld( Engine::Simulation & simulation ) : World( simulation, 9, false, "data/test.h5")
{
}

TestWorld::~TestWorld()
{
}

void TestWorld::stepAgents()
{

	if(_simulation.getNumTasks()==1)
	{
		Engine::Agent * agent = getAgent("TestAgent_0");
		Engine::Agent * agent2 = getAgent("TestAgent_1");

		// euclidean distance between position (31,31) and (25,25) is sqrt (6*6+6*6) = 8.48528137
		float euclideanDistance = sqrt(6*6+6*6);

		assert(countNeighbours(agent, 7, "TestAgent")==0);
		assert(countNeighbours(agent, 8, "TestAgent")==0);
		assert(countNeighbours(agent, euclideanDistance, "TestAgent")==1);
		assert(countNeighbours(agent, 9, "TestAgent")==1);

		Engine::World::AgentsVector neighbors = getNeighbours(agent, euclideanDistance , "TestAgent");
		Engine::Agent * neighbor = *neighbors.begin();
		assert(neighbors.size()==1);
		assert(neighbor->getId().compare("TestAgent_1")==0);

		// with 1 single task you should be able to see the entire set of agents
		assert(countNeighbours(agent,1000, "TestAgent")==2);
		assert(countNeighbours(agent,1000)==500);
		return;
	}
	if(_simulation.getId()==0)
	{
		Engine::Agent * agent = getAgent("TestAgent_0");
		std::cout << "dist 7: " << countNeighbours(agent,7, "TestAgent") << " dist 8: " << countNeighbours(agent,8, "TestAgent") << " 9: " << countNeighbours(agent, 9, "TestAgent") << " all: " << countNeighbours(agent, 1000, "TestAgent") << " --" <<  countNeighbours(agent, 1000) << std::endl;
		assert(countNeighbours(agent,8, "TestAgent")==0);
		assert(countNeighbours(agent,9, "TestAgent")==3);
	}
	
}

void TestWorld::createRasters()
{
}

void TestWorld::createAgents()
{
	if(_simulation.getId()==0)
	{
		TestAgent * agent = new TestAgent("TestAgent_0");
		agent->setPosition(Engine::Point2D<int>(63,63));
		addAgent(agent);
		
		TestAgent * agent2 = new TestAgent("TestAgent_1");
		agent2->setPosition(Engine::Point2D<int>(57,57));
		addAgent(agent2);
		
		TestAgent * agent3 = new TestAgent("TestAgent_2");
		agent3->setPosition(Engine::Point2D<int>(0,0));
		addAgent(agent3);

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
	if(_simulation.getId()==1)
	{
		TestAgent * agent = new TestAgent("TestAgent_3");
		agent->setPosition(Engine::Point2D<int>(69,57));
		addAgent(agent);
		
		TestAgent * agent2 = new TestAgent("TestAgent_4");
		agent2->setPosition(Engine::Point2D<int>(127,0));
		addAgent(agent2);

		return;
	}
	if(_simulation.getId()==3)
	{
		TestAgent * agent = new TestAgent("TestAgent_5");
		agent->setPosition(Engine::Point2D<int>(69,69));
		addAgent(agent);
		
		TestAgent * agent2 = new TestAgent("TestAgent_6");
		agent2->setPosition(Engine::Point2D<int>(127,128));
		addAgent(agent2);

		return;
	}


}

} // namespace Test 

