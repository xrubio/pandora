#ifndef __Village_hxx__
#define __Village_hxx__

#include <list>
#include <Point2D.hxx>
#include <Agent.hxx>

namespace GujaratCellphones
{
class Herder;

class Village : public Engine::Agent
{	
	std::list<Herder*> _herders;
	//probability of exchanging information when two agents talk to each other
	int _inVillageTransmission; // MpiBasicAttribute 
	int _outVillageTransmission;  // MpiBasicAttribute 
	int _index; // MpiBasicAttribute
	long int _agentKey;
public:
	Village(const std::string & id, int inedx);
	virtual ~Village();
	void updateState();

	void setInVillageTransmission( int inVillageTransmission);
	int getInVillageTransmission() const;
	
	void setOutVillageTransmission( int outVillageTransmission);
	int getOutVillageTransmission() const;

	void addHerder(Herder * herder);
	void removeHerder(Herder * herder);

	std::list<Herder*>::iterator beginHerders();
	std::list<Herder*>::iterator endHerders();

	void registerAttributes();
	void serialize();
	long int getNewKey();

	int getIndex() const;
	void fission( Herder & original );

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Village( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace GujaratCellphones

#endif // __Village_hxx__

