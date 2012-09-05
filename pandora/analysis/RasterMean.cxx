
#include <analysis/RasterMean.hxx>
#include <Raster.hxx>
#include <Exceptions.hxx>

namespace Analysis
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
			std::stringstream oss;
			oss << "RasterMean::computeRaster - Raster with history of 0 time steps";
			throw Engine::Exception(oss.str());
		}
		const Engine::Raster & raster = rasterHistory.at(0);
		_numCells = raster.getSize()._x * raster.getSize()._y;
	}

	for(int r=0; r<_results.size(); r++)
	{
		const Engine::Raster & raster = rasterHistory.at(r);
		for(int i=0; i<raster.getSize()._x; i++)
		{
			for(int j=0; j<raster.getSize()._y; j++)
			{
				_results.at(r) += raster.getValue(Engine::Point2D<int>(i,j));
			}
		}
	}
}

void RasterMean::postProcess()
{
	for(int i=0; i<_results.size(); i++)
	{
		if(_numCells>0)
		{
			_results[i] /= _numCells;
		}
		else
		{
			_results[i] = 0.0f;
		}
	}
}

} // namespace Analysis

