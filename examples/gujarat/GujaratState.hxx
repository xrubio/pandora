
#ifndef __GujaratState_hxx__
#define __GujaratState_hxx__


#include <string>
#include <GeneralState.hxx>
#include <Point2D.hxx>
#include <Point3D.hxx>
#include <vector>

class TiXmlElement;

namespace Gujarat 
{

class OriginalDemographics;
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

	static OriginalDemographics * _demographics;

	SectorsMask _sectorsMask;

protected:
	GujaratState();

public:
	static GujaratState & instance();
	virtual ~GujaratState();

	static void setHGCaloricRequirements( int minAge, int adultAge, float minValue, float adultValue );
	static void setHGAvailableForageTime( int minAge, int adultAge, float minValue, float adultValue );

	static int caloricRequirements( const std::string & type, int age );
	static float availableForageTime( const std::string & type, int age );
	//static CaloricRequirementsTable & caloricRequirements( const std::string & type );
	static OriginalDemographics & demographics();

	static void setHGController( const std::string & type, const HunterGathererMDPConfig & config );
	static AgentController & controller();

	static void initializeSectorsMask( int numSectors, int homeRange );
	static int sectorsMask( int i, int j );
};

} // namespace Gujarat

#endif // __GujaratState_hxx__

