
#ifndef __RasterMean_hxx__
#define __RasterMean_hxx__

#include <analysis/Analysis.hxx>

namespace Engine
{
	class SimulationRecord;
}

namespace PostProcess
{

class RasterMean : public RasterAnalysis
{
	long int _numCells;
public:
	RasterMean();
	virtual ~RasterMean();
	void computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory );

	void postProcess();
};

} // namespace PostProcess

#endif // __RasterMean_hxx__

