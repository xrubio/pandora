
#include <AcademiaConfig.hxx>

namespace Examples
{

AcademiaConfig::AcademiaConfig( const std::string & xmlFile ) : Config(xmlFile), _numInitialPapers(0), _probabilityNewAuthor(0.0f), _probabilityNewPaper(0.0f), _originalityThreshold(0), _citationRadius(0), _researcherLife(0)
{
}

AcademiaConfig::~AcademiaConfig()
{
}

void AcademiaConfig::loadParams()
{
    _numInitialPapers = getParamInt("research", "initialPapers");
    _probabilityNewAuthor = getParamFloat("research", "newAuthor");
    _probabilityNewPaper = getParamFloat("research", "newPaper");
    _originalityThreshold = getParamFloat("research", "originality");
    _citationRadius = getParamFloat("research", "radius");
    _researcherLife = getParamInt("research", "researcherLife");
}

} // namespace Examples

