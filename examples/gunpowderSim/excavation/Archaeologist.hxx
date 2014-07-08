
#ifndef __Archaeologist_hxx__
#define __Archaeologist_hxx__

#include "Agent.hxx"
#include <string>

#include "Point2D.hxx"

namespace Engine
{
	class Agent;
}

namespace BattleSim
{

typedef struct
{
	char _id[100];	
	Engine::Point2D<int> _position;
	int _bulletsGathered;
	bool _horizontalMovement;
	int _skill;
} ArchaeologistPackage;

class Archaeologist : public Engine::Agent
{
	int _bulletsGathered;
	bool _horizontalMovement;
	int _skill;

	void move();
	void collectFindings();
	
	void store();
public:
	// todo remove environment from here
	Archaeologist( const std::string & id, bool horizontalMovement, int skill );
	Archaeologist( const ArchaeologistPackage & package );
	virtual ~Archaeologist();

	void step();
	void * createPackage();
};

} // namespace BattleSim

#endif // __Archaeologist_hxx__

