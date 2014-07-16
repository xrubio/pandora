
#include <Climate.hxx>
#include <GujaratConfig.hxx>
#include <GujaratWorld.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>

namespace Gujarat
{

Climate::Climate( const GujaratConfig & config, const GujaratWorld& theWorld ) 	
	: _randomGenerator((long int)config._climateSeed), _currentSeason(HOTDRY),  
	_currentRain(0.0f),
	_uniformDistribution(_randomGenerator, boost::uniform_real <> (0,1)), 
	_alphaRain ( 	(config._rainHistoricalDistribMean/config._rainHistoricalDistribStdev)
	*(config._rainHistoricalDistribMean/config._rainHistoricalDistribStdev)  ),
	_betaRain ( config._rainHistoricalDistribMean
				/
			(config._rainHistoricalDistribStdev*config._rainHistoricalDistribStdev) ),
	_config(config), _theWorld( theWorld )
{
}

Climate::~Climate()
{
}

bool Climate::cellUpdateRequired() const 
{ 
	return _theWorld.getCurrentStep() % _config._daysPerSeason == 0; 
}

void Climate::step()
{
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

bool Climate::rainSeasonStarted() const
{
	return cellUpdateRequired() && _currentSeason == HOTWET;
}

void Climate::advanceSeason()
{
	_currentSeason = (Seasons)((_theWorld.getCurrentStep() / _config._daysPerSeason) % 3);
}

float Climate::getMeanAnnualRain() const
{
	return _config._rainHistoricalDistribMean;
}

const float & Climate::getRain() const
{
	return _currentRain;
}

const Seasons & Climate::getSeason() const
{
	return _currentSeason;
}

} // namespace Gujarat

