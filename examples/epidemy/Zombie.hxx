
#ifndef __Zombie_hxx__
#define __Zombie_hxx__

#include <Agent.hxx>

namespace Examples
{
class Human;

class Zombie : public Engine::Agent
{
//	Engine::Agent * _target;
	bool _hasHumans;
	int _remainingTime; // MpiAttribute
public:
	Zombie( const std::string & id );
	virtual ~Zombie();

	void updateKnowledge();
	void selectActions();
	void updateState();

	void serialize();

	// MPI related
	Zombie( void * );
	void * fillPackage(); 
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);

};

} // namespace Examples

#endif // __Zombie_hxx__


