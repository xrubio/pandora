#ifndef __HunterGathererDecisionTreeConfig_hxx__
#define __HunterGathererDecisionTreeConfig_hxx__

#include <iosfwd>

class TiXmlElement;

namespace Gujarat
{

class HunterGathererDecisionTreeConfig : public HunterGathererControllerConfig
{
public:
	HunterGathererDecisionTreeConfig();
	HunterGathererDecisionTreeConfig( TiXmlElement* elem );
	~HunterGathererDecisionTreeConfig();	
	
	int	getAlpha() const { return _alpha; }
	
	void	dump( std::ostream& os ) const;

private:

	float _alpha; 
	
};

}

#endif // HunterGathererDecisionTreeConfig.hxx
