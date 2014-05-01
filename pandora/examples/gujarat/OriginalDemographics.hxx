#ifndef __GUJARAT_DEMOGRAPHICS__
#define __GUJARAT_DEMOGRAPHICS__

#include <GujaratDemographics.hxx>

namespace Gujarat
{
class GujaratAgent;

class GujaratDemographics 
{
public:

	GujaratDemographics();
	virtual	~GujaratDemographics();
	
	virtual bool checkEmigration( GujaratAgent& agent );
	virtual void checkMortality( GujaratAgent& agent );
	virtual void checkReproduction( GujaratAgent& agent );
};

}

#endif // GujaratDemographics.hxx
