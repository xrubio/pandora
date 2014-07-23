#include <HunterGathererMDPConfig.hxx>
#include <tinyxml.h>
#include <cstdlib>
#include <string>
#include <iostream>

namespace Gujarat
{

HunterGathererMDPConfig::HunterGathererMDPConfig() : _nrForageActions( 2 ), _nrMoveHomeActions( 2 ), _doNothingAllowed( true ), _horizon( 7 )
{
}

HunterGathererMDPConfig::~HunterGathererMDPConfig()
{
}

}

