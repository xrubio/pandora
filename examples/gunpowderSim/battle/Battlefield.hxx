
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
	void createRasters();
	void createAgents();
	
	void stepEnvironment();
public:
	Battlefield( BattlefieldConfig * config, Engine::Scheduler * scheduler = 0);
	virtual ~Battlefield();
};

} // namespace BattleSim 


#endif // __Battlefield_hxx

