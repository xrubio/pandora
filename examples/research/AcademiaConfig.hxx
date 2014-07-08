#ifndef __AcademiaConfig_hxx__
#define __AcademiaConfig_hxx__

#include <Config.hxx>

namespace Examples
{

class AcademiaConfig : public Engine::Config
{	
	int _numInitialPapers;
	int _size;

	float _probabilityNewAuthor;
	float _probabilityNewPaper;

	int _originalityThreshold;
	int _citationRadius;
	int _researcherLife;
public:
	AcademiaConfig();
	virtual ~AcademiaConfig();

	void extractParticularAttribs(TiXmlElement *pRoot);

	int getSize() const;

	friend class Academia;
};

} // namespace Examples

#endif // __AcademiaConfig_hxx__

