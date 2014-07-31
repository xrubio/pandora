
#include "Academia.hxx"

#include "AcademiaConfig.hxx"
#include "Paper.hxx"
#include "Author.hxx"
#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <Agent.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <typedefs.hxx>

namespace Examples 
{

Academia::Academia( AcademiaConfig * config,Engine::Scheduler * scheduler ) : World(config, scheduler, false), _newPaperKey(0), _newAuthorKey(0)
{
}

Academia::~Academia()
{
}

void Academia::createRasters()
{
}

void Academia::createAgents()
{
    const AcademiaConfig & academiaConfig = (const AcademiaConfig &)getConfig();
	for(int i=0; i<academiaConfig._numInitialPapers; i++)
	{
		if((i%getNumTasks())==getId())
		{			
			std::ostringstream oss;
			oss << "Paper_" << _newPaperKey;
			std::ostringstream ossAuthor;
			ossAuthor << "Author_" << _newAuthorKey;

			Author * newAuthor = new Author(ossAuthor.str(), Engine::GeneralState::statistics().getUniformDistValue(0, academiaConfig._researcherLife));
			addAgent(newAuthor);
			newAuthor->setPosition(Engine::Point2D<int>(0,0));
			
			Paper * paper = new Paper(oss.str(), 0);
			addAgent(paper);
			paper->setRandomPosition();
			paper->setAuthor(newAuthor);
			paper->setProbabilityNewPaper(academiaConfig._probabilityNewPaper);
			newAuthor->incrementNumPapers(paper->getPosition());

			_newPaperKey++;
			_newAuthorKey++;
		}
	}
}
	
void Academia::generateNewPaper( Paper & paper )
{
    const AcademiaConfig & academiaConfig = (const AcademiaConfig &)getConfig();
	std::stringstream logName;
	logName << "NewPapers_" << getId();

	std::ostringstream oss;
	oss << "Paper_" << _newPaperKey;

	Engine::AgentsVector potentialCitations = getNeighbours(&paper, academiaConfig._citationRadius, "Paper");

	float influence = 0.0f;
	Engine::Point2D<int> newPosition = paper.getPosition();
	Engine::AgentsVector::iterator it = potentialCitations.begin();
	log_DEBUG( logName.str(), "generating new paper from original: " << paper);

	Engine::AgentsVector citedPapers;
	while(it!=potentialCitations.end() && influence < 1.0f)
	{
		influence += Engine::GeneralState::statistics().getExponentialDistValue(0.0f, 1.0f);
		Engine::Agent * agent = it->get();
		newPosition._x += (agent->getPosition()._x - paper.getPosition()._x)*((1.0f-influence)/2);
		newPosition._y += (agent->getPosition()._y - paper.getPosition()._y)*((1.0f-influence)/2);
		log_DEBUG( logName.str(), "\tnew kene to: " << newPosition << " influenced by paper: " << agent << " with influence : " << influence);
		citedPapers.push_back(*it);
		it++;
	}

	if(paper.getPosition().distance(newPosition)<academiaConfig._originalityThreshold)
	{
		log_DEBUG( logName.str(), "originality threshold: " << academiaConfig._originalityThreshold << " not arrived, with original kene: " << paper.getPosition() << ",  new paper kene: " << newPosition << " and distance: " << paper.getPosition().distance(newPosition));
		return;
	}

	for(int i=0; i<citedPapers.size(); i++)
	{
		Paper * citedPaper = (Paper*)(citedPapers.at(i).get());
		citedPaper->incrementNumCitations();
		Author * author = citedPaper->getAuthor();
		log_DEBUG( logName.str(), citedPaper << " incrementing num citations");
		author->incrementNumCitations();
		log_DEBUG( logName.str(), author << " incrementing num citations");
	}

	
	Paper * newPaper = 0;
	
	float randomValue = Engine::GeneralState::statistics().getUniformDistValue();
	Author * newAuthor = 0;
	if(!paper.getAuthor()->isActive() || randomValue<academiaConfig._probabilityNewAuthor)
	{
		std::ostringstream ossAuthor;
		ossAuthor << "Author_" << _newAuthorKey;
		_newAuthorKey++;
		newAuthor = new Author(ossAuthor.str(), Engine::GeneralState::statistics().getUniformDistValue(0, academiaConfig._researcherLife));
		addAgent(newAuthor);
		newAuthor->setPosition(Engine::Point2D<int>(0,0));
		log_DEBUG( logName.str(), "paper with kene: " << newPosition << " has new author: " << newAuthor);
	}
	else
	{
		newAuthor = paper.getAuthor();
		log_DEBUG( logName.str(), "paper with kene: " << newPosition << " has old author: " << newAuthor);
	}
	log_DEBUG( logName.str(), newAuthor << " new author");
        
	newPaper = new Paper(oss.str(), getCurrentTimeStep());
	addAgent(newPaper);
	newPaper->setCitedPapers(citedPapers.size());
	newPaper->setAuthor(newAuthor);

	newPaper->setProbabilityNewPaper(academiaConfig._probabilityNewPaper);
	newPaper->setPosition(newPosition);
	newAuthor->incrementNumPapers(newPaper->getPosition());
	log_DEBUG( logName.str(), newPaper << " new paper");
	_newPaperKey++;
}

} // namespace Examples

