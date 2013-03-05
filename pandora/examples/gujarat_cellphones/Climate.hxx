#ifndef __Climate_hxx__
#define __Climate_hxx__

#include <boost/random.hpp>
#include <boost/random/gamma_distribution.hpp>

namespace GujaratCellphones {

enum Seasons
{
	HOTWET = 0,
	COLDDRY = 1,
	HOTDRY = 2
};

class MyWorld;
class MyWorldConfig;

class Climate
{	
	typedef boost::mt19937 RandomEngine;
	RandomEngine _randomGenerator;
	Seasons _currentSeason;
	float _currentRain; // mm on heigth of rain
	float _alphaRain;
	float _betaRain;

	boost::variate_generator< RandomEngine, boost::uniform_real<> > _uniformDistribution;
	const MyWorldConfig &_config;
	const MyWorld &_theWorld;	

public:
	Climate(const MyWorldConfig &config, const MyWorld &theWorld);
	virtual ~Climate();

	void step();	
	float getMeanAnnualRain() const;
	const float & getRain() const;
	const Seasons & getSeason() const;
	void advanceSeason();
	bool cellUpdateRequired() const;
	bool rainSeasonStarted() const;
};

}

#endif // __Climate_hxx__

