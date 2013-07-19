
#include "Battlefield.hxx"
#include "Warrior.hxx"
#include <Agent.hxx>
#include "BattlefieldConfig.hxx"

#include <Raster.hxx>
#include <Point2D.hxx>
#include <Exceptions.hxx>
#include <Statistics.hxx>
#include <Rectangle.hxx>

#include <cmath>
#include <limits>

namespace ClassicalWarfare 
{
	
Battlefield::Battlefield( Engine::Simulation & simulation, const BattlefieldConfig & config ) : World(simulation, 1, true, config._resultsFile), _config(config)
{
}

Battlefield::~Battlefield()
{
}

void Battlefield::createRasters()
{
	//registerDynamicRaster("virtualRecord", true, eVirtualRecord);
	//getDynamicRaster(eVirtualRecord).setInitValues(0, std::numeric_limits<int>::max(), 0);
}

void Battlefield::createLine( const Line & line, int depth, bool isRed, int & createdWarriors)
{
	Engine::Point2D<int> blockPosition(0, depth);
	// we need to center the line
	int lineWidth = line._numBlocks*line._columns+(line._numBlocks-1)*line._separationBetweenBlocks;
	if(isRed)
	{
		blockPosition._x = (getGlobalBoundaries()._size._x-lineWidth)/2;
		blockPosition._x += line._offset*line._columns;
	}
	else
	{
		blockPosition._x = getGlobalBoundaries()._size._x-(getGlobalBoundaries()._size._x-lineWidth)/2;
		blockPosition._x -= line._offset*line._columns;
	}

	for(int i=0; i<line._numBlocks; i++)
	{
		std::cout << "next block at pos: " << blockPosition << std::endl;
		Engine::Point2D<int> warriors(0,0);
		for(warriors._y=0; warriors._y<line._ranks; warriors._y++)
		{
			for(warriors._x=0; warriors._x<line._columns; warriors._x++)
			{
				std::ostringstream oss;
				if(isRed)
				{
	 				oss << "RedWarrior_" << createdWarriors;
				}
				else
				{
	 				oss << "BlueWarrior_" << createdWarriors;
				}
				createdWarriors++;
				Warrior * warrior = new Warrior(oss.str());
				addAgent(warrior);
				if(isRed)
				{
					warrior->setPosition(blockPosition+warriors);
				}
				else
				{
					warrior->setPosition(blockPosition-warriors);
				}
			}
		} 
		if(isRed)
		{
			blockPosition._x += line._columns+line._separationBetweenBlocks;
		}
		else
		{
			blockPosition._x -= (line._columns+line._separationBetweenBlocks);
		}
	}
}

void Battlefield::createAgents()
{
	// red a baix, blue a dalt, calculem primer el primer soldat de l'esquerra de la primera línia de combat i anem cap a darrera
	int depth = getGlobalBoundaries()._size._y/2+_config._initialDistance/2;
	int redWarriors = 0;
	for(std::list<Line>::const_iterator it=_config._redLines.begin(); it!=_config._redLines.end(); it++)
	{
		createLine(*it, depth, true, redWarriors);
		depth += (*it)._ranks+_config._redSeparationBetweenLines;
	}

	depth = getGlobalBoundaries()._size._y/2-_config._initialDistance/2;
	int blueWarriors = 0;
	for(std::list<Line>::const_iterator it=_config._blueLines.begin(); it!=_config._blueLines.end(); it++)
	{
		createLine(*it, depth, false, blueWarriors);
		depth -= ((*it)._ranks+_config._blueSeparationBetweenLines);
	}
}

void Battlefield::stepEnvironment()
{
}

} // namespace ClassicalWarfare

