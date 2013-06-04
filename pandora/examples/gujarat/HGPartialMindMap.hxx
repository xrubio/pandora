
#ifndef __HGPartialMindMap_hxx__
#define __HGPArtialMindMap_hxx__

#include <Raster.hxx>

#include <HGMind.hxx>


namespace Gujarat
{
	class HunterGatherer;
	
	class HGPartialMindMap : public HGMind
	{
		//Engine::Raster _LRTimeStamps;
		
	public:
		HGPartialMindMap(GujaratWorld & w);
		~HGPartialMindMap();
		
		void setGuessHeuristicResourceRaster();
		
		void knowledgeExpirationDateProcess();
		
		void initTimeStampRaster();
		
		void updateKnowledge(Engine::Point2D<int> position);
		
		/*void updateKnowledge( const Engine::Point2D<int>& agentPos
							, const Engine::Raster& dataRaster
							, std::vector< Sector* >& HRSectors
							, std::vector< Sector* >& LRSectors  ) const;	
		*/
		/*
		void	updateKnowledge( const Engine::Point2D<int>& agentPos
								, const Engine::Raster& dataRaster
								, std::vector< Sector* >& HRSectors
								, std::vector< Sector* >& LRSectors
								,std::vector< Engine::Point2D<int> >& HRCellPool
								,std::vector< Engine::Point2D<int> >& LRCellPool ) const;	
		*/
							
		void clearSectorKnowledge();
							
		//void clearSectorKnowledge(Engine::Point2D<int> LRHomeLocationOLD
		//						, Engine::Point2D<int> LRHomeLocationNEW);
									
		void updateDueToExecuteAction(Sector *s);
		
		
	};
}

#endif