#include <HunterGathererDecisionTreeConfig.hxx>
#include <tinyxml.h>
#include <cstdlib>
#include <string>
#include <iostream>

namespace Gujarat
{

HunterGathererDecisionTreeConfig::HunterGathererDecisionTreeConfig()
	: _alpha( 0.9 )
{
}

HunterGathererDecisionTreeConfig::HunterGathererDecisionTreeConfig( TiXmlElement* elem )
{
	
	_alpha = atof( elem->Attribute( "alpha" ) );	
}

HunterGathererDecisionTreeConfig::~HunterGathererDecisionTreeConfig()
{
}

void	HunterGathererDecisionTreeConfig::dump( std::ostream& os ) const
{
	os << "Hunter Gatherer DecisionTree Controller Config" << std::endl;
	os << "=====================================" << std::endl;
	os << "alpha: " << getAlpha() << std::endl;
	
}

}
