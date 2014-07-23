
#include <analysis/IndividualStats.hxx>

#include <AgentRecord.hxx>
#include <Exception.hxx>
#include <SimulationRecord.hxx>
#include <sstream>
#include <boost/filesystem.hpp>

namespace PostProcess 
{
	
IndividualStats::IndividualStats( int numStep, const std::string & separator ) : Output(separator), _numStep(numStep)
{
}

IndividualStats::~IndividualStats()
{
}

void IndividualStats::addAttribute( const std::string & attribute )
{
	_attributes.push_back(attribute);
}

void IndividualStats::preProcess(  const Engine::SimulationRecord & , const std::string & outputFile  )
{
	// check if directory exists
	unsigned int filePos = outputFile.find_last_of("/");
	std::string path = outputFile.substr(0,filePos+1);
	if(path.compare("")!=0)
	{
		// create dir where data will be stored if it is not already created
		boost::filesystem::create_directory(path);
	}
	// delete the file if it already exists
	boost::filesystem::remove(outputFile);
	
	_file.open(outputFile.c_str());
	std::stringstream header;
	header << "id";

	for(std::list<std::string>::iterator it=_attributes.begin(); it!=_attributes.end(); it++)
	{
		header << _separator << *it;
	}
	_file << header.str() << std::endl;;
}

void IndividualStats::postProcess( const Engine::SimulationRecord & , const std::string & )
{
	_file.close();
}

bool IndividualStats::attributeToStore( const std::string & key )
{
	std::list<std::string>::iterator it = std::find(_attributes.begin(), _attributes.end(), key);
	if(it==_attributes.end())
	{
		return false;
	}
	return true;
}

void IndividualStats::computeAgent( const Engine::AgentRecord & agentRecord )
{
	if(agentRecord.getInt(_numStep, "exists")==0)
	{
		return;
	}

	std::stringstream newLine;
	newLine << agentRecord.getId();
	for(std::list<std::string>::iterator it=_attributes.begin(); it!=_attributes.end(); it++)
	{
		const std::string & key = *it; 
        if(agentRecord.isInt(key))
        {
		    newLine << _separator << agentRecord.getInt(_numStep, key);
        }
        else if(agentRecord.isFloat(key))
        {  
		    newLine << _separator << agentRecord.getFloat(_numStep, key);
        }
	}
	_file << newLine.str() << std::endl;
}

std::string IndividualStats::getName() const
{
	return "Individual stats";
}

} // namespace PostProcess


