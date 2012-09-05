
#ifndef __RasterSum_hxx__
#define __RasterSum_hxx__

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

class RasterSum : public RasterAnalysis
{
public:
	RasterSum();
	virtual ~RasterSum();
	void computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory );
};

} // namespace Analysis

#endif // __RasterSum_hxx__

