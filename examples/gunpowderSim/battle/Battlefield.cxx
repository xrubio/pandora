
#include "Battlefield.hxx"
#include "Soldier.hxx"
#include <Agent.hxx>
#include "Bullet.hxx"
#include "BattlefieldConfig.hxx"

#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <Exception.hxx>
#include <Statistics.hxx>
#include <Rectangle.hxx>

#include <cmath>
#include <limits>

namespace BattleSim 
{
	
Battlefield::Battlefield( BattlefieldConfig * config, Engine::Scheduler * scheduler ) : World(config, scheduler, false) 
{
}

Battlefield::~Battlefield()
{
}

void Battlefield::createRasters()
{
	registerDynamicRaster("virtualRecord", true, eVirtualRecord);
	getDynamicRaster(eVirtualRecord).setInitValues(0, std::numeric_limits<int>::max(), 0);

	registerDynamicRaster("impacts", true, eImpacts);
	getDynamicRaster(eImpacts).setInitValues(0, std::numeric_limits<int>::max(), 0);
}

void Battlefield::createAgents()
{
    const BattlefieldConfig & battleConfig = (const BattlefieldConfig&)getConfig();
	// blue side, top zone
	int numBlueSoldiers = battleConfig._numBlueSoldiers;
	int numBlueColumns = numBlueSoldiers/battleConfig._blueRanks;

	int numRedSoldiers = battleConfig._numRedSoldiers;
	int numRedColumns = numRedSoldiers/battleConfig._redRanks;


	// 50 meters for each side
	// 12 is 2 * 3 ranks * 2(0.5m)
	int heightBoundary = getConfig().getSize()._height - battleConfig._initialDistance- 12;
	heightBoundary /= 2;
	int widthBoundary = (getConfig().getSize()._width - std::max(numBlueColumns, numRedColumns)*2)/2;

	for(int i=0; i<numBlueSoldiers; i++)
	{
		int column = numBlueColumns-1-i%numBlueColumns;
		int rank = i/numBlueColumns;
		//std::cout << _id << " blue soldier id: " << i << " of: "<< numBlueSoldiers << " in rank: " << rank+1 << "/" << numBlueLines << " and column: " << column << "/" << numBlueColumns << std::endl;
		// each 1m.
		Engine::Point2D<int> posInRegiment((column*2)+widthBoundary, heightBoundary - rank*2);
	
		if(getBoundaries().contains(posInRegiment))
		{
			std::ostringstream oss;
			oss << "blueSoldier_" << i;
			Soldier * newSoldier = new Soldier(oss.str(), rank+1, battleConfig._blueCohesionRating, battleConfig._blueCohesionDistance, battleConfig._blueAccuracy, battleConfig._blueReloadingTime );
			newSoldier->setPosition(posInRegiment);

			int interval = 0;
			int order = 0;	
			int platoon = (column*16/numBlueColumns)+1;
			switch((int)battleConfig._blueTactics)
			{
				case eVolley:
					newSoldier->setDelayBeforeFirstFire(0);
					break;
				case eRanks:
					interval = battleConfig._blueReloadingTime/battleConfig._blueRanks;
					order =  battleConfig._blueRanks - rank - 1;
					newSoldier->setDelayBeforeFirstFire(order * interval); 
					break;
				case ePlatoon:
					interval = battleConfig._blueReloadingTime/4;
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
					interval = battleConfig._blueReloadingTime/8;
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
		Engine::Point2D<int> posInRegiment((column*2)+widthBoundary, heightBoundary +battleConfig._initialDistance+(rank*2));
		if(getBoundaries().contains(posInRegiment))
		{
			std::ostringstream oss;
			oss << "redSoldier_" << i;
			Soldier * newSoldier = new Soldier(oss.str(), rank+1, battleConfig._redCohesionRating, battleConfig._redCohesionDistance, battleConfig._redAccuracy, battleConfig._redReloadingTime );
			newSoldier->setPosition(posInRegiment);	

			int interval = 0;
			int order = 0;	
			int platoon = (column*16/numRedColumns)+1;
			switch((int)battleConfig._redTactics)
			{
				case eVolley:
					newSoldier->setDelayBeforeFirstFire(0);
					break;
				case eRanks:
					interval = battleConfig._redReloadingTime/battleConfig._redRanks;
					order =  battleConfig._redRanks - rank - 1;
					newSoldier->setDelayBeforeFirstFire(order * interval); 
					break;
				case ePlatoon:
					interval = battleConfig._redReloadingTime/4;
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
					interval = battleConfig._redReloadingTime/8;
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

void Battlefield::stepEnvironment()
{
}

} // namespace BattleSim

