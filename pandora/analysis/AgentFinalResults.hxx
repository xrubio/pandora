
#ifndef __AgentFinalResults_hxx__
#define __AgentFinalResults_hxx__

#include <analysis/Analysis.hxx>
#include <list>
#include <iostream>
#include <fstream>
#include <iomanip>

namespace Engine
{
	class AgentRecord;
}

namespace Analysis
{

class AgentFinalResults : public AgentAnalysis
{
	int _numStep;
	std::string _fileName;
	std::string _separator;
	std::ofstream _file;

	std::list<std::string> _attributes;

	bool attributeToStore( const std::string & key );
public:
	// set a list of attributes and store for numStep time step, including agent id
	AgentFinalResults( const std::string & fileName, int numStep, const std::string & separator );
	virtual ~AgentFinalResults();

	void computeAgent( const Engine::AgentRecord & agentRecord );
	void addAttribute( const std::string & attribute );
	void preProcess();
	void postProcess();

};

} // namespace Analysis

#endif // __AgentFinalResults_hxx__

