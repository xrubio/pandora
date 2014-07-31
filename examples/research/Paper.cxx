
#include "Paper.hxx"
#include <Statistics.hxx>
#include <GeneralState.hxx>
#include "Academia.hxx"
#include "Author.hxx"
namespace Examples
{

Paper::Paper( const std::string & id, int timeOfCreation ) : Agent(id), _probabilityNewPaper(0.0f), _author(0), _citations(0), _citedPapers(0), _timeOfCreation(timeOfCreation)
{
}

Paper::~Paper()
{
}

void Paper::registerAttributes()
{
	registerIntAttribute("citations");
	registerIntAttribute("cited papers");
	registerIntAttribute("time of creation");
    registerFloatAttribute("p new paper");
}

void Paper::updateState()
{
	float randomValue = Engine::GeneralState::statistics().getUniformDistValue();
	if(randomValue>_probabilityNewPaper)
	{
		return;
	}
	Academia & academia = (Academia&)getWorldRef();
	academia.generateNewPaper(*this);
}

void Paper::serialize()
{
	serializeAttribute("citations", _citations);
	serializeAttribute("cited papers", _citedPapers);
	serializeAttribute("time of creation", _timeOfCreation);
    serializeAttribute("p new paper", _probabilityNewPaper);
}

void Paper::setProbabilityNewPaper( float probabilityNewPaper )
{
	_probabilityNewPaper = probabilityNewPaper;
}

void Paper::setAuthor( Author * author )
{
	_author = author;
}

Author * Paper::getAuthor()
{
	return _author;
}

void Paper::incrementNumCitations()
{
	_citations++;
}

void Paper::setCitedPapers( int citedPapers )
{
	_citedPapers = citedPapers;
}

} // namespace Examples

