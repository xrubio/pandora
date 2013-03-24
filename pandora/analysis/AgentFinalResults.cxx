
#include "analysis/AgentFinalResults.hxx"
#include <AgentRecord.hxx>
#include <Exceptions.hxx>
#include <boost/filesystem.hpp>

namespace Analysis
{
	
AgentFinalResults::AgentFinalResults( const std::string & fileName, int numStep, const std::string & separator) : AgentAnalysis("Final results parser", false), _numStep(numStep), _fileName(fileName), _separator(separator)
{
}

AgentFinalResults::~AgentFinalResults()
{
}

void AgentFinalResults::addAttribute( const std::string & attribute )
{
	_attributes.push_back(attribute);
}

void AgentFinalResults::preProcess()
{
	// check if directory exists
	unsigned int filePos = _fileName.find_last_of("/");
	std::string path = _fileName.substr(0,filePos+1);
	if(path.compare("")!=0)
	{
		// create dir where data will be stored if it is not already created
		boost::filesystem::create_directory(path);
	}
	// delete the file if it already exists
	boost::filesystem::remove(_fileName);
	
	_file.open(_fileName.c_str());
	std::stringstream header;
	header << "id";

	for(std::list<std::string>::iterator it=_attributes.begin(); it!=_attributes.end(); it++)
	{
		header << _separator << *it;
	}
	_file << header.str() << std::endl;;
}

void AgentFinalResults::postProcess()
{
	_file.close();
}

bool AgentFinalResults::attributeToStore( const std::string & key )
{
	std::list<std::string>::iterator it = std::find(_attributes.begin(), _attributes.end(), key);
	if(it==_attributes.end())
	{
		return false;
	}
	return true;
}

void AgentFinalResults::computeAgent( const Engine::AgentRecord & agentRecord )
{
	if(agentRecord.getState(_numStep, "exists")==0)
	{
		return;
	}

	std::stringstream newLine;
	newLine << agentRecord.getId();
	for(std::list<std::string>::iterator it=_attributes.begin(); it!=_attributes.end(); it++)
	{
		const std::string & key = *it;
		newLine << _separator << agentRecord.getState(_numStep, key);
	}
	_file << newLine.str() << std::endl;
}

} // namespace Analysis

