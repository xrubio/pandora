
#ifndef __RasterMean_hxx__
#define __RasterMean_hxx__

#include <analysis/Analysis.hxx>
#include <SimulationRecord.hxx>

/*
namespace Engine
{
	class ;
}
*/

namespace Analysis
{

class RasterMean : public RasterAnalysis
{
	int _numCells;
public:
	RasterMean();
	virtual ~RasterMean();
	void computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory );

	void postProcess();
};

} // namespace Analysis

#endif // __RasterMean_hxx__

