#ifndef __GUJARAT_DEMOGRAPHICS__
#define __GUJARAT_DEMOGRAPHICS__

namespace Gujarat
{

class GujaratAgent;

class GujaratDemographics 
{
public:
	GujaratDemographics();
	virtual ~GujaratDemographics();

//	virtual bool	checkEmigration( GujaratAgent & agent ) = 0;	
	virtual	void	checkMortality( GujaratAgent & agent  ) = 0;
	virtual void	checkReproduction( GujaratAgent & agent ) = 0;	
};

}

#endif // GujaratDemographics.hxx
