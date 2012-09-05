
#ifndef __SugarConfig_hxx__
#define __SugarConfig_hxx__

#include "Config.hxx"
#include <tinyxml.h>
#include <string>
#include <sstream>

#include "Exceptions.hxx"



/*
enum SUGSymbols_enum
	{ SUGCONFIG_FILE 
	, SUGCONFIG_UNIFORM 
	, SUGCONFIG_RANDOM  
	, SUGCONFIG_EXPONEN
	, SUGCONFIG_DISTRIB
	, SUGCONFIG_PEAK }; 


enum SUGSimpleTypes_enum
	{ SUGCONFIG_INT
	, SUGCONFIG_STR };
*/

namespace Engine
{
	class Config;	
}


namespace SUG 
{
	class SugarWorld;
	class SugarAgent;

	

class SugarConfig : public Config
{ 
	int _numAgents;
	int _size;		
	
	// AGENT CONFIG
	int _agentMinEndowment;
	int _agentMaxEndowment;
	int _agentMinVision;
	int _agentMaxVision;
	int _agentMinDeathAge;
	int _agentMaxDeathAge;
	int _agentMinMetabolism;
	int _agentMaxMetabolism;
	int _agentMinBeginChildBearing;
	int _agentMaxBeginChildBearing;	
	int _agentMinPausiaAgeMen;
	int _agentMaxPausiaAgeMen;	
	int _agentMinPausiaAgeWomen;
	int _agentMaxPausiaAgeWomen;	
	
	// RULES
	bool _growBackRule;
	int  _growBackAlpha;
	bool _seasonalChangeRule;
	int  _seasonalChangeStep;
	int  _seasonalChangeAlpha;
	int  _seasonalChangeBeta;	
	bool _replacementRule;
	int  _replacementAlphaMin;
	int  _replacementAlphaMax;
	bool _matingRule;
	bool _pollutionFormation;
	bool _pollutionDiffusion;
	int  _pollutionDiffusionAlpha;
	int  _pollutionFormationAlpha;
	int  _pollutionFormationBeta;	
	
	// resource map
	std::string _resourceType;
	std::string _distrib;
	int         _peak;
	std::string _resourceFile;
	std::string _path;
  
  
	int parseResourceInfo(TiXmlElement *pResource,const char *strType);
	int parseAgentInfo(TiXmlElement *pResource);
	int parseRulesInfo(TiXmlElement *pRoot);
//----------------------------------  
	public:

	friend class SugarWorld;
	friend class SugarAgent;	
    
	SugarConfig();  
	SugarConfig(const std::string & filename);  
	virtual ~SugarConfig();
  
	// -- methods -----------------
	void extractParticularAttribs(TiXmlElement *pRoot);
    
	// ----------------------------
	friend std::ostream & operator<<( std::ostream & stream, const SugarConfig & c)
	{
		stream << "Config(" 
					<< "path:" << c._path
			<< "," << "agents:" << c._numAgents
			<< "," << "size:" << c._size
			<< "," << "steps:" << c._numSteps;
		
		if (c._resourceType =="file") 
		{
			stream << ",File:" << c._resourceFile << std::endl;
		}		
		
		stream << ")";
		return stream;
	}  
      
	inline int getSize(){ return _size; }
	inline int getNumSteps(){ return _numSteps; }      
  
};//class
  
}//namespace  

#endif		// __SugarConfig_hxx__  

/*
TODO 
config : config assign | assign;

assign : var list_value | var value;

var : var_name type;

value : int | string | bool | list_value;

list_value : list_value value | value;

*/

