#include <HGPartialMindMap.hxx>							   

#include <GujaratWorld.hxx>
#include <Sector.hxx>
#include <GujaratConfig.hxx>

namespace Gujarat{

HGPartialMindMap::HGPartialMindMap(GujaratWorld & w):
	HGMind(w)
{ 
	int LowResRasterSideSize = w.getLowResMapsSideSize();
	Engine::Point2D<int> lowResSize2D( LowResRasterSideSize, LowResRasterSideSize);
	
	
	_LRResourceRaster = *(new Engine::Raster());
	_LRResourceRaster.setInitValues(0, std::numeric_limits<int>::max(), 0);
	_LRResourceRaster.resize( lowResSize2D );
	setGuessHeuristicResourceRaster();
	
	_LRTimeStamps.setInitValues(0, std::numeric_limits<int>::max(), 0);
	_LRTimeStamps.resize( lowResSize2D );
	initTimeStampRaster();
}

HGPartialMindMap::~HGPartialMindMap()
{
	//~HGMind::HGMind();
}

void HGPartialMindMap::setGuessHeuristicResourceRaster()
{
	int minValue =  _world.getDynamicRaster(eResources).getCurrentMinValue();
	int maxValue =  _world.getDynamicRaster(eResources).getCurrentMaxValue();
	int averageValueHR = (maxValue+minValue)/2;
	
	for(int i = 0; i < _LRResourceRaster.getSize()._x;i++)
	{
		for(int j = 0; j < _LRResourceRaster.getSize()._y;j++)
		{	
			Engine::Point2D<int> p(i,j);
			int n = _world.getValueLR(LRCounterSoilINTERDUNE,p);
			int val = n*averageValueHR;
			_LRResourceRaster.setInitValue(p,n*averageValueHR);
		}
	}
	
}

void HGPartialMindMap::initTimeStampRaster()
{
	// called on creation of the agent
	// called in MoveHomeAction, before calling  HGPartialMindMap::updateKnowledgeFromFirstHandVisit
	int t = _world.getCurrentTimeStep();
	
	//_world.fillLRRaster(_LRTimeStamps,t);
	for(int i = 0; i < _LRTimeStamps.getSize()._x;i++)
	{
		for(int j = 0; j < _LRTimeStamps.getSize()._y;j++)
		{
			Engine::Point2D<int> p(i,j);			
			_LRTimeStamps.setInitValue(p,t);
		}
	}	
}

void HGPartialMindMap::knowledgeExpirationDateProcess()
{
	// HGPartialMindMap::updateKnowledge calls it
	
	//int numSectors = ((GujaratConfig)_world.getConfig())._numSectors;
	
	int minValue =  _world.getDynamicRaster(eResources).getCurrentMinValue();
	int maxValue =  _world.getDynamicRaster(eResources).getCurrentMaxValue();
	int averageValueHR = (maxValue+minValue)/2;
	int n;
	
	// Some sectors where empty at creation and where removed
	//for ( unsigned k = 0; k < numSectors; k++ )
	for ( unsigned int k = 0; k < _LRSectors.size(); k++ )
	{
		const std::vector< Engine::Point2D<int>* > & cells = _LRSectors[k]->cells();
		for( unsigned int i = 0; i < cells.size(); i++)
		{
			int ts = _LRTimeStamps.getValue(*cells[i]);
			if ( _world.getCurrentTimeStep() - ts 
				>= 
				((GujaratConfig) _world.getConfig())._hunterGathererInformationCaducityTime )
			{
				n = _world.getValueLR(LRCounterSoilINTERDUNE,*cells[i]);
				_LRResourceRaster.setInitValue(*cells[i],n*averageValueHR);
				_LRTimeStamps.setInitValue(*cells[i],_world.getCurrentTimeStep());
			}
			else
			{
				_LRTimeStamps.setInitValue(*cells[i],ts+1);
			}
		}
	}
}	

void HGPartialMindMap::updateKnowledge(Engine::Point2D<int> position)
{	
	knowledgeExpirationDateProcess();
	
	HGMind::updateKnowledge(position);
}

void HGPartialMindMap::updateDueToExecuteAction(Sector *s)
//void HGPartialMindMap::updateSectorInfoFromFirstHandVisit(Sector *s)
{
	// Call it from any action that makes agent visit some sector :
	// ForageAction : 
	//		foraged sector, 1 call to updateSectorInfoFromFirstHandVisit
	// MoveHomeAction : 
	//		Chosen LRcell, query which sector belongs to, 1 call to updateSectorInfoFromFirstHandVisit.
	//		Erases all sectors
	//		updateKnowledge after creating sectors should do an init call for each
	//		sector
	
	
	
	
	if (s != 0)
	{
		const std::vector< Engine::Point2D<int>* > & cells = s->cells();	
		for(unsigned int i=0; i < cells.size(); i++)
		{
			_LRTimeStamps.setInitValue(*(cells[i]),_world.getCurrentTimeStep());
			int res = _world.getValueLR(eLRResources,*cells[i]);
			_LRResourceRaster.setInitValue(*(cells[i]),res);
		}
	}
	
	
	
}

/*
void HGPartialMindMap::clearSectorKnowledge(Engine::Point2D<int> LRHomeLocationOLD,Engine::Point2D<int> LRHomeLocationNEW)
{
	
	// update new knowledge due to new location.
	// homeLocation is the old location of the agent in LR ressolution
	
	//std::cout << "HGPartialMindMap::clearSectorKnowledge" << std::endl;
	
	//init?? cal?
	//initPrivateLRRasters();	
	
	Engine::Point2D<int> relativeNewHome = LRHomeLocationNEW - LRHomeLocationOLD;
	
	int indexSector = GujaratState::sectorsMask(relativeNewHome._x + _lowResHomeRange
												,relativeNewHome._y + _lowResHomeRange
												,GujaratState::getLRSectorsMask());
	
	updateResRasterFromFirstHandVisit(_LRSectors[indexSector]);	
	
	HGMind::clearSectorKnowledge();	
	
	// Now it is needed : recalc sectors, recalc their utility taking profit 
	// of the new adquired info in _LRResourceRaster
	// --> waiting for updateKnowledge() to be called.
}
*/

}
