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
#include <Exception.hxx>
#include <Statistics.hxx>
#include <cstring>
#include <Logger.hxx>
#include <GeneralState.hxx>

namespace Test
{

TestAgent::TestAgent( const std::string & id , bool moveToDownLeft ) : Agent(id), _message("") , _moveToDownLeft(moveToDownLeft)
{
}

TestAgent::~TestAgent()
{
}

void TestAgent::updateState()	
{	
	std::stringstream logName;
	logName << "agents_" << _world->getId() << "_" << getId();
	log_DEBUG(logName.str(), "Agent: " << this << " has message: " << _message);

	std::stringstream newMessage;
	if(_moveToDownLeft)
	{
		newMessage << "string_" << 1+_world->getCurrentStep();
	}
	else
	{
		newMessage << "string_" << 198-_world->getCurrentStep();
	}
	_message = newMessage.str();
	
	Engine::Point2D<int> newPosition = _position;
	if(_moveToDownLeft)
	{
		newPosition._x++;
		newPosition._y++;
	}
	else
	{
		newPosition._x--;
		newPosition._y--;
	}

	if(_world->checkPosition(newPosition))
	{
	  setPosition(newPosition);
	}
}

void TestAgent::setMessage( const std::string & message )
{
	_message = message;
}

const std::string & TestAgent::getMessage()
{
	return _message;
}

} // namespace Test

