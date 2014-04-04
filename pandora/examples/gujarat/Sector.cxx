#include <iostream>
#include <Sector.hxx>
#include <World.hxx>
#include <Exceptions.hxx>

#include <GujaratWorld.hxx>
#include <cassert>

#include <Logger.hxx>

namespace Gujarat
{

	//*? debugging
	int Sector::pop = 0;
	
Sector::Sector( const GujaratWorld & world ) 
	: _world(world), _biomassAmount(0), _direction(-1),_numInterDunes(0)
{
	_dni = dniTicket();
	
	incSects();
}


Sector::Sector( const Sector* other ):
	_world(other->_world)
	, _cells(other->_cells)
	, _biomassAmount(other->_biomassAmount)
	, _direction(other->_direction)
	, _numInterDunes(other->_numInterDunes)
{
	//_biomassAmountClass = other._biomassAmountClass;
	_dni = dniTicket();
	
	//std::cout << "SECTOR COPY: " << other->_dni << " -> " << _dni << std::endl;
	
	//std::cout << "SECTOR COPIED CELLS: " << _cells.size() << std::endl ;
	
	incSects();
}


Sector::~Sector()
{
	/*std::stringstream logName;
	logName << "infoshar";	
	log_INFO(logName.str(),"DELETING Sector; " 
					<< " population " << pop 
					<< " direction " << _direction);*/
	
	
	_cells.clear();
	
	decSects();
	
	//std::cout << "SECTOR DELETE: " << _dni << " direction:" << _direction << std::endl;
	
}

Engine::Point2D<int> *	Sector::getNearestTo( Engine::Point2D<int> p ) const
{
	assert(_cells.size()>0);
	Engine::Point2D<int> * nearest = _cells[0];
	double nearestDist = nearest->distance( p );

	for ( unsigned i = 1; i < _cells.size(); i++ )
	{
		double currDist = _cells[i]->distance( p );
		if ( currDist < nearestDist )
		{
			nearest = _cells[i];
			nearestDist = currDist;
		} 
	} 

	return nearest;
}

void	Sector::computeBiomassAmount( const Engine::Raster& r )
{
	_biomassAmount = 0;
//	int maxBiomassAmount = r.getCurrentMaxValue();

	// TODO refactor
	for ( unsigned i = 0; i < _cells.size(); i++ )
	{
		
		//std::cout << "INCREASE UTILITY " << r.getValue( _cells[i]-_world.getOverlapBoundaries()._origin ) << std::endl;
		_biomassAmount += r.getValue( *(_cells[i]) - _world.getOverlapBoundaries()._origin);
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


void Sector::computeBiomassAmountLR( const Engine::Raster& r )
{
	_biomassAmount = 0;
//	int maxBiomassAmount = r.getCurrentMaxValue();

	
	// TODO refactor
	for ( unsigned i = 0; i < _cells.size(); i++ )
	{
		
		//std::cout << "INCREASE UTILITY " << r.getValue( (*_cells[i])-_world.getOverlapBoundaries()._origin ) << std::endl;
		
		//std::cout << "[" << std::endl;
		_biomassAmount += r.getValue( *_cells[i] );
		
		//std::cout << "INCREASE UTILITY2 " << _world.getValueGW(r,*_cells[i]) << std::endl;
		
		//_world.getValueLR(r,_cells[i]);
		//std::cout << "]" << std::endl;
		//r.getValue( _cells[i]-_world.getOverlapBoundaries()._origin );
	}
}

void Sector::updateFeaturesLR( const Engine::Raster& r )
{
	computeBiomassAmountLR( r ); 
}

void Sector::updateFeatures( const Engine::Raster& r )
{
	computeBiomassAmount( r ); 
}

void Sector::updateFeatures()
{
	computeBiomassAmount(_world.getConstDynamicRaster(eResources));
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

void	Sector::getAdjacent( Engine::Point2D<int> p, std::vector<Engine::Point2D<int>* >& adjList ) const
{

	for ( unsigned i = 0; i < _cells.size(); i++ )
	{
		Engine::Point2D<int> delta = p - *_cells[i];
		delta._x = abs(delta._x);
		delta._y = abs(delta._y);
		if ( delta._x <= 1 && delta._y <= 1 )
			adjList.push_back( _cells[i] );
	}	
}
	
const GujaratWorld & Sector::getWorld() const
{
	return _world;
}



} // namespace Gujarat

