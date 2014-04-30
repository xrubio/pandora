#include "Climate.hxx"
#include "HerderWorldConfig.hxx"

namespace GujaratCellphones {

Climate::Climate(const HerderWorldConfig & config )
{
	float mean = config._rainHistoricalDistribMean;
	float stddev = config._rainHistoricalDistribStdDev;

	_alphaRain = (mean*mean)/(stddev*stddev);
	_betaRain = mean/(stddev*stddev);
    _currentRain = mean;
}

Climate::~Climate() {
}

void Climate::computeRainValue()
{
	boost::gamma_distribution<> gd(_alphaRain);
	boost::variate_generator<boost::mt19937&, boost::gamma_distribution<> >
	var_gamma(_randomGenerator, gd);
	_currentRain = var_gamma()*1.0/_betaRain;
}

const float & Climate::getRain() const {
	return _currentRain;
}

}
