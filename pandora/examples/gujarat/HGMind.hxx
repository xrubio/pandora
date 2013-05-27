

#ifndef __HGMind_hxx__
#define __HGMind_hxx__


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
		
		Engine::Raster & _LRResourceRaster;
		
	public:
		HGMind(GujaratWorld & w);
		//HGMind(int x);
		
		//int read();
		Engine::Raster & getLRResourcesRaster() 
		{ 
			return _LRResourceRaster;
		}
		
		void createHRSectors( 	const Engine::Point2D<int>& agentPos,
										std::vector< Sector* >& HRSectors) const;
										
		
		void createLRSectors( 	const Engine::Point2D<int>& agentPos, 
								std::vector< Sector* >& LRSectors ) const;
		
		const std::vector<Sector *> & getHRSectors()const{return _HRSectors;}
		const std::vector<Sector *> & getLRSectors()const{return _LRSectors;}
								
		void updateKnowledge(Engine::Point2D<int> position);
		
		void	updateKnowledge( const Engine::Point2D<int>& agentPos
								, const Engine::Raster& dataRaster
								, std::vector< Sector* >& HRSectors
								, std::vector< Sector* >& LRSectors  ) const;	
				
		void clearSectorKnowledge();
		
		virtual void updateDueToExecuteAction(Sector*) = 0;
	
};
}

#endif