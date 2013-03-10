
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
public:
	Village(const std::string & id);
	virtual ~Village();
	void addHerder(Herder * herder);
	void removeHerder(Herder * herder);
	void updateState();
	void registerAttributes();
	void serialize();

//	std::vector<std::string> getHerders();
//	int getId();
//	Engine::Point2D<int> getLocation();
//	bool isCitizenOfVillage(const std::string & id);
//	void setLocation(const Engine::Point2D<int> &p);

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

