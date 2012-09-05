

#include "SugarConfig.hxx"


namespace SUG 
{

//**********************************************************
SugarConfig::SugarConfig() 	
	: _path("./data/")
	, _size(52)
	//, _numAgents(250)
	, _numAgents(50)	 
	, _resourceType("file")		
	, _resourceFile("resources/resources_sugar_52.tiff")
	// AGENT CONFIG
	, _agentMinEndowment(5)
	, _agentMaxEndowment(25)
	, _agentMinVision(1)
	, _agentMaxVision(6)
	, _agentMinDeathAge(60)
	, _agentMaxDeathAge(100)
	, _agentMinMetabolism(1)
	, _agentMaxMetabolism(4)
	, _agentMinBeginChildBearing(12)
	, _agentMaxBeginChildBearing(15)	
	, _agentMinPausiaAgeMen(50)
	, _agentMaxPausiaAgeMen(60)	
	, _agentMinPausiaAgeWomen(40)
	, _agentMaxPausiaAgeWomen(50)	
	// RULES
	, _growBackRule(true)
	, _growBackAlpha(1)
	, _seasonalChangeRule(false)
	, _seasonalChangeStep(50)
	, _seasonalChangeAlpha(1)
	, _seasonalChangeBeta(8)
	, _replacementRule(false)
	, _replacementAlphaMin(60)
	, _replacementAlphaMax(100)
	, _matingRule(false)
	, _pollutionFormation(false)
	, _pollutionDiffusion(false)
	, _pollutionDiffusionAlpha(1)
	, _pollutionFormationAlpha(1)
	, _pollutionFormationBeta(1)
{
	_numSteps = 500;
}
  
//**********************************************************
//**********************************************************
SugarConfig::SugarConfig(const std::string & filename)  
	: Config()
	, _size(0)	
{

	std::cout << "SugarConfig::SugarConfig(filename)" << std::endl;
    
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
SugarConfig::~SugarConfig() {}
//**********************************************************
//**********************************************************
//**********************************************************
void SugarConfig::extractParticularAttribs(TiXmlElement *pRoot)
{
	TiXmlElement *pParm;
	
	pParm = pRoot->FirstChildElement("resource_map");
	parseResourceInfo(pParm,pParm->Attribute("type"));

	pParm = pRoot->FirstChildElement("SugarAgent");
	parseAgentInfo(pParm);

	pParm = pRoot->FirstChildElement("AppliedRules");
	parseRulesInfo(pParm);

	// **********************************************
	// TODO destroy pRoot and pParm
}    
//**********************************************************
int SugarConfig::parseResourceInfo(TiXmlElement *pResource,const char *strType)
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
int SugarConfig::parseAgentInfo(TiXmlElement *pResource)
{    
// *? mirar carregador 3d per canviar de strcmp a STL    

	//*? use SWITCH
    
	try
	{
		TiXmlElement *pParm = pResource->FirstChildElement("vision");
		_agentMinVision     = atoi(pParm->Attribute("min"));
		_agentMaxVision     = atoi(pParm->Attribute("max"));
	} 
	catch(int e) 
	{
		std::cout << "Parsing 'vision' parameter .An exception occurred. Exception Nr. " << e << std::endl;
	}    
    
    try
	{
		TiXmlElement *pParm = pResource->FirstChildElement("metabolism");
		_agentMinMetabolism = atoi(pParm->Attribute("min"));
		_agentMaxMetabolism = atoi(pParm->Attribute("max"));
	} 
	catch(int e) 
	{
		std::cout << "Parsing 'metabolism' parameter .An exception occurred. Exception Nr. " << e << std::endl;
	}    
    
    try
	{
		TiXmlElement *pParm = pResource->FirstChildElement("deathAge");
		_agentMinDeathAge = atoi(pParm->Attribute("min"));
		_agentMaxDeathAge = atoi(pParm->Attribute("max"));
	} 
	catch(int e) 
	{
		std::cout << "Parsing 'deathAge' parameter .An exception occurred. Exception Nr. " << e << std::endl;
	}    
    
    try
	{
		TiXmlElement *pParm = pResource->FirstChildElement("childBearing");
		_agentMinBeginChildBearing = atoi(pParm->Attribute("min"));
		_agentMaxBeginChildBearing = atoi(pParm->Attribute("max"));
	} 
	catch(int e) 
	{
		std::cout << "Parsing 'childBearing' parameter .An exception occurred. Exception Nr. " << e << std::endl;
	}    
    
    try
	{
		TiXmlElement *pParm = pResource->FirstChildElement("pausiaWomen");
		_agentMinPausiaAgeWomen = atoi(pParm->Attribute("min"));
		_agentMaxPausiaAgeWomen = atoi(pParm->Attribute("max"));
	} 
	catch(int e) 
	{
		std::cout << "Parsing 'pausiaWomen' parameter .An exception occurred. Exception Nr. " << e << std::endl;
	}    
    
    try
	{
		TiXmlElement *pParm = pResource->FirstChildElement("pausiaMen");
		_agentMinPausiaAgeMen = atoi(pParm->Attribute("min"));
		_agentMaxPausiaAgeMen = atoi(pParm->Attribute("max"));
	} 
	catch(int e) 
	{
		std::cout << "Parsing 'pausiaMen' parameter .An exception occurred. Exception Nr. " << e << std::endl;
	}    

    try
	{
		TiXmlElement *pParm = pResource->FirstChildElement("endowment");
		_agentMinEndowment = atoi(pParm->Attribute("min"));
		_agentMaxEndowment = atoi(pParm->Attribute("max"));
	} 
	catch(int e) 
	{
		std::cout << "Parsing 'endowment' parameter .An exception occurred. Exception Nr. " << e << std::endl;
	}
	
}
//**********************************************************
int SugarConfig::parseRulesInfo(TiXmlElement *pRoot)
{
	/*TiXmlElement *pParm;	
	
	try
	{
		TiXmlElement *pParm = pResource->FirstChildElement("vision");
		_agentMinVision     = atoi(pParm->Attribute("min"));
		_agentMaxVision     = atoi(pParm->Attribute("max"));
	} 
	catch(int e) 
	{
		cout << "Parsing 'vision' parameter .An exception occurred. Exception Nr. " << e << endl;
	}    
	
	
	
	pParm = pRoot->FirstChildElement("resource_map");
	parseResourceInfo(pParm,pParm->Attribute("type"));
	
	pParm = pRoot->FirstChildElement("SugarAgent");
	parseAgentInfo(pParm);
	*/
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
  
}  
  