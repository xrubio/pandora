
#include "Academia.hxx"

#include "AcademiaConfig.hxx"
#include "Paper.hxx"
#include "Author.hxx"
#include <Simulation.hxx>
#include <Raster.hxx>
#include <Point2D.hxx>
#include <Agent.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>

namespace Examples 
{

Academia::Academia( Engine::Simulation & simulation, const AcademiaConfig & config ) : World(simulation, config._citationRadius+1, false, config._resultsFile), _newPaperKey(0), _newAuthorKey(0), _config(config)
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
	for(int i=0; i<_config._numInitialPapers; i++)
	{
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{			
			std::ostringstream oss;
			oss << "Paper_" << _newPaperKey;
			std::ostringstream ossAuthor;
			ossAuthor << "Author_" << _newAuthorKey;

			Author * newAuthor = new Author(ossAuthor.str(), Engine::GeneralState::statistics().getUniformDistValue(0, _config._researcherLife));
			addAgent(newAuthor);
			newAuthor->setPosition(Engine::Point2D<int>(0,0));
			newAuthor->incrementNumPapers();
			
			Paper * paper = new Paper(oss.str(), 0);
			addAgent(paper);
			paper->setRandomPosition();
			paper->setAuthor(newAuthor);
			paper->setProbabilityNewPaper(_config._probabilityNewPaper);

			_newPaperKey++;
			_newAuthorKey++;
		}
	}
}
	
void Academia::generateNewPaper( Paper & paper )
{
	std::stringstream logName;
	logName << "NewPapers_" << getId();

	std::ostringstream oss;
	oss << "Paper_" << _newPaperKey;

	Engine::World::AgentsVector potentialCitations = getNeighbours(&paper, _config._citationRadius, "Paper");
	
	float influence = 0.0f;
	Engine::Point2D<int> newPosition = paper.getPosition();
	Engine::World::AgentsVector::iterator it = potentialCitations.begin();
	log_DEBUG( logName.str(), "generating new paper from original: " << paper);

	Engine::World::AgentsVector citedPapers;
	while(it!=potentialCitations.end() && influence < 1.0f)
	{
		influence += Engine::GeneralState::statistics().getExponentialDistValue(0.0f, 1.0f);
		Engine::Agent * agent = *it;
		newPosition._x += (agent->getPosition()._x - paper.getPosition()._x)*((1.0f-influence)/2);
		newPosition._y += (agent->getPosition()._y - paper.getPosition()._y)*((1.0f-influence)/2);
		log_DEBUG( logName.str(), "\tnew kene to: " << newPosition << " influenced by paper: " << agent << " with influence : " << influence);
		citedPapers.push_back(agent);
		it++;
	}

	if(paper.getPosition().distance(newPosition)<_config._originalityThreshold)
	{
		log_DEBUG( logName.str(), "originality threshold: " << _config._originalityThreshold << " not arrived, with original kene: " << paper.getPosition() << ",  new paper kene: " << newPosition << " and distance: " << paper.getPosition().distance(newPosition));
		return;
	}


	for(int i=0; i<citedPapers.size(); i++)
	{
		Paper * citedPaper = (Paper*)citedPapers.at(i);
		citedPaper->incrementNumCitations();
		Author * author = citedPaper->getAuthor();
		author->incrementNumCitations();
	}

	log_DEBUG( logName.str(), "paper with kene: " << newPosition << " generated from original: " << paper << " with distance: " << paper.getPosition().distance(newPosition));
	
	Paper * newPaper = 0;
	
	float randomValue = Engine::GeneralState::statistics().getUniformDistValue(0, 100000);
	randomValue /= 100000;

	Author * newAuthor = 0;
	if(!paper.getAuthor()->isActive() || randomValue<_config._probabilityNewAuthor)
	{
		std::ostringstream ossAuthor;
		ossAuthor << "Author_" << _newAuthorKey;
		_newAuthorKey++;
		newAuthor = new Author(ossAuthor.str(), Engine::GeneralState::statistics().getUniformDistValue(0, _config._researcherLife));
		addAgent(newAuthor);
		newAuthor->setPosition(Engine::Point2D<int>(0,0));
		log_DEBUG( logName.str(), "paper with kene: " << newPosition << " has new author: " << newAuthor);
	}
	else
	{
		newAuthor = paper.getAuthor();
		log_DEBUG( logName.str(), "paper with kene: " << newPosition << " has old author: " << newAuthor);
	}
	newAuthor->incrementNumPapers();
	newPaper = new Paper(oss.str(), getCurrentTimeStep());
	newPaper->setCitedPapers(citedPapers.size());
	newPaper->setAuthor(newAuthor);

	newPaper->setProbabilityNewPaper(_config._probabilityNewPaper);
	newPaper->setPosition(newPosition);
	addAgent(newPaper);
	_newPaperKey++;
}


} // namespace Examples

