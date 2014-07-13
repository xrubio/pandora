#ifndef __ORIGINAL_DEMOGRAPHICS__
#define __ORIGINAL_DEMOGRAPHICS__

namespace Gujarat
{
class GujaratAgent;

class OriginalDemographics
{
public:

	OriginalDemographics();
	virtual	~OriginalDemographics();
	
	void checkMortality( GujaratAgent& agent );
	void checkReproduction( GujaratAgent& agent );
};

}

#endif // OriginalDemographics.hxx
