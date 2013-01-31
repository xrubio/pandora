
#include "Battlefield.hxx"
#include "Soldier.hxx"
#include "Agent.hxx"
#include "Bullet.hxx"
#include "BattlefieldConfig.hxx"

#include "Raster.hxx"
#include "Point2D.hxx"
#include "Exceptions.hxx"
#include "Statistics.hxx"
#include "Rectangle.hxx"

#include <cmath>
#include <limits>

namespace BattleSim 
{
	
Battlefield::Battlefield( Engine::Simulation & simulation, const BattlefieldConfig & config ) : World(simulation, 1, true, config._path+"/battle.h5"), _config(config)
{
	setSearchAgents(true);
}

Battlefield::~Battlefield()
{
}

void Battlefield::createRasters()
{
	registerDynamicRaster("virtualRecord", true);
	registerDynamicRaster("impacts", true);

	getDynamicRaster("virtualRecord").setInitValues(0, std::numeric_limits<int>::max(), 0);
	getDynamicRaster("impacts").setInitValues(0, std::numeric_limits<int>::max(), 0);
}

void Battlefield::createAgents()
{
	// 10 meters before starting combat
	int distanceBetweenSides = 320;

	// blue side, top zone
	int numBlueSoldiers = _config._numBlueSoldiers;
	int numBlueColumns = numBlueSoldiers/_config._blueRanks;

	int numRedSoldiers = _config._numRedSoldiers;
	int numRedColumns = numRedSoldiers/_config._redRanks;


	// 50 meters for each side
	// 12 is 2 * 3 ranks * 2(0.5m)
	int heightBoundary = _simulation.getSize() - distanceBetweenSides - 12;
	heightBoundary /= 2;
	int widthBoundary = (_simulation.getSize() - std::max(numBlueColumns, numRedColumns)*2)/2;

	for(int i=0; i<numBlueSoldiers; i++)
	{
		int column = numBlueColumns-1-i%numBlueColumns;
		int rank = i/numBlueColumns;
		//std::cout << _id << " blue soldier id: " << i << " of: "<< numBlueSoldiers << " in rank: " << rank+1 << "/" << numBlueLines << " and column: " << column << "/" << numBlueColumns << std::endl;
		// each 1m.
		Engine::Point2D<int> posInRegiment((column*2)+widthBoundary, heightBoundary - rank*2);
	
		if(_boundaries.isInside(posInRegiment))
		{
			std::ostringstream oss;
			oss << "blueSoldier_" << i;
			Soldier * newSoldier = new Soldier(oss.str(), rank+1, _config._blueCohesionRating, _config._blueCohesionDistance, _config._blueAccuracy );
			newSoldier->setPosition(posInRegiment);

			int reloadingTime = 48;
			int interval = 0;
			int order = 0;	
			int platoon = (column*16/numBlueColumns)+1;
			switch((int)_config._blueTactics)
			{
				case eVolley:
					newSoldier->setDelayBeforeFirstFire(0);
					break;
				case eRanks:
					interval = reloadingTime/_config._blueRanks;
					order =  _config._blueRanks - rank - 1;
					newSoldier->setDelayBeforeFirstFire(order * interval); 
					break;
				case ePlatoon:
					interval = reloadingTime/4;
					if(rank!=0)
					{
						switch(platoon)
						{	
							case 1:
							case 4:
							case 7:
							case 10:
							case 13:
							case 16:
								order = 0;
								break;
							case 3:
							case 6:
							case 11:
							case 14:
								order = 1;
								break;
							default:
								order = 2;
						}
					}
					else
					{
						if(platoon==8 || platoon==9)
						{
							order = 2;
						}
						else
						{
							order = 3;
						}
					}
					newSoldier->setDelayBeforeFirstFire(order * interval); 
					break;
				case eCatalan:		
					interval = reloadingTime/8;
					switch(platoon)
					{	
						case 1:
						case 16:
							order = 0;
							break;
						case 2:
						case 15:
							order = 1;
							break;
						case 3:
						case 14:
							order = 2;
							break;
						case 4:
						case 13:
							order = 3;
							break;
						case 5:
						case 12:
							order = 4;
							break;
						case 6:
						case 11:
							order = 5;
							break;
						case 7:
						case 10:
							order = 6;
							break;
						case 8:
						case 9:
							order = 7;
							break;
						default:
							order = -1;
					}
					newSoldier->setDelayBeforeFirstFire(order * interval); 
					break;
				default:
					break;
			}	
			addAgent(newSoldier);
		}
	}


	for(int i=0; i<numRedSoldiers; i++)
	{
		int column = i%numRedColumns;
		int rank = i/numRedColumns;
		//std::cout << _id << " red soldier id: " << i << " of: "<< numRedSoldiers << " in rank: " << rank+1 << "/" << numRedLines << " and column: " << column << "/" << numRedColumns << std::endl;
		// each 1m.
		Engine::Point2D<int> posInRegiment((column*2)+widthBoundary, heightBoundary +distanceBetweenSides+(rank*2));
		if(_boundaries.isInside(posInRegiment))
		{
			std::ostringstream oss;
			oss << "redSoldier_" << i;
			Soldier * newSoldier = new Soldier(oss.str(), rank+1, _config._redCohesionRating, _config._redCohesionDistance, _config._redAccuracy );
			newSoldier->setPosition(posInRegiment);	

			int reloadingTime = 48;
			int interval = 0;
			int order = 0;	
			int platoon = (column*16/numRedColumns)+1;
			switch((int)_config._redTactics)
			{
				case eVolley:
					newSoldier->setDelayBeforeFirstFire(0);
					break;
				case eRanks:
					interval = reloadingTime/_config._redRanks;
					order =  _config._redRanks - rank - 1;
					newSoldier->setDelayBeforeFirstFire(order * interval); 
					break;
				case ePlatoon:
					interval = reloadingTime/4;
					if(rank!=0)
					{
						switch(platoon)
						{	
							case 1:
							case 4:
							case 7:
							case 10:
							case 13:
							case 16:
								order = 0;
								break;
							case 3:
							case 6:
							case 11:
							case 14:
								order = 1;
								break;
							default:
								order = 2;
						}
					}
					else
					{
						if(platoon==8 || platoon==9)
						{
							order = 2;
						}
						else
						{
							order = 3;
						}
					}
					newSoldier->setDelayBeforeFirstFire(order * interval); 
					break;
				case eCatalan:		
					interval = reloadingTime/8;
					switch(platoon)
					{	
						case 1:
						case 16:
							order = 0;
							break;
						case 2:
						case 15:
							order = 1;
							break;
						case 3:
						case 14:
							order = 2;
							break;
						case 4:
						case 13:
							order = 3;
							break;
						case 5:
						case 12:
							order = 4;
							break;
						case 6:
						case 11:
							order = 5;
							break;
						case 7:
						case 10:
							order = 6;
							break;
						case 8:
						case 9:
							order = 7;
							break;
						default:
							order = -1;
					}
					newSoldier->setDelayBeforeFirstFire(order * interval); 
					break;
				default:
					break;
			}	
			addAgent(newSoldier);
		}
	}
}

Engine::Agent * Battlefield::createAgentFromPackage( const std::string & type, void * package )
{
	if(type.compare("blueSoldier")==0 || type.compare("redSoldier")==0)
	{
		SoldierPackage * formattedPackage = 0;
		try
		{
			formattedPackage = (SoldierPackage *)package;
		}
		catch(std::exception & e)
		{
			std::cout << "Battlefield::createAgentFromPackage, wrong cast with exception: " << e.what() << std::endl;
		}
		return (Engine::Agent*)(new Soldier(*formattedPackage));
	}
	else if(type.compare("blueBullet")==0 || type.compare("redBullet")==0)
	{
		BulletPackage * formattedPackage = 0;
		try
		{
			formattedPackage = (BulletPackage *)package;
		}
		catch(std::exception & e)
		{
			std::cout << "Battlefield::createAgentFromPackage, wrong cast with exception: " << e.what() << std::endl;
		}
		return (Engine::Agent*)(new Bullet(*formattedPackage));
	}
	else
	{	
		std::stringstream oss;
		oss << "Battlefield::createAgentFromPackage - unknown agent type: " << type;
		throw Engine::Exception(oss.str());
	}
}

void * Battlefield::createPackage( const std::string & type )
{
	if(type.compare("blueSoldier")==0 || type.compare("redSoldier")==0)
	{
		return new SoldierPackage;
	}
	else if(type.compare("blueBullet")==0 || type.compare("redBullet")==0)
	{
		return new BulletPackage;
	}
	else
	{	
		std::stringstream oss;
		oss << "Battlefield::createPackage - unknown agent type: " << type;
		throw Engine::Exception(oss.str());
	}
}

void Battlefield::registerTypes()
{
	_types.insert( std::make_pair( "redsoldier", registerSoldier()));
	_types.insert( std::make_pair( "blueSoldier", registerSoldier()));
	_types.insert( std::make_pair( "redBullet", registerBullet()));
	_types.insert( std::make_pair( "blueBullet", registerBullet()));
}

MPI_Datatype * Battlefield::registerBullet()
{
	BulletPackage package;
	int numVariables = 4;
	// we will send id, x and y
	int blockLengths[numVariables];
	MPI_Datatype typelist[numVariables];
	// 100 chars
	blockLengths[0] = 100;
	typelist[0] = MPI_CHAR;

	for(int i=1; i<numVariables; i++)
	{
		blockLengths[i] = 1;
		typelist[i] = MPI_INT;
	}
	MPI_Aint displacements[numVariables];
	displacements[0] = 0;

	MPI_Aint startAddress;
	MPI_Aint address;
	MPI_Address(package._id, &startAddress); 

	MPI_Address(&package._position._x, &address);	
	displacements[1] = address-startAddress;
	MPI_Address(&package._position._y, &address);
	displacements[2] = address-startAddress;
	MPI_Address(&package._isBlueSide, &address);
	displacements[3] = address-startAddress;

	MPI_Datatype * newDataType = new MPI_Datatype;
	MPI_Type_struct(numVariables, blockLengths, displacements, typelist, newDataType);
	MPI_Type_commit(newDataType);
	return newDataType;
}

MPI_Datatype * Battlefield::registerSoldier()
{
	SoldierPackage package;
	int numVariables = 12;
	// we will send id, x and y
	int blockLengths[numVariables];
	MPI_Datatype typelist[numVariables];
	// 100 chars
	blockLengths[0] = 100;
	typelist[0] = MPI_CHAR;

	for(int i=1; i<numVariables; i++)
	{
		blockLengths[i] = 1;
		typelist[i] = MPI_INT;
	}
	MPI_Aint displacements[numVariables];
	displacements[0] = 0;

	MPI_Aint startAddress;
	MPI_Aint address;
	MPI_Address(package._id, &startAddress); 

	MPI_Address(&package._position._x, &address);
	displacements[1] = address-startAddress;
	MPI_Address(&package._position._y, &address);
	displacements[2] = address-startAddress;
	MPI_Address(&package._isBlueSide, &address);
	displacements[3] = address-startAddress;
	MPI_Address(&package._stress, &address);
	displacements[4] = address-startAddress;
	MPI_Address(&package._cohesionRating, &address);
	displacements[5] = address-startAddress;
	MPI_Address(&package._cohesionDistance, &address);
	displacements[6] = address-startAddress;
	MPI_Address(&package._rank, &address);
	displacements[7] = address-startAddress;
	MPI_Address(&package._moving, &address);
	displacements[8] = address-startAddress;
	MPI_Address(&package._routing, &address);
	displacements[9] = address-startAddress;
	MPI_Address(&package._reloadingTime, &address);
	displacements[10] = address-startAddress;
	MPI_Address(&package._timeUntilLoaded, &address);
	displacements[11] = address-startAddress;

	MPI_Datatype * newDataType = new MPI_Datatype;
	MPI_Type_struct(numVariables, blockLengths, displacements, typelist, newDataType);
	MPI_Type_commit(newDataType);
	return newDataType;
}
	
void Battlefield::stepEnvironment()
{
	if(_step+1!=_simulation.getNumSteps())
	{
		return;
	}
	int blueRemainingSoldiers = 0;
	int redRemainingSoldiers = 0;
	for(AgentsList::iterator it=_agents.begin(); it!=_agents.end(); it++)
	{
		Engine::Agent * agent = *it;
		Soldier * soldier = (Soldier*)agent;
		if(soldier->exists() && !soldier->isRouting())
		{
			if(soldier->isType("blueSoldier"))
			{
				blueRemainingSoldiers++;
			}
			else
			{
				redRemainingSoldiers++;
			}
		}
	}
	std::cout << "RCOH:" << _config._redCohesionRating << ";RACC:" << _config._redAccuracy << ";RTAC:" << _config._redTactics << ";RSOL:" << redRemainingSoldiers << ";BCOH:" << _config._blueCohesionRating << ";BACC:" << _config._blueAccuracy << ";BTAC:" << _config._blueTactics << ";BSOL:" << blueRemainingSoldiers << ";" << std::endl;
}

} // namespace BattleSim

