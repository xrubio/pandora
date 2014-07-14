
#include <AcademiaConfig.hxx>

namespace Examples
{

AcademiaConfig::AcademiaConfig() : _numInitialPapers(0), _probabilityNewAuthor(0.0f), _probabilityNewPaper(0.0f), _originalityThreshold(0), _citationRadius(0), _researcherLife(0)
{
}

AcademiaConfig::~AcademiaConfig()
{
}

void AcademiaConfig::extractParticularAttribs(TiXmlElement * root)
{
    TiXmlElement * element = root->FirstChildElement("size");
	retrieveAttributeMandatory( element, "width", _size._width);
	retrieveAttributeMandatory( element, "height", _size._height);

	element = root->FirstChildElement("research");
	retrieveAttributeMandatory( element, "initialPapers", _numInitialPapers);
	retrieveAttributeMandatory( element, "newAuthor", _probabilityNewAuthor);
	retrieveAttributeMandatory( element, "newPaper", _probabilityNewPaper);
	retrieveAttributeMandatory( element, "originality", _originalityThreshold);
	retrieveAttributeMandatory( element, "radius", _citationRadius);
	retrieveAttributeMandatory( element, "researcherLife", _researcherLife);
}
	
const Engine::Size<int> & AcademiaConfig::getSize() const
{
	return _size;
}

} // namespace Examples

