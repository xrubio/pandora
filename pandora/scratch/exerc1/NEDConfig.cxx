

#include "NEDConfig.hxx"
#include "Exceptions.hxx"
#include <sstream>
#include <string.h>

//**********************************************************
  NEDConfig::NEDConfig() 
      :_size(0)
      ,_minHomeRange(-1)
      ,_maxHomeRange(-1)
      ,_minSplitChance(-1)
      ,_maxSplitChance(-1)
      ,_minCannibalism(-1)
      ,_maxCannibalism(-1)      
      ,_peak(-1)      
      {
//       Config::_path 	  = (char*)0;
//       Config::_numAgents = 0;
//       Config::_numSteps  = 0;
      _resourceType = "";
      _distrib      = "";
      _resourceFile = "";      
      }
  
//**********************************************************
//**********************************************************
  NEDConfig::NEDConfig(const std::string & filename)  
    : Config()
    , _size(0)
    , _minHomeRange(0)
    , _maxHomeRange(0)
    , _minSplitChance(-1)
    , _maxSplitChance(-1)
    , _minCannibalism(-1)
    , _maxCannibalism(-1)
    ,_peak(-1)    
    {     
    
    std::cout << "NEDConfig::NEDConfig(filename)" << std::endl;
    
//    Config::_path      = (char*)0;
//    Config::_numAgents = 0;
//    Config::_numSteps  = 0;
    
    std::cout << "lala" << std::endl;
    
    _resourceType = "";
    _distrib      = "";
    _resourceFile = "";      
      
    //deserializeTemplate(this, filename); 
    //deserialize(filename); 
    };
//**********************************************************
  NEDConfig::~NEDConfig() {}
//**********************************************************
//**********************************************************
//**********************************************************
  void NEDConfig::extractParticularAttribs(TiXmlElement *pRoot)
    {
    TiXmlElement *pParm;


      pParm = pRoot->FirstChildElement("HomeRange");
      _minHomeRange = atoi(pParm->Attribute("min"));
      _maxHomeRange = atoi(pParm->Attribute("max")); 	
      pParm = pRoot->FirstChildElement("SplitChance");
      _minSplitChance = atoi(pParm->Attribute("min"));
      _maxSplitChance = atoi(pParm->Attribute("max"));
      pParm = pRoot->FirstChildElement("Cannibalism");
      _minCannibalism = atoi(pParm->Attribute("min"));
      _maxCannibalism = atoi(pParm->Attribute("max"));

      pParm = pRoot->FirstChildElement("resource_map");
      parseResourceInfo(pParm,pParm->Attribute("type"));


      // **********************************************
      // TODO destroy pRoot and pParm
    }    
//**********************************************************
  int NEDConfig::parseResourceInfo(TiXmlElement *pResource,const char *strType)
    {    
// *? mirar carregador 3d per canviar de strcmp a STL    

    //*? use SWITCH
    
    TiXmlElement *pParm = pResource->FirstChildElement(strType);
    
    //_resourceType = "file";
    _resourceType = strType;
    
    if ( pParm )
      {
      if (!strcmp(strType,"file")) 
	  {        
	  _resourceFile  = pParm->Attribute("value"); // path
	  TiXmlElement *pParm2;
	  pParm2 = pResource->FirstChildElement("size");
	  char* strend;
	  _size  = atoi(pParm2->Attribute("value"));
	  return 1;
	  }
      else if (!strcmp(strType,"peak")) 
	  {	  
	  _peak = atoi(pParm->Attribute("value")); // int
	  TiXmlElement *pParm2;
	  pParm2 = pResource->FirstChildElement("size");
	  _size  = atoi(pParm2->Attribute("value"));	
	  return 1;
	  } 
      else if (!strcmp(strType,"distrib")) 
	  {	  
	  _distrib  = pParm->Attribute("value"); // uniform, random, exp
	  TiXmlElement *pParm2;
	  pParm2 = pResource->FirstChildElement("size");
	  _size  = atoi(pParm2->Attribute("value"));	  
	  return 1;
	  } 
      }
    std::stringstream oss;
    oss << "Undefined resource map type : " << strType << std::endl;
    throw Engine::Exception(oss.str());	
    }
//**********************************************************
    /*
    extract scenario:
    
    <resource_map type="AAA" }>
	<AAA value="BBB"/>
    </resource_map>
    
    AAA={raster_file,distrib,peak}

    TODO:

    <resource_map type="CCC" }>
	<CCC value="DDD"/>
	<CCC value="EEE"/>
	<CCC value="FFF"/>
	<CCC value="GGG"/>
	<CCC value="HHH"/>
    </resource_map>
    
    */
  
//**********************************************************

/* USAGE EXAMPLE :
int main(int argc, char *argv[])
  {
  std::cout << "begin tinyxml test" << std::endl;
  //std::stringstream oss;  
  config c("config.xml");  
    
  std::cout << "config>" << c << std::endl;
  }
*/  
  
  
  