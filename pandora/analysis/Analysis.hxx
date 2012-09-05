
#ifndef __Analysis_hxx__
#define __Analysis_hxx__

#include <string>
#include <vector>

#include <SimulationRecord.hxx>

namespace Engine
{
	class AgentRecord;
}

namespace Analysis
{

class Analysis
{
protected:
	std::string _name;
	std::vector<float> _results;
	// if _writeResults is false _results will not be written in csv file
	// it is used for analysis that produce other results (i.e. AgentHDFtoSHP parser)
	bool _writeResults;
public:
	Analysis( const std::string & name, bool writeResults = true );
	virtual ~Analysis();
	void setNumTimeSteps( int numTimeSteps );

	const std::string & getName() const;
	virtual void preProcess(){};
	virtual void postProcess(){};
	float getResult( int timeStep ) const;
	bool writeResults(){return _writeResults;}	
};

class RasterAnalysis : public Analysis
{
public:
	RasterAnalysis( const std::string & name, bool writeResults = true ) : Analysis(name, writeResults)
	{
	}
	virtual ~RasterAnalysis(){}
	virtual void computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory ) = 0;
};


class AgentAnalysis : public Analysis
{
public:
	AgentAnalysis( const std::string & name, bool writeResults = true ) : Analysis(name, writeResults)
	{
	}
	virtual void computeAgent( const Engine::AgentRecord & ) = 0;
};


} // namespace Analysis

#endif // __Analysis_hxx__
