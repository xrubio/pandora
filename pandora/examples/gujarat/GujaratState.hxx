
#ifndef __GujaratState_hxx__
#define __GujaratState_hxx__

#include <cstdlib>
#include <string>
#include <GeneralState.hxx>
#include <Point2D.hxx>
#include <Point3D.hxx>
#include <vector>

class TiXmlElement;

namespace Gujarat 
{

class GujaratDemographics;
class AgentController;
class HunterGathererMDPConfig;

class GujaratState
{
public:
	typedef std::vector< std::vector <int> > SectorsMask;

private:
	static GujaratState * _instance;

	std::vector<int> _hgCaloricRequirements;
	std::vector<float> _hgAvailableForageTime;

	std::vector<AgentController*> _hgControllers;

	static GujaratDemographics * _demographics;

	SectorsMask _HRSectorsMask;
	SectorsMask _LRSectorsMask;
	
	long int HRCellsInHomeRange;
	long int LRCellsInHomeRange;

	Engine::Point2D<int> _initialPosition;
	
protected:
	GujaratState();

public:
	static GujaratState & instance();
	virtual ~GujaratState();

	static void setHGCaloricRequirements( int minAge, int adultAge, float minValue, float adultValue );
	static void setHGAvailableForageTime( int minAge, int adultAge, float minValue, float adultValue );
	//static void setAPCaloricRequirements( TiXmlElement * element );
	static void setDemographics( const std::string & model );

	static int caloricRequirements( const std::string & type, int age );
	static float availableForageTime( const std::string & type, int age );
	//static CaloricRequirementsTable & caloricRequirements( const std::string & type );
	static GujaratDemographics & demographics();

	static void setHGController( const std::string & type, const HunterGathererMDPConfig & config );
	static AgentController & controller();

	static void initializeSectorsMask( int numSectors, int homeRange, SectorsMask & sm );
	static void initializeSectorsMaskTrigonometricaly( int numSectors, int homeRange, SectorsMask & sm );
	
	static void setInitialPosition(int positionX, int positionY) { 
		instance()._initialPosition._x = positionX;
		instance()._initialPosition._y = positionY;
	 }

	static Engine::Point2D<int> getInitialPosition() { return instance()._initialPosition; }	

	
	//static int sectorsMask( int i, int j);
	static int sectorsMask( int i, int j, const SectorsMask & sm );
	static SectorsMask & getHRSectorsMask() { return instance()._HRSectorsMask; }
	static SectorsMask & getLRSectorsMask() { return instance()._LRSectorsMask; }
	
	//*****************************************************************************
	//*?
	
	static float Point2Angle(float ex, float ey);
	static float getAngle(int i, int j, int homeRange);
	static float getAngle(float x, float y);
	
};

} // namespace Gujarat

#endif // __GujaratState_hxx__

