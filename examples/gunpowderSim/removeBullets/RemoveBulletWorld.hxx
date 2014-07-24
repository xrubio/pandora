
#ifndef __RemoveBulletWorld_hxx
#define __RemoveBulletWorld_hxx

#include <World.hxx>
#include <SimulationRecord.hxx>

namespace BattleSim
{
class RemoveBulletConfig;

class RemoveBulletWorld : public Engine::World
{
	Engine::SimulationRecord _record;

	void createRasters();
public:
	RemoveBulletWorld(RemoveBulletConfig * config, Engine::Scheduler * scheduler = 0);
	virtual ~RemoveBulletWorld();
};

} // namespace BattleSim 


#endif // __RemoveBulletWorld_hxx

