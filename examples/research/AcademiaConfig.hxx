#ifndef __AcademiaConfig_hxx__
#define __AcademiaConfig_hxx__

#include <Config.hxx>
#include <Size.hxx>

namespace Examples
{

class AcademiaConfig : public Engine::Config
{	
	int _numInitialPapers;
    Engine::Size<int> _size;

	float _probabilityNewAuthor;
	float _probabilityNewPaper;

	int _originalityThreshold;
	int _citationRadius;
	int _researcherLife;
public:
	AcademiaConfig();
	virtual ~AcademiaConfig();

	void extractParticularAttribs(TiXmlElement *pRoot);

	const Engine::Size<int> & getSize() const;

	friend class Academia;
};

} // namespace Examples

#endif // __AcademiaConfig_hxx__

