
#ifndef __Battlefield_hxx
#define __Battlefield_hxx

#include <World.hxx>

namespace BattleSim
{

class BattlefieldConfig;

enum Rasters
{
	eVirtualRecord,
	eImpacts
};

class Battlefield : public Engine::World
{
	const BattlefieldConfig & _config;

	void createRasters();
	void createAgents();
	
	void stepEnvironment();
public:
	Battlefield( Engine::Simulation & simulation, const BattlefieldConfig & config );
	virtual ~Battlefield();
};

} // namespace BattleSim 


#endif // __Battlefield_hxx

