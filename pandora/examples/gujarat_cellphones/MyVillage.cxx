#include "MyVillage.hxx"
#include <iostream>

namespace GujaratCellphones
{

MyVillage::MyVillage()
{
}

MyVillage::MyVillage(int id) : _id(id)
{
}

MyVillage::~MyVillage() {
}

void MyVillage::addCitizen(const std::string & id)
{
	_citizens.push_back(id);
}

std::vector<std::string> MyVillage::getCitizens() {
	return _citizens;
}

int MyVillage::getId() {
	return _id;
}

Engine::Point2D<int> MyVillage::getLocation() {
	return _location;
}

bool MyVillage::isCitizenOfVillage(const std::string & id)
{
	for (int i = 0; i < _citizens.size(); ++i)
	{
		if (_citizens.at(i).compare(id)==0)
		{
			return true;
		}
	}
	return false;
}

void MyVillage::setLocation(const Engine::Point2D<int> & location )
{
	_location = location;
}

}

