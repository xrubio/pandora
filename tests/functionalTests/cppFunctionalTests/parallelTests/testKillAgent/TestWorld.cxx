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
		assert(_agents.size()==0);
		assert(_testScheduler->getNumberOfOverlapAgents()==0);
		return;
	}

	if(getId()==3)
	{
		if(_step<31)
		{	
			assert(_agents.size()==1);
			assert(_testScheduler->getNumberOfOverlapAgents()==0);
		}
		else
		{
			assert(_agents.size()==0);
			assert(_testScheduler->getNumberOfOverlapAgents()==0);
		}
		return;
	}
	if(_step<28)
	{	
		assert(_agents.size()==0);
		assert(_testScheduler->getNumberOfOverlapAgents()==0);
	}
	else if(_step<31)
	{	
		assert(_agents.size()==0);
		assert(_testScheduler->getNumberOfOverlapAgents()==1);
	}
	else
	{
		assert(_agents.size()==0);
		assert(_testScheduler->getNumberOfOverlapAgents()==0);
	}
}

void TestWorld::createAgents()
{
	if(getId()==3)
	{
		TestAgent * agent = new TestAgent("TestAgent_0");
		addAgent(agent);
		agent->setPosition(Engine::Point2D<int>(63,63));
	}
}

} // namespace Test 

