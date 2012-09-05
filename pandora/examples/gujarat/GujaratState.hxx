
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

class CaloricRequirementsTable;
class GujaratDemographics;
class AgentController;
class HunterGathererMDPConfig;

class GujaratState
{
public:
	typedef std::vector< std::vector <int> > SectorsMask;

private:
	static GujaratState * _instance;

	static CaloricRequirementsTable * _hgCaloricRequirements;
	static CaloricRequirementsTable * _apCaloricRequirements;

	std::vector<AgentController*> _hgControllers;

	static GujaratDemographics * _demographics;

	SectorsMask _sectorsMask;

protected:
	GujaratState();

public:
	static GujaratState & instance();
	virtual ~GujaratState();

	static void setHGCaloricRequirements( TiXmlElement * element );
	static void setAPCaloricRequirements( TiXmlElement * element );
	static void setDemographics( const std::string & model );

	static CaloricRequirementsTable & caloricRequirements( const std::string & type );
	static GujaratDemographics & demographics();

	static void setHGController( const std::string & type, const HunterGathererMDPConfig & config );
	static AgentController & controller();

	static void initializeSectorsMask( int numSectors, int homeRange );
	static int sectorsMask( int i, int j );
};

} // namespace Gujarat

#endif // __GujaratState_hxx__

