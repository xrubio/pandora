#ifndef __ALEXIS_DEMOGRAPHICS__
#define __ALEXIS_DEMOGRAPHICS__

#include <GujaratDemographics.hxx>

namespace Gujarat
{

class AlexisDemographics : public GujaratDemographics
{
public:

	AlexisDemographics();
	virtual	~AlexisDemographics();
	
//	virtual bool checkEmigration (GujaratAgent& agent);
	virtual void checkMortality (GujaratAgent& agent);
	virtual void checkReproduction (GujaratAgent& agent);
//	virtual void checkStarvationMortality (GujaratAgent& agent);
};


}

#endif // AlexisDemographics.hxx
