#ifndef __Village_hxx__
#define __Village_hxx__

#include <vector>
#include <Point2D.hxx>
#include <Agent.hxx>

namespace GujaratCellphones
{
class Herder;

class Village : public Engine::Agent
{	
	std::list<Herder*> _herders;
	//probability of exchanging information when two agents talk to each other
	int _shareKnowledge; 

public:
	Village(const std::string & id);
	virtual ~Village();
	void updateState();

	void setShareKnowledge( int shareKnowledge );
	int getShareKnowledge();
	void addHerder(Herder * herder);
	void removeHerder(Herder * herder);
	Herder* getRandomHerder( const std::string & id ); 

	void registerAttributes();
	void serialize();

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

