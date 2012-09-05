
#ifndef NEDCONFIGCLASS
#define NEDCONFIGCLASS 1

#include <iostream>
#include <cstdlib>
#include <tinyxml.h>
#include "Config.hxx"


enum NEDSymbols_enum
    { NEDCONFIG_FILE 
    , NEDCONFIG_UNIFORM 
    , NEDCONFIG_RANDOM  
    , NEDCONFIG_EXPONEN
    , NEDCONFIG_DISTRIB
    , NEDCONFIG_PEAK }; 


enum NEDSimpleTypes_enum
    { NEDCONFIG_INT
    , NEDCONFIG_STR };


namespace NED {
//class NED::NEDWorld;
    class NEDWorld;
    };

class NEDConfig : public Config
  { 
  //private:
  protected:	
//  std::string _path; 	// essential : put it in the superclass
//  int _numAgents;	// essential : put it in the superclass
//  int _numSteps;	// essential : put it in the superclass
  int _size;		
  int _minHomeRange;
  int _maxHomeRange;
  int _minSplitChance;
  int _maxSplitChance;
  int _minCannibalism;
  int _maxCannibalism;  
  // resource map
  std::string _resourceType;
  std::string _distrib;
  int         _peak;
  std::string _resourceFile;
  
  
  int parseResourceInfo(TiXmlElement *pResource,const char *strType);
    
  
//----------------------------------  
  public:

  friend class NED::NEDWorld;
  //friend class NEDWorld;
    
  NEDConfig();  
  NEDConfig(const std::string & filename);  
  virtual ~NEDConfig();
  
  // -- methods -----------------
  void extractParticularAttribs(TiXmlElement *pRoot);
    
  // ----------------------------
  friend std::ostream & operator<<( std::ostream & stream, const NEDConfig & c)
      {
      stream << "Config(" 
			<< "path:" << c.Config::_path
		<< "," << "agents:" << c.Config::_numAgents
		<< "," << "size:" << c._size
		<< "," << "steps:" << c.Config::_numSteps
		<< "," << "homeRange:[" << c._minHomeRange
		<< "," << c._maxHomeRange << "]"
		<< "," << "splitChance:[" << c._minSplitChance
		<< "," << c._maxSplitChance << "]"
		<< "," << "cannibalism:[" << c._minCannibalism
		<< "," << c._maxCannibalism << "]";
		
	if (c._resourceType =="file") 
		    {
		    stream << ",File:" << c._resourceFile << std::endl;
		    }
	else if (c._resourceType == "peak") 
		    {  
		    stream << ",Peak:" << c._peak << std::endl;
		    } 
	else if (c._resourceType == "distrib") 
		    {  
		    stream << ",Distrib:" << c._distrib << std::endl;
		    }
		    
	stream << ")";
      return stream;
      }  
      
  inline int getSize(){ return _size; }
  inline int getNumSteps(){ return _numSteps; }      
  
  };//class
  
  
#endif  

/*
TODO 
config : config assign | assign;

assign : var list_value | var value;

var : var_name type;

value : int | string | bool | list_value;

list_value : list_value value | value;

*/

