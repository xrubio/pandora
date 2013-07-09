
#ifndef __Battlefield_hxx
#define __Battlefield_hxx

#include <World.hxx>

namespace ClassicalWarfare
{

class BattlefieldConfig;
class Line;

class Battlefield : public Engine::World
{
	const BattlefieldConfig & _config;

	void createRasters();
	void createAgents();
	void createLine( const Line & line, int depth, bool isRed, int & createdWarriors);
	
	void stepEnvironment();

public:
	Battlefield( Engine::Simulation & simulation, const BattlefieldConfig & config );
	virtual ~Battlefield();
};

} // namespace ClassicalWarfare 


#endif // __Battlefield_hxx

