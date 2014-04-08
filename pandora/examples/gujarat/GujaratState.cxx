
#include <GujaratState.hxx>

#include <tinyxml.h>
#include <Exceptions.hxx>
#include <CaloricRequirementsTable.hxx>
#include <GujaratDemographics.hxx>
//#include <RamirezDemographics.hxx>
//#include <AlexisDemographics.hxx>
#include <OriginalDemographics.hxx>
#include <AgentController.hxx>
#include <Geometry.hxx>

#include <omp.h>

#include <HunterGathererDecisionTreeController.hxx>
#include <HunterGathererProgrammedController.hxx>
#include <HunterGathererMDPController.hxx>
//#include <HunterGathererRandomMDPPolicyController.hxx>

#include <math.h> 

namespace Gujarat 
{

GujaratState * GujaratState::_instance = 0;
//CaloricRequirementsTable * GujaratState::_hgCaloricRequirements = 0;
//CaloricRequirementsTable * GujaratState::_apCaloricRequirements = 0;
GujaratDemographics * GujaratState::_demographics = 0;

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
}

GujaratState::~GujaratState()
{
	/*
	if( _apCaloricRequirements )
	{
		delete _apCaloricRequirements;
		_apCaloricRequirements = 0;
	}
	*/
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
	

void GujaratState::setDemographics( const std::string & model )
{
	if(_demographics)
	{
		delete _demographics;
	}
	_demographics = new OriginalDemographics;

	/*
	if(model.compare("original")==0)
	{
		_demographics = new OriginalDemographics;
		return;
	}
	else if(model.compare("ramirez")==0)
	{
		_demographics = new RamirezDemographics;
		return;
	}
	else if(model.compare("alexis")==0)
	{
		_demographics = new AlexisDemographics;
		return;
	}
	*/

	/*
	std::stringstream oss;
	oss << "GujaratState::setDemographics() - unknown model: " << model;
	throw Engine::Exception(oss.str());
	*/
}

GujaratDemographics & GujaratState::demographics()
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
	else if(type.compare("RandomMDP")==0)
	{	
		for(int i=0; i<instance()._hgControllers.size(); i++)
		{
			//instance()._hgControllers.at(i) = new HunterGathererRandomMDPPolicyController(config);
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

int GujaratState::sectorsMask( int i, int j, const SectorsMask & sm)
{
	if(sm.size()==0)
	{
		std::stringstream oss;
		oss << "GujaratState::sectorsMask() - asking for sectors mask without being initialized";
		throw Engine::Exception(oss.str());
	}
	return sm.at(i).at(j);
}


float GujaratState::getAngle(float xx, float yy)
{	
	float x=xx;
	float y=yy;
	
	if(x < 0.0) x=-x;
	if(y < 0.0) y=-y;
	
	float result = (atan(y/x)*180.0)/M_PI;	
	
	x=xx;
	y=yy;
	
	//if ( 0 <= x && 0 <= y)
	//	return result;
	
	if ( 0 > x && 0 <= y)
		return 180.0 - result;
	
	if ( 0 > x && 0 > y)
		return 180.0 + result;
	
	if ( 0 <= x && 0 > y)
		return 360.0 - result;
	
	return result;
}

void GujaratState::initializeSectorsMaskTrigonometricaly( int numSectors, int homeRange, SectorsMask & sm )
{
	
	float alpha = 360.0f/(float)numSectors;	
	
	// center position + home Range in any direction
	sm.resize( 1+2*homeRange );
	for ( unsigned int k = 0; k < (unsigned int)1+2*homeRange; k++ )
	{
		sm.at(k).resize( 1+2*homeRange );
	}

	sm.at(0).at(0) = -1;
	Engine::Point2D<int> center(0,0);
	
	for ( int x=-homeRange; x<=homeRange; x++ )
	{
		for ( int y=-homeRange; y<=homeRange; y++ )
		{
			float fX = x;
			float fY = y;
			Engine::Point2D<int> p( x, y );
			
			if((0==x && 0==y) || p.distanceSQ(center) > homeRange*homeRange)
			{
				sm.at(x+homeRange).at(y+homeRange) = -1;			
			}
			else
			{
				float angle = getAngle(fX,fY);
				sm.at(x+homeRange).at(y+homeRange) =  (angle/alpha);	
				
				//std::cout <<"GNUPLOT "<< x <<"\t"<<y<<"\t"<< sm.at(x+homeRange).at(y+homeRange) << std::endl;
			}
			// show points from gnuplot :
			// plot "./data.csv" u 1:2:3 with points pointtype 7 palette
			
		}
	}

	
	std::cout << "SectorMask Homerange =" << homeRange << std::endl;		
	for ( int x=-homeRange; x<=homeRange; x++ )
	{
		for ( int y=-homeRange; y<=homeRange; y++ )
		{
			std::cout << 1+sm.at(x+homeRange).at(y+homeRange);		
		}
		std::cout << std::endl;		
	}
	
}



void GujaratState::initializeSectorsMask( int numSectors, int homeRange, SectorsMask & sm )
{
	// Use a more elegant way of doing this
	//*? TODO
	/*if(homeRange < 100) 
	{
		initializeSectorsMaskTrigonometricaly(numSectors, homeRange, sm );	
	}
	else*/ {
		std::vector< std::vector< Engine::Point2D<int> > > sectors;
	
		float alpha = 360/numSectors;
		alpha = alpha * M_PI/180.0f;
		Engine::Point2D<float> b;
		Engine::Point2D<float> c;

		sectors.resize( numSectors );
		// center position + home Range in any direction
		sm.resize( 1+2*homeRange );
		for ( unsigned int k = 0; k < (unsigned int)1+2*homeRange; k++ )
		{
			sm.at(k).resize( 1+2*homeRange );
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
	
		sm.at(0).at(0) = -1;
		for ( int x=-homeRange; x<=homeRange; x++ )
		{
			for ( int y=-homeRange; y<=homeRange; y++ )
			{
				if(x==0 && y==0)
				{
					continue;
				}
				Engine::Point2D<int> p( x, y );
				sm.at(x+homeRange).at(y+homeRange) = -1;	
				for ( int k = 0; k < numSectors; k++ )
				{
					if ( Engine::insideTriangle( p, sectors.at(k).at(0), sectors.at(k).at(1) ) )
					{
						sm.at(x+homeRange).at(y+homeRange) = k;
						break;
					}
				}
		 
			}
	 
		}

	}
}



} // namespace Gujarat 

