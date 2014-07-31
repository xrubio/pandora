
#ifndef __Survey_hxx
#define __Survey_hxx

#include <World.hxx>
#include <SimulationRecord.hxx>

namespace BattleSim
{
class SurveyConfig;

class Survey : public Engine::World
{
	Engine::SimulationRecord _record;

	void createRasters();
	void createAgents();

    void stepEnvironment();
public:
	Survey( SurveyConfig * config, Engine::Scheduler * scheduler = 0);
	virtual ~Survey();
};

} // namespace BattleSim 


#endif // __Survey_hxx

