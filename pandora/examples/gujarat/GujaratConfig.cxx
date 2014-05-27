#include <GujaratConfig.hxx>
#include <sstream>
#include <Exceptions.hxx>
#include <HunterGathererMDPConfig.hxx>

#include <GujaratState.hxx>
#include <HGMindFactory.hxx>

#include <locale>         // std::locale, std::tolower


namespace Gujarat
{

GujaratConfig::GujaratConfig() 
	: _size(0), _soilFile("soil file not loaded"), _demFile("dem file not loaded"), _distWaterFile("distance to water file not loaded"), _weightWaterFile("weight to water file not loaded"), _climateSeed(1), _hunterGathererController( "Rule-Based" ), _controllerConfig(0), _numHG(0), _numAP(0)
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

	element = root->FirstChildElement("biomassDistribution");
	int numCells = 0;
	retrieveAttributeMandatory( element, "type", _biomassDistribution);
	retrieveAttributeMandatory( element, "size", numCells);
	retrieveAttributeMandatory( element, "distToWaterFile", _distWaterFile);

	TiXmlElement * option = 0;
	if(_biomassDistribution.compare("linDecayFromWater")==0)
	{
		option = element->FirstChildElement("linDecayFromWater");
	}
	else if(_biomassDistribution.compare("logDecayFromWater")==0)
	{
		option = element->FirstChildElement("logDecayFromWater");
	}
	long int sumWeights = 0;
	if(option)
	{
		retrieveAttributeMandatory(option, "weightFile", _weightWaterFile);
		retrieveAttributeMandatory(option, "sumWeights", sumWeights);
		// numCells = numRows*numColumns
		numCells *= numCells;
		_waterDistConstant = (double)numCells/(double)sumWeights;
		//std::cout << "type: " << _biomassDistribution << " sum weight: " << sumWeights << " constant: " << _waterDistConstant << std::endl;
	}

	element = root->FirstChildElement("rainHistoricalDistribution");
	retrieveAttributeMandatory( element, "mean", _rainHistoricalDistribMean );
	retrieveAttributeMandatory( element, "stdev", _rainHistoricalDistribStdev );	
	
	
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
				retrieveAttributeMandatory( elem, "value", _duneBiomass );
				retrieveAttributeMandatory( elem, "efficiency", _duneEfficiency );
				retrieveAttributeMandatory( elem, "minimum", _duneMinimum);
				
				retrieveAttributeMandatory( elem, "baseMeanRainfall", _duneBiomassBaseMeanRainfall);
				
			}
			else if ( !elemType.compare("interdune") )
			{
				retrieveAttributeMandatory( elem, "value", _interduneBiomass );
				retrieveAttributeMandatory( elem, "efficiency", _interduneEfficiency );
				retrieveAttributeMandatory( elem, "minimum", _interduneMinimum);
				
				retrieveAttributeMandatory( elem, "baseMeanRainfall", _interduneBiomassBaseMeanRainfall);
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
			retrieveAttributeMandatory( elem, "value", _storeRasters[name]);
		}
	}
	
	
	
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
	
	retrieveAttributeMandatory( element, "alphaDecTree", _alphaDecTree );	
	
	retrieveAttributeMandatory( element, "numSectors", _numSectors );

	retrieveAttributeMandatory( element, "hunterGathererInformationCaducityTime", _hunterGathererInformationCaducityTime );
	
	// Generate a sector mask of lowrescells for the lowresmap the would overlap the original sector mask.
	// The difference of areas between both sectors must be minimized. 
	int r_down    = _homeRange / _lowResolution;
	int r_up      = 1 + r_down;
	int loss_down = abs(_homeRange*_homeRange 
						- 
						r_down*r_down*_lowResolution*_lowResolution); 
	int loss_up   = abs(_homeRange*_homeRange 
						- 
						r_up*r_up*_lowResolution*_lowResolution); 
	
	if (loss_down > loss_up)
	{
		_lowResHomeRange = r_up;
	}
	else
	{
		_lowResHomeRange = r_down;
	}
	
	GujaratState::initializeSectorsMask(_numSectors, _homeRange, GujaratState::getHRSectorsMask());
	GujaratState::initializeSectorsMask(_numSectors, _lowResHomeRange, GujaratState::getLRSectorsMask());
	
	retrieveAttributeMandatory( element, "walkingSpeedHour", _walkingSpeedHour );
	retrieveAttributeMandatory( element, "forageTimeCost", _forageTimeCost );
