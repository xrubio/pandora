

#include <iostream>
#include <HunterGatherer.hxx>
#include <GujaratWorld.hxx>
#include <Exceptions.hxx>
#include <Action.hxx>
#include <Sector.hxx>
#include <cmath>
#include <cassert>
#include <AgentController.hxx>

#include <GujaratState.hxx>
#include <GeneralState.hxx>
#include <HGMindFactory.hxx>
#include <Logger.hxx>

#include <GujaratConfig.hxx>


#include <HGMind.hxx>

//#include <GujaratWorld.hxx>

namespace Gujarat
{

/*
HGMind::HGMind(int x):_x(x)
{}

int HGMind::read() { return _x;}
*/

HGMind::HGMind(GujaratWorld & w):
	_world(w),
	_LRResourceRaster(w.getDynamicRaster(eLRResources))
{}

void HGMind::createHRSectors( 	const Engine::Point2D<int>& agentPos,
										std::vector< Sector* >& HRSectors) const
{
	int homeRange  = ((GujaratConfig)_world.getConfig())._homeRange;
	int numSectors = ((GujaratConfig)_world.getConfig())._numSectors;
	
	//if(HRSectors.size()==0)
	//{
		HRSectors.resize(numSectors);	
		for ( unsigned k = 0; k < numSectors; k++ )
		{
			HRSectors[k] = new Sector(_world);
		}
		
		for ( int x=-homeRange; x<=homeRange; x++ )
		{
			for ( int y=-homeRange; y<=homeRange; y++ )
			{
				
				int indexSector = GujaratState::sectorsMask(x+homeRange,y+homeRange, GujaratState::getHRSectorsMask());
				if ( indexSector == - 1 )
				{
					continue;
				}
				
				Engine::Point2D<int> p;
				p._x = agentPos._x + x;
				p._y = agentPos._y + y;
				// TODO overlapboundaries
				if ( !_world.getBoundaries().isInside(p) )
				{
					continue;
				}
				HRSectors[indexSector]->addCell( p );
				//getWorld()->setValue( "sectors", p, 1 );	
			}
		}
	//}
	
}


void HGMind::createLRSectors( 	const Engine::Point2D<int>& agentPos, 
								std::vector< Sector* >& LRSectors ) const
{
	int homeRange  = ((GujaratConfig)_world.getConfig())._homeRange;
	int numSectors = ((GujaratConfig)_world.getConfig())._numSectors;
	
	//if(LRSectors.size()==0)
	//{
		LRSectors.resize(numSectors);
		for ( unsigned k = 0; k < numSectors; k++ )
		{			
			LRSectors[k] = new Sector(_world);	
		}
		
		//std::cout << "**********************" << std::endl;
		
		int lowResHomeRange = ((GujaratConfig)_world.getConfig())._lowResHomeRange;
		//TODO Those low resolution calculi should be arranged by the GujaratWorld class
		register int C = ((GujaratConfig)_world.getConfig())._lowResolution;
		Engine::Point2D<int> LRpos;
		_world.worldCell2LowResCell( agentPos, LRpos );
		for ( int x=-lowResHomeRange; x<=lowResHomeRange; x++ )
		{
			for ( int y=-lowResHomeRange; y<=lowResHomeRange; y++ )
			{				
				//*?
				if (x==0 && y==0)
				{
					continue;
				}
				
				int indexSector = GujaratState::sectorsMask(x+lowResHomeRange,y+lowResHomeRange,GujaratState::getLRSectorsMask());
				
				if ( indexSector == - 1 )
				{
					continue;
				}			
				
				//std::cout << "CELL:"<< x << "," << y << " at " << indexSector << std::endl;
				
				Engine::Point2D<int> LRxycell(x+LRpos._x,y+LRpos._y);
				
				Engine::Point2D<int> corners[4]; // 4 corners that bound the world cells belonging to the low res cell
				corners[0]._x = LRxycell._x*C;
				corners[0]._y = LRxycell._y*C;
				corners[1]._x = LRxycell._x*C;
				corners[1]._y = LRxycell._y*C + C-1;
				corners[2]._x = LRxycell._x*C + C-1;
				corners[2]._y = LRxycell._y*C;
				corners[3]._x = LRxycell._x*C + C-1;
				corners[3]._y = LRxycell._y*C + C-1;
				
				// All four corners of the low res cell must be in the boundaries
				
				if(	!_world.getOverlapBoundaries().isInside(corners[0]) ||
					!_world.getOverlapBoundaries().isInside(corners[1]) ||
					!_world.getOverlapBoundaries().isInside(corners[2]) ||
					!_world.getOverlapBoundaries().isInside(corners[3]))	
				{
					continue;
				}
				
				LRSectors[indexSector]->addCell( LRxycell );
				
			}//for
		}//for
		
		for ( unsigned k = 0; k < numSectors; k++ )
		{
			LRSectors[k]->addCell( LRpos );
		}	
	//}	   
	
}	


void HGMind::updateKnowledge(Engine::Point2D<int> position)
{		
	// update utility  _HRsectors ???
	// _HRsectors contain only Point2D referencing HR cells in HR raster of resources,
	// so there are no utility attribs to update.
	
	int numSectors = ((GujaratConfig)_world.getConfig())._numSectors;
	
	if (_HRSectors.size()==0)
	{
		createHRSectors( position, _HRSectors);
		
	}
	if (_LRSectors.size()==0)
	{
		createLRSectors( position, _LRSectors);
	}
	
	for ( unsigned k = 0; k < numSectors; k++ )
	{
		_LRSectors[k]->updateFeaturesLR(_LRResourceRaster);
	}
	
}


void HGMind::updateKnowledge( const Engine::Point2D<int>& agentPos
								, const Engine::Raster& dataRaster
								, std::vector< Sector* >& HRSectors
								, std::vector< Sector* >& LRSectors  ) const
{	
	
	int numSectors = ((GujaratConfig)_world.getConfig())._numSectors;
	
	if (HRSectors.size()==0)
	{
		createHRSectors(agentPos,HRSectors);
	}
	
	if (LRSectors.size()==0)
	{
		createLRSectors(agentPos,LRSectors);
	}
	
	for ( unsigned k = 0; k < numSectors; k++ )
	{	
		LRSectors[k]->updateFeaturesLR(dataRaster);
	} 
	
}


void HGMind::clearSectorKnowledge() 
{ 
	for ( unsigned i = 0; i < _HRSectors.size(); i++ )
	{
		delete _HRSectors[i]; 
	}		
	_HRSectors.clear(); 
	
	for ( unsigned i = 0; i < _LRSectors.size(); i++ )
	{
		delete _LRSectors[i]; 
	}		
	_LRSectors.clear(); 
}
	
	
}