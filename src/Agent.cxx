
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

#include <Agent.hxx>

#include <Exception.hxx>
#include <World.hxx>

#include <Serializer.hxx>

#include <Action.hxx>
#include <Logger.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>

namespace Engine
{

Agent::Agent( const std::string & id ) : _id(id), _exists(true), _position(-1,-1), _world(0)
{
	_stringAttributes.push_back("id");
	_intAttributes.push_back("x");
	_intAttributes.push_back("y");
}

Agent::~Agent()
{
}

void Agent::setWorld( World * world )
{
	_world = world;
}

World * Agent::getWorld()
{
	return _world;
}

const World * Agent::getWorld() const
{
	return _world;
}

World & Agent::getWorldRef()
{
	if(!_world)
	{
		std::stringstream oss;
		oss << "Agent::getWorldRef - accessing world reference without defining Agent's World";
		throw Exception(oss.str());
	}
	return *_world;
}

void Agent::setPosition( const Point2D<int> & position )
{
	_position = position;
}

const Point2D<int> & Agent::getPosition() const
{
	return _position;
}

void Agent::serializeAttribute( const std::string & name, const int & value )
{
	_world->addIntAttribute(getType(), name,value);
}

void Agent::serializeAttribute( const std::string & name, const float & value )
{
	_world->addFloatAttribute(getType(), name,value);
}

void Agent::serializeAttribute( const std::string & name, const std::string & value)
{
	_world->addStringAttribute(getType(), name,value);
}

const std::string & Agent::getId() const
{
	return _id;
}

bool Agent::exists() const
{
	return _exists;
}

void Agent::setExists(bool exists)
{
	_exists = exists;
}

std::ostream& Agent::print(std::ostream& os) const {
	os << "id: " << getId() << " pos: " << getPosition() << " exists: " << exists();
	return getWorld() ? os << " at world: " << getWorld()->getId() : os << " without world";
}

void Agent::remove()
{
	_exists = false;
	if(!_world)
	{
		std::stringstream oss;
		oss << "Agent::remove - removing agent without assigned World";
		throw Exception(oss.str());
	}
	_world->removeAgent(this);
}
	
bool Agent::isType( const std::string & type ) const
{
	if(type.compare(getType())==0)
	{
		return true;
	}
	return false;
}

std::string Agent::getType() const
{
	unsigned int typePos = _id.find_first_of("_");
	return _id.substr(0,typePos);
}

void Agent::executeActions()
{
	std::list<Action *>::iterator it = _actions.begin();
	unsigned i = 0;
	while(it!=_actions.end())
	{
		Action * nextAction = *it;
		nextAction->execute((Agent&)(*this));
		it = _actions.erase(it);
		delete nextAction;
		i++;
	}
}
	
void Agent::setRandomPosition()
{
	_position = _world->getRandomPosition();
}

void Agent::changeType( const std::string & type )
{
    std::string oldType = getType();
    size_t startPos = _id.find(oldType);
    _id.replace(startPos, oldType.length(), type);
}

} // namespace Engine

