
#include <SettlementAreas.hxx>

#include <queue>
#include <GujaratWorld.hxx>
#include <assert.h>
#include <math.h>

#include <GeneralState.hxx>
#include <Logger.hxx>

namespace Gujarat
{

SettlementAreas::SettlementAreas()
{		
}

SettlementAreas::~SettlementAreas()
{	
}


/*
void SettlementAreas::updateArea( const Engine::Point2D<int> & newPoint, Engine::Rectangle<int> & r)
{

	if (newPoint._x < r._origin._x)
	{
		r._size._width += r._origin._x - newPoint._x;
		r._origin._x = newPoint._x;
	}
	else if (newPoint._x > r._origin._x + (r._size._width -1) )
	{
		r._size._width = newPoint._x - r._origin._x + 1;    
	}

	if (newPoint._y < r._origin._y)
	{
		r._size._height += r._origin._y - newPoint._y;
		r._origin._y = newPoint._y;
	}
	else if (newPoint._y > r._origin._y + (r._size._height -1) )
	{
		r._size._height = newPoint._y - r._origin._y + 1;    
	}
}
*/
float SettlementAreas::computeAreaScore(const Engine::Rectangle<int> & newArea, GujaratWorld &w)
{
	int numInterdune = 0;
	int numCells = 0;

	Engine::Point2D<int> index;
	for(index._x = newArea._origin._x; index._x < newArea._origin._x + newArea._size._width ; index._x++)
	{
		for(index._y = newArea._origin._y; index._y < newArea._origin._y + newArea._size._height ; index._y++)
		{	
			numCells++;
			if (w.getValue(eSoils,index) == INTERDUNE)
			{
				numInterdune++;
			}
		}
	}
	if(!numCells)
	{
		return 0.0f;
	}
	return (float)numInterdune/numCells;
}

void SettlementAreas::testDuneInside( const Engine::Rectangle<int> & newArea, GujaratWorld & w )
{
	Engine::Point2D<int> insidePosition(newArea._origin);
	for(insidePosition._x=newArea._origin._x; insidePosition._x<newArea._origin._x+newArea._size._width; insidePosition._x++)
	{
		for(insidePosition._y=newArea._origin._y; insidePosition._y<newArea._origin._y+newArea._size._height; insidePosition._y++)
		{
			// if only one point is dune, add new area
			if(w.getValue(eSoils, insidePosition)==DUNE)
			{
				_areas.push_back(newArea);
				_scoreAreas.push_back(computeAreaScore(newArea,w));
				//std::cout << "created area with rectangle: " << newArea << " and score: " << computeAreaScore(newArea,w) << std::endl;				
				return;
			}
		}
	}
	//std::cout << "area without dune with rectangle: " << newArea << std::endl;				
}

void SettlementAreas::generateAreas(GujaratWorld &w, int lowResolution)
{
	std::stringstream logName;
	logName << "SettlementAreas_world_" << w.getId();

	Engine::Point2D<int> position;
	for(position._x=w.getBoundaries()._origin._x; position._x<w.getBoundaries()._origin._x+w.getBoundaries()._size._width; position._x+=lowResolution)
	{
		for(position._y=w.getBoundaries()._origin._y; position._y<w.getBoundaries()._origin._y+w.getBoundaries()._size._height; position._y+=lowResolution)
		{
			log_DEBUG(logName.str(), "checking position: " << position << " with resolution: " << lowResolution);
			if(!w.getBoundaries().contains(position))
			{
				continue;
			}
			Engine::Rectangle<int> newArea(Engine::Size<int>(lowResolution,lowResolution), position);

			// adjust border
			if(position._x+lowResolution>=w.getBoundaries()._origin._x+w.getBoundaries()._size._width)
			{
				newArea._size._width = w.getBoundaries()._origin._x+w.getBoundaries()._size._width-position._x;
			}
			if(position._y+lowResolution>=w.getBoundaries()._origin._y+w.getBoundaries()._size._height)
			{
				newArea._size._height = w.getBoundaries()._origin._y+w.getBoundaries()._size._height-position._y;
			}
			testDuneInside(newArea, w);
		}
	}
	log_DEBUG(logName.str(), "generate areas done");
}

void SettlementAreas::intersectionFilter(Engine::Rectangle<int> & r, std::vector<int> & candidates) const
{	
	// looping in search of candidates
	Engine::Rectangle<int> intersection;
	for(unsigned long i=0; i < _areas.size(); i++)
	{
		if(r.intersection(_areas[i],intersection))
		{
		//	if( insideTheCircle(intersection,a._location,a._homeRange) )
		//		insideTheCircle, an extra filter, future issue	
			candidates.push_back(i);
		}
	}
}
		
}//namespace
