#ifndef __HunterGathererControllerConfig_hxx__
#define __HunterGathererControllerConfig_hxx__

#include <iosfwd>

class TiXmlElement;

namespace Gujarat
{

class HunterGathererControllerConfig
{
public:
	HunterGathererControllerConfig();
	HunterGathererControllerConfig( TiXmlElement* elem );
	~HunterGathererControllerConfig();

};

}

#endif // HunterGathererControllerConfig.hxx
