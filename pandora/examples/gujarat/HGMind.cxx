

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

HGMind::~HGMind()
{
	clearSectorKnowledge();
}

void HGMind::createHRSectors( 	const Engine::Point2D<int>& agentPos
				,std::vector< Sector* >* resultHRSectors
				,std::vector< Engine::Point2D<int> >* cellPool) const
{
	int homeRange  = ((GujaratConfig)_world.getConfig())._homeRange;
	int numSectors = ((GujaratConfig)_world.getConfig())._numSectors;
	std::vector< Sector* > HRSectors;
	
	//if(HRSectors.size()==0)
	//{
		
		
		//cellPool.resize(GujaratState::HRCellsInHomeRange);
		if(0 == cellPool->size())
		{
			cellPool->resize((2*homeRange+1)*(2*homeRange+1));
		}
		int insertPoint = 0;
		
		HRSectors.resize(numSectors);	
		for ( int k = 0; k < numSectors; k++ )
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
				
				(*cellPool)[insertPoint]=p;				
				HRSectors[indexSector]->addCell( &((*cellPool)[insertPoint]) );				
				insertPoint++;
				
				if (_world.getValue(eSoils,p) == INTERDUNE)
				{
					HRSectors[indexSector]->_numInterDunes++;
				}
				
				//HRSectors[indexSector]->addCell( p );
				//getWorld()->setValue( "sectors", p, 1 );	
			}
		}
	//}
	
	
	// Remove empty sectors.
	// Empty sectors can happen if agent is near enough of a world boundary.
	/*unsigned int read  = 0;
	unsigned int write = 0;
	while (read<HRSectors.size())
	{
		if(HRSectors[read]->cells().size()<1)
		{
			read++;
		}
		else
		{
			if(read!=write)
			{
				if(HRSectors[write]->cells().size()<1) 
				{
					delete HRSectors[write];
				}
				HRSectors[write] = HRSectors[read];
			}
			read++;
			write++;
		}
	}	
	HRSectors.resize(write);*/
	
	for ( unsigned k = 0; k < HRSectors.size(); k++ )
	{
		if(HRSectors[k]->cells().size()>0) 
		{
			HRSectors[k]->setDirection(k);
			resultHRSectors->push_back(HRSectors[k]);
			HRSectors[k] = 0;
		}
		else
		{
			delete HRSectors[k];
		}			
	}
	
	
}


void HGMind::createLRSectors( 	const Engine::Point2D<int>& agentPos, 
				std::vector< Sector* >* resultLRSectors,
				std::vector< Engine::Point2D<int> >* cellPool) const
{
	std::vector< Sector* > LRSectors;
	int homeRange  = ((GujaratConfig)_world.getConfig())._homeRange;
	int numSectors = ((GujaratConfig)_world.getConfig())._numSectors;
	
	int lowResHomeRange = ((GujaratConfig)_world.getConfig())._lowResHomeRange;	
	
	//if(LRSectors.size()==0)
	//{
		//cellPool.resize(GujaratState::LRCellsInHomeRange);
		if(0 == cellPool->size())
		{
			cellPool->resize((2*lowResHomeRange+1)*(2*lowResHomeRange+1));
		}
		int insertPoint = 0;
				
		LRSectors.resize(numSectors);
		for ( int k = 0; k < numSectors; k++ )
		{			
			LRSectors[k] = new Sector(_world);	
		}
		
		//std::cout << "**********************" << std::endl;
		
		register int C = ((GujaratConfig)_world.getConfig())._lowResolution;
		Engine::Point2D<int> LRpos;
		_world.worldCell2LowResCell( agentPos, LRpos );
		int insertPoint00 = -1;
		for ( int x=-lowResHomeRange; x<=lowResHomeRange; x++ )
		{
			for ( int y=-lowResHomeRange; y<=lowResHomeRange; y++ )
			{	
				Engine::Point2D<int> LRxycell(x+LRpos._x,y+LRpos._y);
				
				if (x==0 && y==0)
				{
					insertPoint00 = insertPoint;
					(*cellPool)[insertPoint]=LRxycell;
					insertPoint++;
					continue;
				}
				
				int indexSector = GujaratState::sectorsMask(x+lowResHomeRange,y+lowResHomeRange,GujaratState::getLRSectorsMask());
				
				if ( indexSector == - 1 )
				{
					continue;
				}			
				
				//std::cout << "CELL:"<< x << "," << y << " at " << indexSector << std::endl;
				
				
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
				
				if(	_world.getOverlapBoundaries().isInside(corners[0]) 
				&&
				_world.getOverlapBoundaries().isInside(corners[1]) 
				&&
				_world.getOverlapBoundaries().isInside(corners[2]) 
				&&
				_world.getOverlapBoundaries().isInside(corners[3]))	
				{
					(*cellPool)[insertPoint]=LRxycell;
					LRSectors[indexSector]->addCell( &((*cellPool)[insertPoint]) );
					insertPoint++;
				//LRSectors[indexSector]->addCell( LRxycell );
				}				
			}//for
		}//for
		
		// Remove empty sectors.
		// Empty sectors can happen if agent is near enough of a world boundary.
		
		
		// Remove empty sectors
		//*? ucthack
		/*
		unsigned int read  = 0;
		unsigned int write = 0;
		while (read<LRSectors.size())
		{
			if(LRSectors[read]->cells().size()<1)
			{
				read++;
			}
			else
			{
				if(read!=write) 
				{
					if(LRSectors[write]->cells().size()<1) 
					{
						delete LRSectors[write];
					}
					LRSectors[write] = LRSectors[read];
				}
				read++;
				write++;
			}
		}	
		LRSectors.resize(write);
		*/
		for ( unsigned k = 0; k < LRSectors.size(); k++ )
		{
			if(LRSectors[k]->cells().size()>0) 
			{
				LRSectors[k]->setDirection(k);
				resultLRSectors->push_back(LRSectors[k]);
				LRSectors[k] = 0;
			}
			else
			{
				delete LRSectors[k];
			}			
		}
	
		
		for ( unsigned k = 0; k < resultLRSectors->size(); k++ )
		{
			(*resultLRSectors)[k]->addCell( &((*cellPool)[insertPoint00]) );
		}
	//}	   
	assert(_logName.str().length() >0);
	//log_INFO(_logName.str(),"CREATE_LR_DBG("<< (unsigned long)this <<"): " << agentPos << " produces " << resultLRSectors->size() );
	
	
}	


