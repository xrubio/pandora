

#include <Climate.hxx>
#include <GujaratConfig.hxx>
#include <GujaratWorld.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>

namespace Gujarat
{

Climate::Climate( const GujaratConfig & config, const GujaratWorld& theWorld ) 	
//	: _randomGenerator(Engine::GeneralState::statistics().getNewSeed()), _currentSeason(HOTDRY),  
	: _randomGenerator((long int)config._climateSeed), _currentSeason(HOTDRY),  
	_currentRain(0.0f),
	_uniformDistribution(_randomGenerator, boost::uniform_real <> (0,1)), 
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
	/*
	World passes its Statistics object. This way random number management is centralized.
	*/

	if ( !cellUpdateRequired() )
		return;

	if(_currentSeason == HOTWET )
	{
		float randomNumber = _uniformDistribution();
		// TODO document weibull distribution
		_currentRain = _config._rainHistoricalDistribScale*pow(-log(1.0-randomNumber),(1.0/_config._rainHistoricalDistribShape));
		std::cout << "new current rain: " << _currentRain << std::endl;
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

