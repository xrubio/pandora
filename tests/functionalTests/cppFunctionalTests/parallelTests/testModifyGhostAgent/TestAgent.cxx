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

#include <TestAgent.hxx>
#include <World.hxx>
#include <Point2D.hxx>
#include <Logger.hxx>
#include <GeneralState.hxx>
#include <typedefs.hxx>

namespace Test
{

TestAgent::TestAgent( const std::string & id ) : Agent(id), _flag(false)
{
}

TestAgent::~TestAgent()
{
}

void TestAgent::setFlag( const bool & flag )
{
	_flag = flag;
}

bool TestAgent::getFlag()
{
	return _flag;
}

void TestAgent::updateState()
{
	Engine::AgentsVector neighbors = _world->getNeighbours( this, 100);
	if(neighbors.size()==0)
		{
			return;
		}
	TestAgent * agent = (TestAgent*)neighbors.at(0).get();
	agent->setFlag(true);
	return;
}

} // namespace Test

