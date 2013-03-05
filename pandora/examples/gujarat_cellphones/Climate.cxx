#include "Climate.hxx"
#include <Statistics.hxx>
#include "MyWorld.hxx"
#include "MyWorldConfig.hxx"

namespace GujaratCellphones {

Climate::Climate(const MyWorldConfig &config, const MyWorld &theWorld) 	 
	: _randomGenerator((long int)config.getClimateSeed()), _currentSeason(HOTDRY),  
	_currentRain(0.0f),
	_uniformDistribution(_randomGenerator, boost::uniform_real <> (0,1)),
	_config(config), _theWorld( theWorld ) {
	_alphaRain = (_config.getRainHistoricalDistribMean()/_config.getRainHistoricalDistribStdDev())*(_config.getRainHistoricalDistribMean()/_config.getRainHistoricalDistribStdDev());
	_betaRain = _config.getRainHistoricalDistribMean()/(_config.getRainHistoricalDistribStdDev()*_config.getRainHistoricalDistribStdDev());
}

Climate::~Climate() {
}

bool Climate::cellUpdateRequired() const { 
	return _theWorld.getCurrentStep() % _config.getDaysPerSeason() == 0; 
}

void Climate::step() {
	/*
	World passes its Statistics object. This way random number management is centralized.
	*/

	if ( !cellUpdateRequired() )
		return;

	if(_currentSeason == HOTWET )
	{	
		boost::gamma_distribution<> gd(_alphaRain);
		boost::variate_generator<boost::mt19937&, boost::gamma_distribution<> >
		var_gamma(_randomGenerator, gd);
		_currentRain = var_gamma()*1.0/_betaRain;
	}
}

bool Climate::rainSeasonStarted() const {
	return cellUpdateRequired() && _currentSeason == HOTWET;
}

void Climate::advanceSeason() {
	_currentSeason = (Seasons)((_theWorld.getCurrentStep() / _config.getDaysPerSeason()) % 3);
}

float Climate::getMeanAnnualRain() const {
	return _config.getRainHistoricalDistribMean();
}

const float & Climate::getRain() const {
	return _currentRain;
}

const Seasons & Climate::getSeason() const {
	return _currentSeason;
}

}
