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

#include <TestAgentA.hxx>
#include <World.hxx>
#include <Point2D.hxx>

namespace Test
{

TestAgentA::TestAgentA( const std::string & id ) : Agent(id), _testValueA(10)
{
	_testId = "test agent A";
}

TestAgentA::~TestAgentA()
{
}

void TestAgentA::move()
{
	Engine::Point2D<int> newPosition = _position;
	newPosition._x++;

	if(_world->checkPosition(newPosition))
	{
	  setPosition(newPosition);
	}
}

void TestAgentA::updateState()
{
	move();
}

const std::string & TestAgentA::getTestId()
{
	return _testId;
}

const int & TestAgentA::getTestValueA()
{
	return _testValueA;
}

} // namespace Test

