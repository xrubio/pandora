
#include <AcademiaConfig.hxx>

namespace Examples
{

AcademiaConfig::AcademiaConfig() : _numInitialPapers(0), _size(0), _probabilityNewAuthor(0.0f), _probabilityNewPaper(0.0f), _originalityThreshold(0), _citationRadius(0), _researcherLife(0)
{
}

AcademiaConfig::~AcademiaConfig()
{
}

void AcademiaConfig::extractParticularAttribs(TiXmlElement * root)
{
	TiXmlElement * element = root->FirstChildElement("numInitialPapers");
	retrieveAttributeMandatory( element, "value", _numInitialPapers);
	element = root->FirstChildElement("size");
	retrieveAttributeMandatory( element, "value", _size);

	element = root->FirstChildElement("probabilityNewAuthor");
	retrieveAttributeMandatory( element, "value", _probabilityNewAuthor);
	element = root->FirstChildElement("probabilityNewPaper");
	retrieveAttributeMandatory( element, "value", _probabilityNewPaper);
	
	element = root->FirstChildElement("originalityThreshold");
	retrieveAttributeMandatory( element, "value", _originalityThreshold);
	element = root->FirstChildElement("citationRadius");
	retrieveAttributeMandatory( element, "value", _citationRadius);

	element = root->FirstChildElement("researcherLife");
	retrieveAttributeMandatory( element, "value", _researcherLife);	
}
	
int AcademiaConfig::getSize() const
{
	return _size;
}

} // namespace Examples

