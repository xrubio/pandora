
#include <analysis/RasterSum.hxx>
#include <DynamicRaster.hxx>

namespace PostProcess
{

RasterSum::RasterSum() : RasterAnalysis("Sum")
{
}

RasterSum::~RasterSum()
{
}

void RasterSum::computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory )
{
	for(unsigned r=0; r<_results.size(); r++)
	{
		const Engine::DynamicRaster & raster = rasterHistory.at(r);
		for(int i=0; i<raster.getSize()._width; i++)
		{
			for(int j=0; j<raster.getSize()._height; j++)
			{
				_results.at(r) += raster.getValue(Engine::Point2D<int>(i,j));
			}
		}
	}
}

} // namespace PostProcess


