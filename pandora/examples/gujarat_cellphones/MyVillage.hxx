#ifndef __MyVillage_hxx__
#define __MyVillage_hxx__

#include <vector>
#include <Point2D.hxx>

class MyVillage
{	
	int _id;
	std::vector<std::string> _citizens;
	Engine::Point2D<int> _location;

public:
	MyVillage();
	MyVillage(int id);
	virtual ~MyVillage();

	int getId();
	std::vector<std::string> getCitizens();
	Engine::Point2D<int> getLocation();
	bool isCitizenOfVillage(std::string id);
	void setLocation(const Engine::Point2D<int> &p);
	void addCitizen(std::string id);
};


#endif 
