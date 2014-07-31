
#include <GujaratState.hxx>

#include <tinyxml.h>
#include <Exception.hxx>
#include <OriginalDemographics.hxx>
#include <AgentController.hxx>
#include <Geometry.hxx>

#include <omp.h>

#include <HunterGathererDecisionTreeController.hxx>
#include <HunterGathererProgrammedController.hxx>
#include <HunterGathererMDPController.hxx>

namespace Gujarat 
{

GujaratState * GujaratState::_instance = 0;
OriginalDemographics * GujaratState::_demographics = 0;

GujaratState & GujaratState::instance()
{
	if(!_instance)
	{
		_instance = new GujaratState;
	}
	return *_instance;
}

GujaratState::GujaratState()
{
	_demographics = new OriginalDemographics;
}

GujaratState::~GujaratState()
{
	if(_demographics)
	{
		delete _demographics;
		_demographics = 0;
	}
	
	for(int i=0; i<_hgControllers.size(); i++)
	{
		delete _hgControllers.at(i);
		_hgControllers.at(i) = 0;
	}
	_hgControllers.clear();
}

void GujaratState::setHGCaloricRequirements( int minAge, int adultAge, float minValue, float adultValue )
{
	if(!instance()._hgCaloricRequirements.empty())
	{
		instance()._hgCaloricRequirements.clear();
	}
	instance()._hgCaloricRequirements.resize(adultAge+1);

	for(int i=0; i<minAge; i++)
	{
		instance()._hgCaloricRequirements.at(i) = 0;
	}
	for(int i=minAge; i<adultAge; i++)
	{
		instance()._hgCaloricRequirements.at(i) = minValue+float(i-minAge)*float((adultValue-minValue)/(adultAge-minAge));
	}
	instance()._hgCaloricRequirements.at(adultAge) = adultValue;
}

void GujaratState::setHGAvailableForageTime( int minAge, int adultAge, float minValue, float adultValue )
{
	if(!instance()._hgAvailableForageTime.empty())
	{
		instance()._hgAvailableForageTime.clear();
	}
	instance()._hgAvailableForageTime.resize(adultAge+1);

	for(int i=0; i<minAge; i++)
	{
		instance()._hgAvailableForageTime.at(i) = 0;
	}
	for(int i=minAge; i<adultAge; i++)
	{
		instance()._hgAvailableForageTime.at(i) = minValue+float(i-minAge)*float((adultValue-minValue)/(adultAge-minAge));
	}
	instance()._hgAvailableForageTime.at(adultAge) = adultValue;

}

int GujaratState::caloricRequirements( const std::string & type, int age )
{
	if(type.compare("HunterGatherer")==0)
	{
		if(instance()._hgCaloricRequirements.empty())
		{
			std::stringstream oss;
			oss << "GujaratState::caloricRequirements() - hg caloric needs not initialized";
			throw Engine::Exception(oss.str());
		}
		if(age<instance()._hgCaloricRequirements.size())
		{
			return instance()._hgCaloricRequirements.at(age);

		}
		else
		{
			return instance()._hgCaloricRequirements.at(instance()._hgCaloricRequirements.size()-1);
		}
	}
	/*
	else if(type.compare("AgroPastoralist")==0)
	{
		if(!_apCaloricRequirements)
		{
			std::stringstream oss;
			oss << "GujaratState::caloricRequirements() - ap table not initialized";
			throw Engine::Exception(oss.str());
		}
		return *(instance()._apCaloricRequirements);
	}
	*/
	std::stringstream oss;
	oss << "GujaratState::caloricRequirements() - asking for needs with unknown type: " << type;
	throw Engine::Exception(oss.str());

}

float GujaratState::availableForageTime( const std::string & type, int age )
{
	if(type.compare("HunterGatherer")==0)
	{
		if(instance()._hgAvailableForageTime.empty())
		{
			std::stringstream oss;
			oss << "GujaratState::availableForageTime() - forage time not initialized";
			throw Engine::Exception(oss.str());
		}	
		if(age<instance()._hgAvailableForageTime.size())
		{
			return instance()._hgAvailableForageTime.at(age);

		}
		else
		{
			return instance()._hgAvailableForageTime.at(instance()._hgAvailableForageTime.size()-1);
		}
	}
	/*
	else if(type.compare("AgroPastoralist")==0)
	{
		if(!_apCaloricRequirements)
		{
			std::stringstream oss;
			oss << "GujaratState::caloricRequirements() - ap table not initialized";
			throw Engine::Exception(oss.str());
		}
		return *(instance()._apCaloricRequirements);
	}
	*/
	std::stringstream oss;
	oss << "GujaratState::caloricRequirements() - asking for forage time with unknown type: " << type;
	throw Engine::Exception(oss.str());
}
	
OriginalDemographics & GujaratState::demographics()
{
	if(!_demographics)
	{
		std::stringstream oss;
		oss << "GujaratState::demographics() - asking for demographics without being initialized";
		throw Engine::Exception(oss.str());
	}
	return *(instance()._demographics);
}

void GujaratState::setHGController( const std::string & type, const HunterGathererMDPConfig & config )
{
	for(int i=0; i<instance()._hgControllers.size(); i++)
	{
		delete instance()._hgControllers.at(i);
	}
	instance()._hgControllers.clear();
	instance()._hgControllers.resize(omp_get_max_threads());

	if( type.compare("MDP")==0)
	{
		for(int i=0; i<instance()._hgControllers.size(); i++)
		{
			instance()._hgControllers.at(i) = new HunterGathererMDPController( config );	
		}	
		return;
	}
	else if(type.compare("Random")==0)
	{	
		for(int i=0; i<instance()._hgControllers.size(); i++)
		{
			instance()._hgControllers.at(i) = new HunterGathererProgrammedController();
		}
		return;
	}
	else if(type.compare("DecisionTree")==0)
	{	
		for(int i=0; i<instance()._hgControllers.size(); i++)
		{
			instance()._hgControllers.at(i) = new HunterGathererDecisionTreeController();
		}
		return;
	}
	
	std::stringstream oss;
	oss << "GujaratState::setHGController() - unknown type of controller: " << type;
	throw Engine::Exception(oss.str());
}

AgentController & GujaratState::controller()
{
	int numThread = omp_get_thread_num();
	AgentController * controller = instance()._hgControllers.at(numThread);
	if(!controller)
	{
		std::stringstream oss;
		oss << "GujaratState::controller() - asking for controller without being initialized";
		throw Engine::Exception(oss.str());
	}
	return *(instance()._hgControllers.at(numThread));
}

int GujaratState::sectorsMask( int i, int j)
{
	if(instance()._sectorsMask.size()==0)
	{
		std::stringstream oss;
		oss << "GujaratState::sectorsMask() - asking for sectors mask without being initialized";
		throw Engine::Exception(oss.str());
	}
	return instance()._sectorsMask.at(i).at(j);
}

void GujaratState::initializeSectorsMask( int numSectors, int homeRange )
{
	std::vector< std::vector< Engine::Point2D<int> > > sectors;
	
	float alpha = 360/numSectors;
	alpha = alpha * M_PI/180.0f;
	Engine::Point2D<float> b;
	Engine::Point2D<float> c;

	sectors.resize( numSectors );
	// center position + home Range in any direction
	instance()._sectorsMask.resize( 1+2*homeRange );
	for ( unsigned k = 0; k < 1+2*homeRange; k++ )
	{
		instance()._sectorsMask.at(k).resize( 1+2*homeRange );
	}

	b._x = 0;
	b._y = - homeRange;

	for(int i=0; i<numSectors; i++)
	{
		c._x = b._x*std::cos(alpha) - b._y*std::sin(alpha);
		c._y = b._x*std::sin(alpha) + b._y*std::cos(alpha);
		sectors.at(i).push_back( Engine::Point2D<int>( (int)b._x, (int)b._y ) );
		sectors.at(i).push_back( Engine::Point2D<int>( (int)c._x, (int)c._y ) );
		b = c;
	}

	for ( int x=-homeRange; x<=homeRange; x++ )
	{
		for ( int y=-homeRange; y<=homeRange; y++ )
		{
			if(x==0 && y==0)
			{
				continue;
			}
			Engine::Point2D<int> p( x, y );
			instance()._sectorsMask.at(x+homeRange).at(y+homeRange) = -1;	
			for ( unsigned k = 0; k < numSectors; k++ )
			{
				if ( Engine::insideTriangle( p, sectors.at(k).at(0), sectors.at(k).at(1) ) )
				{
					instance()._sectorsMask.at(x+homeRange).at(y+homeRange) = k;
					break;
				}
			}
		}
	}

}

} // namespace Gujarat 

