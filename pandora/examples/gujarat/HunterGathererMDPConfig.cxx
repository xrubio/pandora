#include <HunterGathererMDPConfig.hxx>
#include <tinyxml.h>
#include <cstdlib>
#include <string>
#include <iostream>

namespace Gujarat
{

HunterGathererMDPConfig::HunterGathererMDPConfig()
	: _nrForageActions( 2 ), _nrMoveHomeActions( 2 ), _doNothingAllowed( true ),
	_horizon( 7 )
{
}

HunterGathererMDPConfig::HunterGathererMDPConfig( TiXmlElement* elem )
{
	// MRJ: Let's unmarshall the config right here
	_nrForageActions = atoi( elem->Attribute( "nrForageActions" ) );
	_nrMoveHomeActions = atoi( elem->Attribute( "nrMoveHomeActions" ) );
	std::string boolText = elem->Attribute( "doNothingIsAllowed" );	
	_doNothingAllowed = ( boolText == "true" ? true : false );
	_horizon = atoi( elem->Attribute( "horizon" ) );
	_width = atoi( elem->Attribute( "width" ) );
	_explorationBonus = -1.0f*atof( elem->Attribute( "explorationBonus" ) );
}

HunterGathererMDPConfig::~HunterGathererMDPConfig()
{
}

void	HunterGathererMDPConfig::dump( std::ostream& os ) const
{
	os << "Hunter Gatherer MDP Controller Config" << std::endl;
	os << "=====================================" << std::endl;
	os << "# Forage Actions: " << getNumberForageActions() << std::endl;
	os << "# MoveHome Actions: " << getNumberMoveHomeActions() << std::endl;
	os << "Is DoNothing an available action? " << ( isDoNothingAllowed() ? "yes" : "no" ) << std::endl;
	os << "Horizon (# sim time steps): " << _horizon << std::endl;
	os << "Width: " << _width << std::endl;
	os << "Exploration Bonus: " << _explorationBonus << std::endl;	
}

}
