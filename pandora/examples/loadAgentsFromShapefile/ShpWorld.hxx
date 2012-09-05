
#ifndef __ShpWorld_hxx
#define __ShpWorld_hxx

#include <World.hxx>

namespace Engine
{
	class Simulation;
}

namespace Examples 
{

class ShpWorld : public Engine::World
{
	void createRasters();
	void createAgents();
	std::string _shpFile;

public:
	ShpWorld( const Engine::Simulation & simulation, const std::string & shpFile );
	virtual ~ShpWorld();
};

} // namespace Examples 

#endif // __ShpWorld_hxx

