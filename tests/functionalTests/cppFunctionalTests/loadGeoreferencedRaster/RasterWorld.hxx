
#ifndef __RasterWorld_hxx
#define __RasterWorld_hxx

#include <World.hxx>

namespace Engine
{
	class Simulation;
}

namespace Test 
{

class RasterWorld : public Engine::World
{
	void createRasters();
	void createAgents();
	std::string _rasterFile;

public:
	RasterWorld( const Engine::Simulation & simulation, Engine::Scheduler * scheduler = 0, const std::string & rasterFile = "no file");
	virtual ~RasterWorld();
};

} // namespace Test 

#endif // __RasterWorld_hxx

