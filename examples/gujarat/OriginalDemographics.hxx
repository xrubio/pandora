#ifndef __ORIGINAL_DEMOGRAPHICS__
#define __ORIGINAL_DEMOGRAPHICS__

#include <GujaratDemographics.hxx>

namespace Gujarat
{
class GujaratAgent;

class OriginalDemographics : public GujaratDemographics
{
public:

	OriginalDemographics();
	virtual	~OriginalDemographics();
	
	virtual bool checkEmigration( GujaratAgent& agent );
	virtual void checkMortality( GujaratAgent& agent );
	virtual void checkReproduction( GujaratAgent& agent );
};

}

#endif // OriginalDemographics.hxx
