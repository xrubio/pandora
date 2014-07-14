
#include "Author.hxx"
#include "Paper.hxx"
#include <iostream>

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
    // too old to publish ;-)
	if(_researchLife>_researchLifeMaximum)
	{
        return;
    }

    // position of the authors set to the weighted mean of her papers
    Engine::Point2D<int> position(0,0);
    for( auto pos : _paperPositions)
    {
        position += pos; 
    }
    position._x /= _paperPositions.size();
    position._y /= _paperPositions.size();
    if(getWorld()->checkPosition(position))
    {
        setPosition(position);
    }
    _researchLife++;
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

void Author::incrementNumPapers( const Engine::Point2D<int> & paperPosition)
{
    _paperPositions.push_back(paperPosition);
	_numPapers++;
}

void Author::incrementNumCitations()
{
	_numCitations++;
}

} // namespace Examples

