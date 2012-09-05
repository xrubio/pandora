
#ifndef __AgentHistogram_hxx__
#define __AgentHistogram_hxx__

#include <analysis/Analysis.hxx>

namespace Engine
{
	class AgentRecord;
}

namespace Analysis
{

class AgentHistogram : public AgentAnalysis
{	
	std::string _fileName;
	std::string _attribute;
	int _interval;
	// if a num step is passed the histogram will be generated from it
	// if not, the final result of the simulation will be stored
	int _numStep;
	std::string _separator;

	std::vector<int> _histogram;
public:
	AgentHistogram( const std::string & fileName, const std::string & attribute, int interval = 1, int numStep = -1, const std::string & separator = ";");
	virtual ~AgentHistogram();

	void preProcess();
	void computeAgent( const Engine::AgentRecord & agentRecord );
	void postProcess();
};

} // namespace Analysis

#endif // __AgentHistogram_hxx__

