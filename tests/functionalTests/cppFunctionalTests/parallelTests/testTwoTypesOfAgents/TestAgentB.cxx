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

#include <TestAgentB.hxx>
#include <World.hxx>
#include <Point2D.hxx>

namespace Test
{

TestAgentB::TestAgentB( const std::string & id ) : Agent(id), _testValueB(5)
{
}

TestAgentB::~TestAgentB()
{
}

void TestAgentB::move()
{
	Engine::Point2D<int> newPosition = _position;
	newPosition._x++; // += 2;
	
	if(_world->checkPosition(newPosition))
	{
	  	setPosition(newPosition);
		return;
	}
}

void TestAgentB::updateState()
{
	move();
}


const int & TestAgentB::getTestValueB()
{
	return _testValueB;
}
} // namespace Test

