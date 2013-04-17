
#include "PanicAgent.hxx"

#include <Statistics.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <cmath>

#include "Scenario.hxx"
#include "MoveAction.hxx"

namespace Panic
{

PanicAgent::PanicAgent( const std::string & id ) : Agent(id), _direction(0), _exited(false), _rest(0.0f, 0.0f)
{
	_direction = Engine::GeneralState::statistics().getUniformDistValue(0,359);
}

PanicAgent::~PanicAgent()
{
}

Engine::Point2D<float> PanicAgent::getNextPos( const int & direction, const Engine::Point2D<float> & position )
{
	float radians = float(direction)*2*M_PI/360.0f;
	Engine::Point2D<float> directionVector(std::sin(radians), std::cos(radians));

	Engine::Point2D<float> newPos = Engine::Point2D<float>(position._x+directionVector._x, position._y-directionVector._y);
//	std::cout << "direction: " << directionVector << " for direction: " << direction << " old pos: " << position << " new pos: " << newPos << std::endl;
	return newPos;
}
	
float PanicAgent::getDistToNearestObstacle( const int & direction )
{
	Engine::Point2D<float> newPos = Engine::Point2D<float>(_position._x, _position._y);
	while(1)
	{
		//newPos = getNextPos(_direction, Engine::Point2D<float>(newPos._x+_rest._x, newPos._y+_rest._y));
		newPos = getNextPos(direction, newPos);
		Engine::Point2D<int> newIntPos = Engine::Point2D<int>(std::floor(newPos._x), std::floor(newPos._y));
		// out of position
		if(!_world->checkPosition(newIntPos))
		{	
			if(newIntPos!=_position)
			{
				return newIntPos.distance(_position);
			}
		}
		
		if(_world->getDynamicRaster(eExits).getValue(newIntPos)==1)
		{	
			if(newIntPos!=_position)
			{
				return newIntPos.distance(_position);
			}
		}

		// obstacle found
		if(_world->getDynamicRaster(eObstacles).getValue(newIntPos)==1)
		{	
			if(newIntPos!=_position)
			{
				return newIntPos.distance(_position);
			}
		}
		// there is a person there	
//		if(_world->getAgent(Engine::Point2D<int>(std::floor(newPos._x), std::floor(newPos._y))).size()!=0)
//		{
//			if(newIntPos!=_position)
//			{
//				return newIntPos.distance(_position);
//			}
//		}
	}
}



void PanicAgent::selectActions()
{
	if(_exited)
	{
		return;
	}

	float rangeOfSight = 900.0f;
	// fov in degrees
	int fov = 360;
	int increaseDirection = 5;

	float minValue = std::numeric_limits<float>::max();
	int finalDirection = 0;

	for(float newDirection=_direction-fov/2; newDirection<=_direction+fov/2; newDirection=newDirection+increaseDirection)
	{
		int direction = newDirection;
		if(direction>360)
		{
			direction -= 360;
		}
		else if(direction<0)
		{
			direction += 360;
		}
		float radians = float(direction)*M_PI/180.0f;

		float distToObstacle = getDistToNearestObstacle(direction);

		Engine::Point2D<float> directDirectionVector(_position._x-_exit._x, _position._y-_exit._y);
		float desiredRadians = std::atan2(-directDirectionVector._x, directDirectionVector._y);
		float desiredDegrees = desiredRadians*180.0f/M_PI;
		if(desiredDegrees>360.0f)
		{
			desiredDegrees-= 360.0f;
		}
		else if(desiredDegrees<0.0f)
		{
			desiredDegrees+= 360.0f;
		}
//		std::cout << "pos: " << _position << " to 0,0 rads: " << desiredRadians<< " degrees: " << desiredDegrees<< std::endl;
//		std::cout << "pos: " << _position << " basic dir: " << _direction << " fov: " << fov << " direction to test: " << direction << " dist to obstacle: " << distToObstacle << std::endl;

		float diffRadiansCos = std::cos(desiredRadians)*std::cos(radians) + std::sin(desiredRadians)*std::sin(radians);
		float diffRadians = std::acos(diffRadiansCos);
		float diffDegrees = diffRadians*180.0f/M_PI;

		float value = rangeOfSight*rangeOfSight + distToObstacle*distToObstacle - 2*rangeOfSight*distToObstacle*std::cos(diffRadians);
//		std::cout << "degreea: " << desiredDegrees << " degreeb: " << direction << " diff: " << diffDegrees << " rada: " << desiredRadians << " radb: " << radians << " diffRadians: " << diffRadians << " value: " << value << std::endl;
		if(value<minValue)
		{
			minValue = value;
			finalDirection = direction;
		}
	}

//	std::cout << "final direction: " << finalDirection << " with value: " << minValue << std::endl;
	_direction = finalDirection;

	Engine::Point2D<float> newPos = getNextPos(_direction, Engine::Point2D<float>(_position._x+_rest._x, _position._y+_rest._y));
	Engine::Point2D<int> newIntPos = Engine::Point2D<int>(std::floor(newPos._x), std::floor(newPos._y));
	_rest._x = newPos._x - newIntPos._x;
	_rest._y = newPos._y - newIntPos._y;
//	std::cout << "rest: " << _rest << " new pos: " << newPos << " new int pos: " << newIntPos << std::endl;

	if(_world->checkPosition(newIntPos) && _world->getDynamicRaster(eObstacles).getValue(newIntPos)==0)
	{
	//	if(_world->getDynamicRaster(eExits).getValue(newIntPos)!=0) // || _world->getAgent(newIntPos).size()==0)
	//	{
			_actions.push_back(new MoveAction(newIntPos));
	//	}
	}
}

void PanicAgent::updateState()
{
	if(_world->getDynamicRaster(eExits).getValue(_position)==1)
	{
		_exited = true;
	}
}

void PanicAgent::registerAttributes()
{
	registerIntAttribute("direction");
}

void PanicAgent::serialize()
{
	serializeAttribute("direction", _direction);
}
	
void PanicAgent::setExit( const Engine::Point2D<int> & exit )
{
	_exit = exit;
}

} // namespace Panic

