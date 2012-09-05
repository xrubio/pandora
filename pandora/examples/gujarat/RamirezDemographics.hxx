#ifndef __RAMIREZ_DEMOGRAPHICS__
#define __RAMIREZ_DEMOGRAPHICS__

#include <GujaratDemographics.hxx>

namespace Gujarat
{

class GujaratAgent;

class RamirezDemographics : public GujaratDemographics
{
public:

	RamirezDemographics();
	virtual	~RamirezDemographics();
	
	virtual bool checkEmigration( GujaratAgent& agent );
	virtual void checkMortality( GujaratAgent& agent );
	virtual void checkReproduction( GujaratAgent& agent );
};


}

#endif // RamirezDemographics.hxx
