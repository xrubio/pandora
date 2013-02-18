#include <MyVillage.hxx>

MyVillage::MyVillage() {
}

MyVillage::MyVillage(int id) {
	_id = id;
}

MyVillage::~MyVillage() {
}

int MyVillage::getId() {
	return _id;
}

std::vector<std::string> MyVillage::getCitizens() {
	return _citizens;
}

Engine::Point2D<int> MyVillage::getLocation() {
	return _location;
}

void MyVillage::setLocation(const Engine::Point2D<int> &p) {
	_location = p;
}

bool MyVillage::isCitizenOfVillage(std::string id) {
	for (int i = 0; i < _citizens.size(); ++i) {
		if (_citizens[i] == id) return true;
	}
	return false;
}

void MyVillage::addCitizen(std::string id) {
	_citizens.push_back(id);
}
