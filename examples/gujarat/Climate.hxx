
#ifndef __Climate_hxx__
#define __Climate_hxx__

#include <boost/random.hpp>
#include <boost/random/gamma_distribution.hpp>

namespace Gujarat
{

enum Seasons
{
	HOTWET = 0,
	COLDDRY = 1,
	HOTDRY = 2
};

class GujaratWorld;
class GujaratConfig;

class Climate
{	
	typedef boost::mt19937 RandomEngine;
	RandomEngine _randomGenerator;
	Seasons _currentSeason;
	float   _currentRain; // mm on heigth of rain
	float	_alphaRain;
	float	_betaRain;

	boost::variate_generator< RandomEngine, boost::uniform_real<> > _uniformDistribution;

	const GujaratConfig & _config;

	const GujaratWorld &  _theWorld;	

public:
	Climate( const GujaratConfig & config, const GujaratWorld& theWorld );
	virtual ~Climate();

	void step();	
	float getMeanAnnualRain() const;
	const float & getRain() const;
	const Seasons & getSeason() const;

	void advanceSeason();
	bool cellUpdateRequired() const;
	bool rainSeasonStarted() const;
};

} // namespace Gujarat

#endif // __Climate_hxx__

