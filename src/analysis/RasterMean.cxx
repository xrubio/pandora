
#include <analysis/RasterMean.hxx>
#include <DynamicRaster.hxx>
#include <Exception.hxx>
#include <Size.hxx>

namespace PostProcess
{

RasterMean::RasterMean() : RasterAnalysis("Mean"), _numCells(0)
{
}

RasterMean::~RasterMean()
{
}

void RasterMean::computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory )
{
	if(_numCells==0)
	{
		if(rasterHistory.size()==0)
		{
			return;
		}
		const Engine::DynamicRaster & raster = rasterHistory.at(0);
		_numCells = raster.getSize()._width * raster.getSize()._height;
	}

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

void RasterMean::postProcess()
{
	for(unsigned i=0; i<_results.size(); i++)
	{
		if(_numCells>0)
		{
			_results[i] = _results.at(i)/_numCells;
		}
		else
		{
			_results.at(i) = 0.0f;
		}
	}
}

} // namespace PostProcess

