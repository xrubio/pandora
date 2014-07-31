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
#include <cstring>
#include <Logger.hxx>
#include <GeneralState.hxx>

namespace Test
{

TestAgent::TestAgent( const std::string & id , bool moveToDownLeft ) : Agent(id), _moveToDownLeft(moveToDownLeft)
{
}

TestAgent::~TestAgent()
{
}

void TestAgent::updateState()	
{
	std::stringstream logName;
	logName << "agents_" << _world->getId() << "_" << getId();

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
	log_DEBUG(logName.str(), "Agent: " << this << " will move to: " << newPosition);

	log_DEBUG(logName.str(), "vector int - with size: " << _vectorInt.size());
	std::stringstream intVector;
	for(int i=0; i<_vectorInt.size(); i++)
	{
		intVector << "[" << i << "] = " << _vectorInt.at(i) << ", ";
	}
	intVector << std::endl;
	log_DEBUG(logName.str(), intVector.str());
	
	log_DEBUG(logName.str(), "vector float - with size: " << _vectorFloat.size());
	std::stringstream floatVector;
	for(int i=0; i<_vectorFloat.size(); i++)
	{
		floatVector << "[" << i << "] = " << _vectorFloat.at(i) << ", ";
	}
	floatVector << std::endl;
	log_DEBUG(logName.str(), floatVector.str());

	if(_world->checkPosition(newPosition))
	{
	  setPosition(newPosition);
	}
}

std::vector<int> & TestAgent::getVectorInt()
{
	return _vectorInt;
}

std::vector<float> & TestAgent::getVectorFloat()
{
	return _vectorFloat;
}

} // namespace Test

