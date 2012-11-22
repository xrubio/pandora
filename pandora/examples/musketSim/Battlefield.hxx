
#ifndef __Battlefield_hxx
#define __Battlefield_hxx

#include <World.hxx>

namespace MusketSim
{

class BattlefieldConfig;

class Battlefield : public Engine::World
{
	const BattlefieldConfig & _config;

	int _numSoldiers;
	void createRasters();
	void createAgents();

	void stepEnvironment(){}
public:
	Battlefield( Engine::Simulation & simulation, const BattlefieldConfig & config );
	virtual ~Battlefield();
};

} // namespace MusketSim 


#endif // __Battlefield_hxx

