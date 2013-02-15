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

std::vector<int> MyVillage::getCitizens() {
	return _citizens;
}

Engine::Point2D<int> MyVillage::getLocation() {
	return _location;
}

void MyVillage::setLocation(const Engine::Point2D<int> &p) {
	_location = p;
}

bool MyVillage::isCitizenOfVillage(int id) {
	for (int i = 0; i < _citizens.size(); ++i) {
		if (_citizens[i] == id) return true;
	}
	return false;
}
