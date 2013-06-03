
#include "PanicAgent.hxx"

#include <Statistics.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <cmath>

#include "Scenario.hxx"
#include "MoveAction.hxx"
#include "ScenarioConfig.hxx"

namespace Panic
{

PanicAgent::PanicAgent( const std::string & id, ScenarioConfig & config ) : Agent(id), _direction(0), _exited(false), _panicked(false), _config(&config), _compressionThreshold(config._compressionThreshold), _rest(0.0f, 0.0f), _consecutive(0)
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
	//std::cout << "direction: " << directionVector << " for direction: " << direction << " old pos: " << position << " new pos: " << newPos << std::endl;
	return newPos;
}
	
float PanicAgent::getDistToNearestObstacle( const int & direction )
{
	Engine::Point2D<float> newPos = Engine::Point2D<float>(_position._x+_rest._x, _position._y+_rest._y);
	while(1)
	{
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
		
		if(_world->getValue(eExits, newIntPos)==1)
		{	
			if(newIntPos!=_position)
			{
				return newIntPos.distance(_position);
			}
		}

		// obstacle found
		if(_world->getValue(eObstacles, newIntPos)==1)
		{	
			if(newIntPos!=_position)
			{
				return newIntPos.distance(_position);
			}
		}
		
		// too many people (>_bodiesToObstacle)
		if(_world->getValue(eNumAgents, newIntPos)>=_config->_bodiesToObstacle)
		{	
			if(newIntPos!=_position)
			{
				return newIntPos.distance(_position);
			}
		}	
		
		// too many injured people (>_bodiesToObstacle)
		if(_world->getValue(eDeaths, newIntPos)>=_config->_bodiesToObstacle)
		{	
			if(newIntPos!=_position)
			{
				return newIntPos.distance(_position);
			}
		}
	}
}

float PanicAgent::getCompressionLevel( float direction )
{
	float compressionLevel = 0;
	
	Engine::Point2D<float> newPos = getNextPos(_direction, Engine::Point2D<float>(_position._x+_rest._x, _position._y+_rest._y));
	Engine::Point2D<int> newIntPos = Engine::Point2D<int>(std::floor(newPos._x), std::floor(newPos._y));
	Engine::Point2D<int> index;
	for(index._x=newIntPos._x-1; index._x<=newIntPos._x+1; index._x++)
	{
		for(index._y=newIntPos._y-1; index._y<=newIntPos._y+1; index._y++)
		{
			if(!_world->checkPosition(index))
			{
				continue;
			}
			compressionLevel += _world->getValue(eCompression, index);
		}
	}
	return compressionLevel;
}


void PanicAgent::selectActions()
{
	if(_exited || !_panicked)
	{
		return;
	}

	float rangeOfSight = 200.0f;
	// fov in degrees
	int fov = 180;
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
		//std::cout << "pos: " << _position << " to 0,0 rads: " << desiredRadians<< " degrees: " << desiredDegrees << std::endl;
		//std::cout << "pos: " << _position << " basic dir: " << _direction << " fov: " << fov << " direction to test: " << direction << " dist to obstacle: " << distToObstacle << std::endl;

		float diffRadiansCos = std::cos(desiredRadians)*std::cos(radians) + std::sin(desiredRadians)*std::sin(radians);
		float diffRadians = std::acos(diffRadiansCos);
		float diffDegrees = diffRadians*180.0f/M_PI;

		float dirValue = rangeOfSight*rangeOfSight + distToObstacle*distToObstacle - 2*rangeOfSight*distToObstacle*std::cos(diffRadians);
	//	float value = _config->_weightDir*dirValue + _config->_weightCompression*getCompressionLevel(newDirection);
		float value = (1+getCompressionLevel(newDirection))*dirValue;
//		std::cout << "dir: " <<newDirection << " dir: " <<  _config->_weightDir*dirValue << " compression: " << _config->_weightCompression*getCompressionLevel(newDirection) << std::endl;
		//std::cout << "degreea: " << desiredDegrees << " degreeb: " << direction << " diff: " << diffDegrees << " rada: " << desiredRadians << " radb: " << radians << " diffRadians: " << diffRadians << " value: " << value << std::endl;


		if(value<minValue)
		{	
			Engine::Point2D<float> newPos = getNextPos(direction, Engine::Point2D<float>(_position._x+_rest._x, _position._y+_rest._y));
			Engine::Point2D<int> newIntPos = Engine::Point2D<int>(std::floor(newPos._x), std::floor(newPos._y));
			if(newIntPos!=_position || _consecutive<2)
			{
				if(_world->checkPosition(newIntPos) && _world->getDynamicRaster(eObstacles).getValue(newIntPos)==0)
				{
					minValue = value;
					finalDirection = direction;
				}
			}
		}
	}

	//std::cout << "final direction: " << finalDirection << " with value: " << minValue << std::endl;
	_direction = finalDirection;

	Engine::Point2D<float> newPos = getNextPos(_direction, Engine::Point2D<float>(_position._x+_rest._x, _position._y+_rest._y));
	Engine::Point2D<int> newIntPos = Engine::Point2D<int>(std::floor(newPos._x), std::floor(newPos._y));
	_rest._x = newPos._x - newIntPos._x;
	_rest._y = newPos._y - newIntPos._y;
//	std::cout << this << " rest: " << _rest << " new pos: " << newPos << " new int pos: " << newIntPos << " for step: " << " dir: " << _direction << " step: " <<  _world->getCurrentStep() << std::endl;

	if(_world->checkPosition(newIntPos) && _world->getDynamicRaster(eObstacles).getValue(newIntPos)==0)
	{
		if(newIntPos!=_position)
		{
			_consecutive = 0;
		}
		else
		{
			_consecutive++;
			/*
			if(_consecutive>1)
			{
				std::cout << "consecutive at pos: " << newIntPos << " - " << _consecutive << std::endl;
			}
			*/
		}
		_actions.push_back(new MoveAction(newIntPos, _config->_agentCompressionWeight, _config->_wallCompressionWeight, _config->_contagion));
	}
}

void PanicAgent::updateState()
{
	if(_exited)
	{
		return;
	}

	if(_world->getValue(eExits, _position)==1)
	{
		_exited = true;	
		_world->removeAgent(this);
	}

	if(_world->getValue(eCompression, _position)>_compressionThreshold)
	{
		_world->setValue(eDeaths, _position, _world->getValue(eDeaths, _position)+1);		
		_world->setValue(eNumAgents, _position, _world->getValue(eNumAgents, _position)-1);		
		_world->setValue(eCompression, _position, std::max(0, _world->getValue(eCompression, _position)-_compressionThreshold));
		_world->removeAgent(this);
	}

	if(!_panicked && _world->getValue(ePanic, _position)==1)
	{
		_panicked = true;
	}

}

void PanicAgent::registerAttributes()
{
	registerIntAttribute("direction");
	registerIntAttribute("panicked");
}

void PanicAgent::serialize()
{
	serializeAttribute("direction", _direction);
	serializeAttribute("panicked", _panicked);
}
	
void PanicAgent::setExit( const Engine::Point2D<int> & exit )
{
	_exit = exit;
}

} // namespace Panic

