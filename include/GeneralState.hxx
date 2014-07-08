
#ifndef __GeneralState_hxx__
#define __GeneralState_hxx__

#include <LoggerBase.hxx>
#include <Statistics.hxx>
#include <RasterLoader.hxx>
#include <ShpLoader.hxx>

namespace Engine
{

class GeneralState
{
	static GeneralState * _instance;

	Logger _logger;
	//! random number generator
	Statistics _statistics;
	RasterLoader _rasterLoader;
	ShpLoader _shpLoader;

protected:
	GeneralState();

public:
	static GeneralState & instance();
	virtual ~GeneralState();

	static Logger & logger()
	{
		return instance()._logger;
	}

	static Statistics & statistics()
	{
		return instance()._statistics;
	}

	static RasterLoader & rasterLoader()
	{
		return instance()._rasterLoader;
	}

	static ShpLoader & shpLoader()
	{
		return instance()._shpLoader;
	}
};

} // namespace Engine

#endif // __GeneralState_hxx__

