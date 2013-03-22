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

	int getIndex() const;

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
#ifdef PANDORAMPI
	Village( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
#endif // PANDORAMPI
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace GujaratCellphones

#endif // __Village_hxx__

