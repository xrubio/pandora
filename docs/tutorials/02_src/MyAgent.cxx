
#include "MyAgent.hxx"
#include <iostream>

#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>

namespace Tutorial
{

MyAgent::MyAgent( const std::string & id ) : Agent(id), _gatheredResources(0)
{
}

MyAgent::~MyAgent()
{
}

void MyAgent::updateState()
{
	std::cout << "updating state for agent: " << this << std::endl;
	Engine::Point2D<int> newPosition = _position;
	newPosition._x += Engine::GeneralState::statistics().getUniformDistValue(-1,1);
	newPosition._y += Engine::GeneralState::statistics().getUniformDistValue(-1,1);

	if(_world->checkPosition(newPosition))
	{
		setPosition(newPosition);
		_gatheredResources += _world->getValue("resources", _position);
		_world->setValue("resources", _position, 0);
	}
}

void MyAgent::serialize()
{
	serializeAttribute("resources", _gatheredResources);
}

void MyAgent::registerAttributes()
{
	registerIntAttribute("resources");
}


} // namespace Tutorial

