#include <iostream>
#include <Sector.hxx>
#include <World.hxx>
#include <Exception.hxx>

#include <GujaratWorld.hxx>
#include <cassert>

namespace Gujarat
{

Sector::Sector( const Engine::World & world ) 
	: _world(world), _biomassAmount(0)
{
}

/*
Sector::Sector( const Sector& other )
{
	_world = other._world;
	_cells = other._cells;
	_biomassAmount = other._biomassAmount;
	_biomassAmountClass = other._biomassAmountClass;
}
*/

Sector::~Sector()
{
}

Engine::Point2D<int>	Sector::getNearestTo( Engine::Point2D<int> p ) const
{
	Engine::Point2D<int> nearest = _cells[0];
	double nearestDist = nearest.distance( p );

	for ( unsigned i = 1; i < _cells.size(); i++ )
	{
		double currDist = _cells[i].distance( p );
		if ( currDist < nearestDist )
		{
			nearest = _cells[i];
			nearestDist = currDist;
		} 
	} 

	return nearest;
}

void	Sector::computeBiomassAmount( const Engine::DynamicRaster& r )
{
	_biomassAmount = 0;
//	int maxBiomassAmount = r.getCurrentMaxValue();


	// TODO refactor
	for ( unsigned i = 0; i < _cells.size(); i++ )
	{
		_biomassAmount += r.getValue( _cells[i]-_world.getBoundaries()._origin );
	}
/*	
	double normAmount = (double)_biomassAmount;
	if ( maxBiomassAmount > 0 )
		normAmount /= ((double)_cells.size()*maxBiomassAmount);
	else
		normAmount = 0.0;

	if ( normAmount <= 1.0/3.0)
		_biomassAmountClass = BIOMASS_AMOUNT_LOW;
	else if ( normAmount <= 2.0/3.0 )
		_biomassAmountClass = BIOMASS_AMOUNT_MED;
	else
		_biomassAmountClass = BIOMASS_AMOUNT_HI;
	*/
}

void	Sector::updateFeatures( const Engine::DynamicRaster& r )
{
	computeBiomassAmount( r ); 
}

void Sector::updateFeatures()
{
	computeBiomassAmount(_world.getDynamicRaster(eResources));
}
/*
std::string Sector::biomassClass() const
{
	std::string bioclass;
	switch( (unsigned)_biomassAmountClass ) 
	{
	case BIOMASS_AMOUNT_LOW :
		bioclass = "LOW";
		break;
	case BIOMASS_AMOUNT_MED :
		bioclass = "MED";
		break;
	case BIOMASS_AMOUNT_HI :
		bioclass = "HI";
		break;
	}
	return bioclass;	
}
*/

/*
void	Sector::showFeatures( std::ostream& stream )
{
	stream << "\tFeature: BioMassAmount: " << _biomassAmount << std::endl;
	
	std::string bioclass;
	switch( (unsigned)_biomassAmountClass ) 
	{
	case BIOMASS_AMOUNT_LOW :
		bioclass = "LOW";
		break;
	case BIOMASS_AMOUNT_MED :
		bioclass = "MED";
		break;
	case BIOMASS_AMOUNT_HI :
		bioclass = "HI";
		break;
	}
	stream << "\tFeature: BioMassAmountClass: " << bioclass << std::endl;
}
*/

void	Sector::getAdjacent( Engine::Point2D<int> p, std::vector<Engine::Point2D<int> >& adjList ) const
{

	for ( unsigned i = 0; i < _cells.size(); i++ )
	{
		Engine::Point2D<int> delta = p - _cells[i];
		delta._x = abs(delta._x);
		delta._y = abs(delta._y);
		if ( delta._x <= 1 && delta._y <= 1 )
			adjList.push_back( _cells[i] );
	}	
}
	
const Engine::World & Sector::getWorld() const
{
	return _world;
}

} // namespace Gujarat

