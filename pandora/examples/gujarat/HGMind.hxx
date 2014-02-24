

#ifndef __HGMind_hxx__
#define __HGMind_hxx__

#include <sstream>
//#include <HunterGatherer.hxx>
//#include <GujaratAgent.hxx>
#include <vector>
#include <Point2D.hxx>
#include <Raster.hxx>

namespace Gujarat
{
	class Sector;
	class GujaratWorld;
	class GujaratConfig;
	
	class HGMind
	{
	protected:
	//	int _x;
		
		GujaratWorld & _world;
		
		std::vector<Sector *> _HRSectors;
		std::vector<Sector *> _LRSectors;
		
		std::vector< Engine::Point2D<int> > _HRCellPool;
		std::vector< Engine::Point2D<int> > _LRCellPool;
		
		Engine::Raster & _LRResourceRaster;
		Engine::Raster _LRTimeStamps;
		
	public:
		HGMind(GujaratWorld & w);
		~HGMind();
		//HGMind(int x);
		
		//int read();
		Engine::Raster & getLRResourcesRaster() 
		{ 
			return _LRResourceRaster;
		}
		
		void createHRSectors( const Engine::Point2D<int>& agentPos
				, std::vector< Sector* >* resultHRSectors
				, std::vector< Engine::Point2D<int> >* cellPool ) const;
										
		
		void createLRSectors( 	const Engine::Point2D<int>& agentPos
					, std::vector< Sector* >* resultLRSectors
					, std::vector< Engine::Point2D<int> >* cellPool ) const;
		
		//----------------------------------------------------------------------
		const std::vector<Sector *> & getHRSectors()const
		{return _HRSectors;}
		const std::vector<Sector *> & getLRSectors()const
		{return _LRSectors;}
		const std::vector< Engine::Point2D<int> > & getHRCellPool()const
		{return _HRCellPool;}
		const std::vector< Engine::Point2D<int> > & getLRCellPool()const
		{return _LRCellPool;}
		//----------------------------------------------------------------------
		std::vector<Sector *> & getHRSectorsNoConst()
		{return _HRSectors;}
		std::vector<Sector *> & getLRSectorsNoConst()
		{return _LRSectors;}
		std::vector< Engine::Point2D<int> > & getHRCellPoolNoConst()
		{return _HRCellPool;}
		std::vector< Engine::Point2D<int> > & getLRCellPoolNoConst()
		{return _LRCellPool;}
		//----------------------------------------------------------------------
		
		Engine::Raster & getLRTimeStamps() { return _LRTimeStamps; }
		
		void updateKnowledge(Engine::Point2D<int> position);
		
		void	updateKnowledge( const Engine::Point2D<int>& agentPos
								, const Engine::Raster& dataRaster
								, std::vector< Sector* >* HRSectors
								, std::vector< Sector* >* LRSectors
								, std::vector< Engine::Point2D<int> >* HRCellPool
								, std::vector< Engine::Point2D<int> >* LRCellPool
   							) const;	
				
		void clearSectorKnowledge();
		
		virtual void updateDueToExecuteAction(Sector*) = 0;
	
	public:
		std::stringstream _logName;
};
}

#endif