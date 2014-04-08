#include <HunterGathererControllerConfig.hxx>
#include <tinyxml.h>
#include <cstdlib>
#include <string>
#include <iostream>

namespace Gujarat
{

HunterGathererControllerConfig::HunterGathererControllerConfig()
	: _nrForageActions( 2 ), _nrMoveHomeActions( 2 ), _doNothingAllowed( true ),
	_horizon( 7 )
{
}

HunterGathererControllerConfig::HunterGathererControllerConfig( TiXmlElement* elem )
{

}

HunterGathererControllerConfig::~HunterGathererControllerConfig()
{
}

void	HunterGathererControllerConfig::dump( std::ostream& os ) const
{
}

}