//	retrieveAttributeMandatory( element, "availableForageTime", _availableForageTime );
	retrieveAttributeMandatory( element, "demographicsModel", _demographicsModel );
	GujaratState::setDemographics(_demographicsModel);

	retrieveAttributeMandatory( element, "controllerType", _hunterGathererController );
	// call parseHGMDPConfig after initialization of _numSectors
	parseHGMDPConfig( element->FirstChildElement("controllerConfig") );
	GujaratState::setHGController( _hunterGathererController, *_controllerConfig);

	retrieveAttributeMandatory( element, "mindType", _hunterGathererMind );
	HGMindFactory::getInstance().setHGMind(_hunterGathererMind);
	
	
	std::string strShareInfo;
	retrieveAttributeMandatory( element, "shareInfo", strShareInfo );
	std::locale loc;
	for (std::string::size_type i=0; i<strShareInfo.length(); ++i)
	{
		strShareInfo[i]=std::tolower(strShareInfo[i],loc);
	}
	
	if (strShareInfo.compare("yes")==0) 
	{
		_shareInfo = true;
	}
	else if (strShareInfo.compare("no")==0)
	{
		_shareInfo = false;
	}
	else
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: Unrecognized value for shareInfo : " << strShareInfo << std::endl;
		throw Engine::Exception(sstr.str());
	}
	
std::cout << "DBG 1" << std::endl;
	int positionX=-1;
	int positionY=-1;
        TiXmlElement * initialPosition = element->FirstChildElement("initialPosition");
        retrieveAttributeMandatory(initialPosition, "x", positionX);
	retrieveAttributeMandatory(initialPosition, "y", positionY);
	GujaratState::setInitialPosition(positionX,positionY);
std::cout << "DBG 2" << std::endl;
	
	float minValue = 0;
	float adultValue = 0;
	int minAge = 0;
	int adultAge = 0;
	

	TiXmlElement * calories = element->FirstChildElement("calories");
	retrieveAttributeMandatory(calories, "minValue", minValue);
	retrieveAttributeMandatory(calories, "adultValue", adultValue);
	retrieveAttributeMandatory(calories, "minAge", minAge);
	retrieveAttributeMandatory(calories, "adultAge", adultAge);
	GujaratState::setHGCaloricRequirements(minAge, adultAge, minValue, adultValue);

	TiXmlElement * workTime = element->FirstChildElement("availableForageTime");
	retrieveAttributeMandatory(workTime, "minValue", minValue);
	retrieveAttributeMandatory(workTime, "adultValue", adultValue);
	retrieveAttributeMandatory(workTime, "minAge", minAge);
	retrieveAttributeMandatory(workTime, "adultAge", adultAge);
	GujaratState::setHGAvailableForageTime(minAge, adultAge, minValue, adultValue);
/*
	TiXmlElement* calTable = element->FirstChildElement( "caloriesTable" );
	if ( calTable == NULL )	
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: No caloriesTable element found for Hunter Gatherers in Config" << std::endl;
		throw Engine::Exception(sstr.str());
	}
	*/
	/*
	TiXmlElement* initializerElem = element->FirstChildElement( "initialization" );
	if ( initializerElem == NULL )	
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: No <initialization> element found for Hunter Gatherers in Config" << std::endl;
		throw Engine::Exception(sstr.str());
	}
//	GujaratState::setHGCaloricRequirements(calTable);

	// MRJ: Loading agro pastoralists attributes	
	element = root->FirstChildElement("agroPastoralists");

	calTable = element->FirstChildElement( "caloriesTable" );
	if ( calTable == NULL )	
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: No caloriesTable element found for AgroPastoralists in Config" << std::endl;
		throw Engine::Exception(sstr.str());
	}
*/
	/*
	initializerElem = element->FirstChildElement( "initialization" );
	if ( initializerElem == NULL )	
	{
		std::stringstream sstr;
		sstr << "[CONFIG]: ERROR: No <initialization> element found for Agro Pastoralists in Config" << std::endl;
		throw Engine::Exception(sstr.str());
	}
	*/

//	GujaratState::setAPCaloricRequirements(calTable);
	
//	retrieveAttributeMandatory( element, "num", _numAP );
//	retrieveAttributeMandatory( element, "maxCropHomeDistance", _maxCropHomeDistance );

	/*
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
	*/
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
	
	
	if (_controllerConfig->getNumberForageActions()!=_numSectors)
	{
		throw Engine::Exception("_controllerConfig.getNumberForageActions() != _numSectors");
	}
	
	if (_controllerConfig->getNumberMoveHomeActions()!=1)
	{
			//*?
			//throw Engine::Exception("_controllerConfig.getNumberMoveHomeActions() != 1");
	}
	
	_uctSeed = _controllerConfig->getSeed();
	
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

