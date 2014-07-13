
#include "Author.hxx"

namespace Examples
{

Author::Author( const std::string & id, int researchLifeMaximum ) : Agent(id), _researchLife(0), _researchLifeMaximum(researchLifeMaximum), _numPapers(0), _numCitations(0)
{
}

Author::~Author()
{
}

void Author::updateState()
{
	if(_researchLife<_researchLifeMaximum)
	{
		_researchLife++;
	}
}

void Author::registerAttributes()
{
	registerIntAttribute("papers");
	registerIntAttribute("citations");
}

void Author::serialize()
{
	serializeAttribute("papers", _numPapers);
	serializeAttribute("citations", _numCitations);
}

bool Author::isActive() const
{
	if(_researchLife<_researchLifeMaximum)
	{
		return true;
	}
	return false;
}

void Author::incrementNumPapers()
{
	_numPapers++;
}

void Author::incrementNumCitations()
{
	_numCitations++;
}

} // namespace Examples

