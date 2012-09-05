#include <GujaratConfig.hxx>
#include <sstream>
#include <Exceptions.hxx>
#include <HunterGathererMDPConfig.hxx>
#include <FixedAgentInitializer.hxx>

#include <GujaratState.hxx>

namespace Gujarat
{

GujaratConfig::GujaratConfig() 
	: _size(0), _soilFile("no loaded file"), _demFile("no loaded file"), _duneMapFile("no loaded file"), _climateSeed(1),
	_hunterGathererController( "Rule-Based" ), _hgInitializer(0), _apInitializer(0), _controllerConfig(0)
{
}
  
GujaratConfig::~GujaratConfig()
{

}

void GujaratConfig::extractParticularAttribs(TiXmlElement * root)
{
	TiXmlElement * element = NULL;
	
	element = root->FirstChildElement("climateSeed");
	retrieveAttributeMandatory( element, "value", _climateSeed );
	
	element = root->FirstChildElement("soil");	
	parseSoilInfo(element);

	element = root->FirstChildElement("dem");
	retrieveAttributeMandatory( element, "fileName", _demFile );

	element = root->FirstChildElement("duneMap");
	retrieveAttributeMandatory( element, "fileName", _duneMapFile);

	element = root->FirstChildElement("rainHistoricalDistribution");

	retrieveAttributeMandatory( element, "shape", _rainHistoricalDistribShape );
	retrieveAttributeMandatory( element, "scale", _rainHistoricalDistribScale );
	retrieveAttributeMandatory( element, "mean", _rainHistoricalDistribMean );

	element = root->FirstChildElement("socialRange");
	retrieveAttributeMandatory( element, "value", _socialRange );
	
	element = root->FirstChildElement("hunterGatherers");
	retrieveAttributeMandatory( element, "num", _numHG );
	retrieveAttributeMandatory( element, "homeRange", _homeRange );
//	retrieveAttributeMandatory( element, "surplusForReproductionThreshold", _surplusForReproductionThreshold );
//	retrieveAttributeMandatory( element, "surplusWanted", _surplusWanted );
//	retrieveAttributeMandatory( element, "surplusSpoilage", _surplusSpoilage );
	
//	retrieveAttributeMandatory( element, "foodNeedsForReproduction", _hgFoodNeedsForReproduction );
	
	retrieveAttributeMandatory( element, "adulthoodAge", _adulthoodAge );	
	
	retrieveAttributeMandatory( element, "numSectors", _numSectors );
	GujaratState::initializeSectorsMask(_numSectors, _homeRange);

	retrieveAttributeMandatory( element, "walkingSpeedHour", _walkingSpeedHour );
	retrieveAttributeMandatory( element, "forageTimeCost", _forageTimeCost );
	retrieveAttributeMandatory( element, "availableForageTime", _availableForageTime );
	retrieveAttributeMandatory( element, "demographicsModel", _demographicsModel );
	GujaratState::setDemographics(_demographicsModel);

	retrieveAttributeMandatory( element, "controllerType", _hunterGathererController );
	parseHGMDPConfig( element->FirstChildElement("controllerConfig") );
	GujaratState::setHGController( _hunterGathererController, *_controllerConfig);

	TiXmlElement* calTable = element->FirstChildElement( "caloriesTable" );
	if ( calTable == NULL )	
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: No caloriesTable element found for Hunter Gatherers in Config" << std::endl;
		throw Engine::Exception(sstr.str());
	}
	/*
	TiXmlElement* initializerElem = element->FirstChildElement( "initialization" );
	if ( initializerElem == NULL )	
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: No <initialization> element found for Hunter Gatherers in Config" << std::endl;
		throw Engine::Exception(sstr.str());
	}
	std::string initializerType = initializerElem->Attribute("type");	
	if ( initializerType == "fixed" )
	{
		_hgInitializer = new FixedAgentInitializer( initializerElem );
	}
	else
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: Unknown initializer '" << initializerType << "' type specified" << std::endl;
		throw Engine::Exception(sstr.str());	
	}
	*/

	GujaratState::setHGCaloricRequirements(calTable);

	// MRJ: Loading agro pastoralists attributes	
	element = root->FirstChildElement("agroPastoralists");

