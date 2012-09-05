
#ifndef __AgentHDFtoSHP_hxx__
#define __AgentHDFtoSHP_hxx__

#include <analysis/Analysis.hxx>

class OGRDataSource;
class OGRLayer;

namespace Engine
{
	class AgentRecord;
}

namespace Analysis
{

class AgentHDFtoSHP : public AgentAnalysis
{
	// if a num step is passed only this step will be stored
	// if not, the entire simulation will be stored
	// one of the fields will be numStep, with the number of step of every agent
	int _numStep;
	std::string _fileName;

	OGRDataSource * _dataSource;
	OGRLayer * _layer;

	// origin of simulation
	Engine::Point2D<int> _origin;
	// resolution of meters of a simulation cell
	float _resolution;
	// spatial reference system (in wkt)
	std::string _srs;

	// this variable is checked to see if definition of shapefile is complete (it will be done with the first agent computed)
	bool _definitionComplete;

	// create a feature for the agent stored in agentRecord in time step timeStep. If storeTimeStep is true, a field called "num step" will be stored.
	void createFeature( const Engine::AgentRecord & agentRecord, int timeStep, bool storeTimeStep );
	// defines the complete list of fields for the agentRecord to be stored
	void defineFields( const Engine::AgentRecord & agentRecord );

	// shortens an attribute to shapefile standard (10 characters without trailing spaces)
	std::string getFieldNameFromString( const std::string & fieldName );
public:
	// generic constructor
	AgentHDFtoSHP( const std::string & fileName, int numStep = -1 );
	// constructor for specifying real coordinates
	AgentHDFtoSHP( const std::string & fileName, const Engine::Point2D<int> & origin, float resolution, const std::string & srs, int numStep = -1 );
	virtual ~AgentHDFtoSHP();
	void preProcess();
	void computeAgent( const Engine::AgentRecord & agentRecord );
	void postProcess();
};

} // namespace Analysis

#endif // __AgentHDFtoSHP_hxx__

