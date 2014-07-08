
#ifndef __RasterSum_hxx__
#define __RasterSum_hxx__

#include <analysis/Analysis.hxx>
#include <SimulationRecord.hxx>

namespace Engine
{
	class SimulationRecord;
}

namespace PostProcess
{

class RasterSum : public RasterAnalysis
{
public:
	RasterSum();
	virtual ~RasterSum();
	void computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory );
};

} // namespace PostProcess

#endif // __RasterSum_hxx__