	calTable = element->FirstChildElement( "caloriesTable" );
	if ( calTable == NULL )	
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: No caloriesTable element found for AgroPastoralists in Config" << std::endl;
		throw Engine::Exception(sstr.str());
	}

	/*
	initializerElem = element->FirstChildElement( "initialization" );
	if ( initializerElem == NULL )	
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: No <initialization> element found for Agro Pastoralists in Config" << std::endl;
		throw Engine::Exception(sstr.str());
	}
	initializerType = initializerElem->Attribute("type");
	if ( initializerType == "fixed" )
	{
		_apInitializer = new FixedAgentInitializer( initializerElem );
	}
	else
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: Unknown initializer '" << initializerType << "' type specified" << std::endl;
		throw Engine::Exception(sstr.str());	
	}
	*/

	GujaratState::setAPCaloricRequirements(calTable);
	
	retrieveAttributeMandatory( element, "num", _numAP );
	retrieveAttributeMandatory( element, "maxCropHomeDistance", _maxCropHomeDistance );

	element = root->FirstChildElement("daysPerSeason");
	retrieveAttributeMandatory( element, "value", _daysPerSeason );
	element = root->FirstChildElement("daysPerYear");
	retrieveAttributeMandatory( element, "value", _daysPerYear );
	
	element = root->FirstChildElement("cellResolution");
	retrieveAttributeMandatory( element, "value", _cellResolution );

	element = root->FirstChildElement("lowResolution");
	retrieveAttributeMandatory( element, "value", _lowResolution);

	element = root->FirstChildElement("massToEnergyRate" );
	retrieveAttributeMandatory( element, "value", _massToEnergyRate );
	element = root->FirstChildElement("energyToCaloriesRate" );
	retrieveAttributeMandatory( element, "value", _energyToCalRate );
	
	TiXmlNode* n = NULL;
	while ( ( n = root->IterateChildren( n ) ) )
	{
		if ( n->Type() != TiXmlNode::TINYXML_ELEMENT ) continue;
		TiXmlElement* elem = n->ToElement();
		if ( !elem->ValueStr().compare("cellBiomass") )
		{
			std::string elemType;
			retrieveAttributeMandatory( elem, "type", elemType );
			if ( !elemType.compare("dune" ) )
			{
				retrieveAttributeMandatory( elem, "mean", _duneBiomass );
				retrieveAttributeMandatory( elem, "stddev", _duneBiomassStdDev );
				retrieveAttributeMandatory( elem, "efficiency", _duneEfficiency );
			}
			else if ( !elemType.compare("interdune") )
			{
				retrieveAttributeMandatory( elem, "mean", _interduneBiomass );
				retrieveAttributeMandatory( elem, "stddev", _interduneBiomassStdDev );
				retrieveAttributeMandatory( elem, "efficiency", _interduneEfficiency );
			}
			else
			{
				std::stringstream sstr;
				sstr << "ERROR: Loading simulation config document" << std::endl;
				sstr << "Unknown cellBiomass type " << elemType << " found!!!" << std::endl;
				throw Engine::Exception( sstr.str() ); 
			}
		}
		else if ( !elem->ValueStr().compare( "storeRaster" ) ) 
		{
			std::string name, valueStr;
			retrieveAttributeMandatory( elem, "name", name );
			retrieveAttributeMandatory( elem, "value", valueStr );
			bool value = ( valueStr == "yes" ? true : false );
			_storeRasters[name] = value;
		}
	}

	std::cout << "[CONFIG]: Mass To Energy Rate: " << _massToEnergyRate << std::endl;
	std::cout << "[CONFIG]: Energy To Calories Rate: " << _energyToCalRate << std::endl;
	std::cout << "[CONFIG]: Dune Cell: Biomass: Mass: " << _duneBiomass << std::endl;
	std::cout << "[CONFIG]: Dune Cell: Biomass: Std. Dev: " << _duneBiomassStdDev << std::endl;
	std::cout << "[CONFIG]: Dune Cell: Biomass: Efficiency: " << _duneEfficiency << std::endl;
	std::cout << "[CONFIG]: Interdune Cell: Biomass: Mass: " << _interduneBiomass << std::endl;
	std::cout << "[CONFIG]: Interdune Cell: Biomass: Std. Dev: " << _interduneBiomassStdDev << std::endl;
	std::cout << "[CONFIG]: Interdune Cell: Biomass: Efficiency: " << _interduneEfficiency << std::endl;
	std::cout << "[CONFIG]: Hunter Gatherer Controller: " << _hunterGathererController << std::endl;
	if ( _controllerConfig != NULL )
		_controllerConfig->dump( std::cout ); 

}
 
void GujaratConfig::parseHGMDPConfig( TiXmlElement* element )
{
	if ( element == NULL )
	{
		std::cerr << "[CONFIG]: controllerConfig element not found on configuration document" << std::endl;
		return;
	}
	// Not model-based controller config
	if ( element->Attribute("type")!=std::string("MDP"))
	{
		return;
	}
	_controllerConfig = new HunterGathererMDPConfig(element);
}
 
void GujaratConfig::parseSoilInfo( TiXmlElement * element )
{    
	// TODO use STL
    //*? use SWITCH
	_soilFile = element->Attribute("fileName"); // path
	_size = atoi(element->Attribute("size"));
}

const int & GujaratConfig::getSize() const
{
	return _size;
}

} // namespace Gujarat

