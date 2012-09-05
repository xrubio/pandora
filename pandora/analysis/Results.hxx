
#ifndef __Results_hxx__
#define __Results_hxx__

#include <list>
#include <string>

namespace Engine
{
	class SimulationRecord;
}

namespace Analysis
{

class Analysis;
class AgentAnalysis;

class Results
{
protected:
	typedef std::list<Analysis *> AnalysisList;
	AnalysisList _analysisList;
	const Engine::SimulationRecord & _simRecord;

	std::string _outputFile;
	std::string _type;
	std::string _separator;
	

public:
	Results( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type, const std::string & separator );	
	virtual ~Results();

	void apply() const;
	void addAnalysis( Analysis * analysis );
	
	// method implemented in children (raster, agents, ...)
	virtual void concreteApply() const = 0;
};

class AgentResults : public Results
{
	void concreteApply() const;
public:
	AgentResults( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type, const std::string & separator = ";" );
	virtual ~AgentResults(){}
};

class RasterResults : public Results
{
	void concreteApply() const;
public:
	RasterResults( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type, const std::string & separator = ";" );
	virtual ~RasterResults(){}
};

} // namespace Analysis

#endif // __Results_hxx__

