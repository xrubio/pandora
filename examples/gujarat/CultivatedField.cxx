
#include <CultivatedField.hxx>
#include <GujaratWorld.hxx>

#include <Statistics.hxx>
#include <GeneralState.hxx>

namespace Gujarat
{

CultivatedField::CultivatedField( GujaratWorld & world, const Engine::Point2D<int> & position ) 
	: _position(position), _world(world),  _sown(false), _potential(0)
{
	_world.setValue("resourceType", _position, DOMESTICATED);
}

CultivatedField::~CultivatedField()
{
}

const Engine::Point2D<int> CultivatedField::getPosition()
{
	return _position;
}

void CultivatedField::sow()
{
	_sown = true;
	_potential = 0;
}

int CultivatedField::harvest()
{
	_sown = false;
	int harvestResult = 0;
	int maxResources = _world.getValue("resources", getPosition());
	float potentialResources = (float)_potential/100.0f*(float)maxResources;
	harvestResult = Engine::GeneralState::statistics().getUniformDistValue( (int)potentialResources, maxResources);
	_potential = 0;
	return harvestResult;
}

bool CultivatedField::isSown()
{
	return _sown;
}

bool  CultivatedField::requiresFallow()
{
	return _world.getValue("resourceType", getPosition())==FALLOW;
}

bool  CultivatedField::isDomesticated()
{
	return _world.getValue("resourceType", getPosition())==DOMESTICATED;
}

void  CultivatedField::increasePotential()
{
	_potential += Engine::GeneralState::statistics().getUniformDistValue(0,5);
	if ( _potential > 100 ) _potential = 100;
}

} // namespace Gujarat