void HGMind::updateKnowledge(Engine::Point2D<int> position)
{		
	// update utility  _HRsectors ???
	// _HRsectors contain only Point2D referencing HR cells in HR raster of resources,
	// so there are no utility attribs to update.
	
	
	if (_HRSectors.size()==0)
	{
		createHRSectors( position, &_HRSectors, &_HRCellPool);
		
	}
	if (_LRSectors.size()==0)
	{
		createLRSectors( position, &_LRSectors, &_LRCellPool);
	}
	
	for ( unsigned k = 0; k < _LRSectors.size(); k++ )
	{
		_LRSectors[k]->updateFeaturesLR(_LRResourceRaster);
	}
	
}


void HGMind::updateKnowledge( const Engine::Point2D<int>& agentPos
								, const Engine::Raster& dataRaster
								, std::vector< Sector* >* HRSectors
								, std::vector< Sector* >* LRSectors
								, std::vector< Engine::Point2D<int> >* HRCellPool
								, std::vector< Engine::Point2D<int> >* LRCellPool
							) const
{	
	//log_INFO(_logName.str(),"HGMind::updateKnowledge receives: " << agentPos << ", amount sectors " << LRSectors->size());	
	
	
	if (HRSectors->size()==0)
	{
		createHRSectors(agentPos,HRSectors, HRCellPool);
	}
	
	if (LRSectors->size()==0)
	{
		createLRSectors(agentPos,LRSectors, LRCellPool);
	}
	
	assert(_logName.str().length() >0);
	//log_INFO(_logName.str(),"CREATE_LR_DBG("<< (unsigned long)this <<"): " << agentPos << " after createLRSectors " << LRSectors->size() );
	
	for ( unsigned k = 0; k < LRSectors->size(); k++ )
	{	
		(*LRSectors)[k]->updateFeaturesLR(dataRaster);
	} 
	
	
	//log_INFO(_logName.str(),"CREATE_LR_DBG("<< (unsigned long)this <<"): " << agentPos << " after update features LRSectors " << LRSectors->size() );
	
	
	//log_INFO(_logName.str(),"CREATE_HR_DBG("<< (unsigned long)this <<"): " << agentPos << " after update features HRSectors " << HRSectors->size() );
	
	
	//log_INFO(_logName.str(),"HGMind::updateKnowledge ends: " << agentPos << ", amount sectors " << LRSectors->size());	
	
	
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
