#ifndef __AcademiaConfig_hxx__
#define __AcademiaConfig_hxx__

#include <Config.hxx>

namespace Examples
{

class AcademiaConfig : public Engine::Config
{	
	int _numInitialPapers;

	float _probabilityNewAuthor;
	float _probabilityNewPaper;

	float _originalityThreshold;
	float _citationRadius;
	int _researcherLife;
public:
	AcademiaConfig( const std::string & xmlFile );
	virtual ~AcademiaConfig();

	void loadParams(); 
	friend class Academia;
};

} // namespace Examples

#endif // __AcademiaConfig_hxx__

