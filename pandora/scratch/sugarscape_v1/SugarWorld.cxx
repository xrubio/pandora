
#include "SugarWorld.hxx"
#include "Raster.hxx"
#include "Point2D.hxx"
#include "SugarAgent.hxx"
#include "Exceptions.hxx"
#include "Statistics.hxx"
#include "gdal_priv.h"

namespace SUG
{

SugarWorld::SugarWorld( Engine::Simulation & simulation, SugarConfig & c) 
	: World(simulation, c._agentMaxVision+1, false, c._path+"/sugar.h5")
	, _numAgents(c._numAgents)
	, _agentKey(0)
	, _resourceType(c._resourceType)
	, _resourceFile(c._resourceFile)								
	, _seasonNorth(WINTER)
	, _seasonSouth(SUMMER)
	, _config(c)
{
	setSearchAgents(true);
	// overlap is maxHomeRange + 1 to allow splits to be in adjacent worlds
	// TODO code a function proces config for resources 
	
	// SUGAR RULE
	// seasonNorth = SUMMER -> WINTER
	// seasonSouth = WINTER -> SUMMER  
	// Pay attention : t=0 -> t mod g = 0 -> season change
	// assign to North the South value for coherence with change at t=0
	
}

SugarWorld::~SugarWorld()
{
}

void SugarWorld::createRasters()
{
	registerDynamicRaster("sugar",true);	
	//getStaticRaster("sugar").loadGDALFile(_config._resourceFile, *this);
	getDynamicRaster("sugar").loadGDALFile(_config._resourceFile, *this);
	
	Engine::Point2D<int> index;	
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)			
		{
			Engine::Point2D<int> index2(index - _boundaries._origin);
			int value = getValue("sugar", index2);
			setMaxValue("sugar", index2, value);			
			std::cout << "data:" << index2 << "," << value << std::endl;
		}
	}
	
	if (_config._pollutionDiffusion)
		{
			registerDynamicRaster("pollution",true);			
			getDynamicRaster("pollution").setInitValues(0,std::numeric_limits<int>::max());
			//getInitialRasters("cannibalized").setInitValues(0,std::numeric_limits<int>::max());
			// at least each cell has value 1			
		}
	registerDynamicRaster("tmpRaster",false);
	
	/*
		if (_resourceType == "file")
	    {
		getStaticRaster("sugar").loadGDALFile(_config._soilFile, *this);
		std::cout << "LOADED" << std::endl;
		}
	*/
	
}


void SugarWorld::createAgents()
{
	//SugarAgent::setConfig(_config);
	for(int i=0; i<_numAgents; i++)
	{ 
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{
			std::ostringstream oss;
			oss << "agent_" << getId() << "-" << getNewKey();	
			SugarAgent * a;
			bool located = false;
			while(!located)
			{
				Engine::Point2D<int> bornPlace(_statistics.getUniformDistValue(0,_boundaries._size._x-1), _statistics.getUniformDistValue(0,_boundaries._size._y-1));
				bornPlace += _boundaries._origin;
				if(checkPosition(bornPlace) && _boundaries.isInside(bornPlace))
				{
					a = new SugarAgent(oss.str(),this,bornPlace);
					located = true;
					std::cout << _simulation.getId() << " new agent: " << a << std::endl;
					addAgent(a);
				}
			}
			
		}
	}
	_agentKey = _numAgents;
}

void SugarWorld::replace() // R_[a,b] Agent Replacement
{		
	if(_numAgents >= _boundaries._size._x * _boundaries._size._y)
	{
		std::cout << "WARNING : no room for more agents in the raster." << std::endl; 
	}
	else
	{
		std::ostringstream oss;
		oss << "agent_" << getId() << "-" << getNewKey();	
		SugarAgent * a;
		bool located = false;
		while(!located)
		{
			Engine::Point2D<int> bornPlace(_statistics.getUniformDistValue(0,_boundaries._size._x-1), _statistics.getUniformDistValue(0,_boundaries._size._y-1));
			bornPlace += _boundaries._origin;
			if(checkPosition(bornPlace) && _boundaries.isInside(bornPlace))
			{
				a = new SugarAgent(oss.str(),this,bornPlace);
				// PENDENT : apply alpha and beta of Replace Rule
				located = true;
				std::cout << _simulation.getId() << " new agent: " << a << std::endl;
				addAgent(a);
			}
		}
	}
}
	
void SugarWorld::stepEnvironment( const std::string & key)
{
	stepRaster("sugar");
	
	// SUGAR RULE pollution Diffusion
	if(_config._pollutionDiffusion)
	{
		if(_step % _config._pollutionDiffusionAlpha)
			{
			pollutionDiffusion();
			}
	}

	// SUGAR SUBRULE seasonalChange();
	if (_config._seasonalChangeRule)
	{
		if(_step % _config._seasonalChangeStep)
		{
			SeasonCategories foo   = _seasonNorth;
			_seasonNorth = _seasonSouth;
			_seasonSouth = foo;
		}
	}
	// SUGAR RULE Seasonal Growback
	if (_config._growBackRule)
	{
		
		growBack();
		// PENDENT
		/* Seasonal Change 
		// recode this part, Backup, etc... is not correct
		if (_season == WINTER)
		{
			// PENDENT enlloc de la condició periòdica, 
			// fer _alphaGrowBack = _alphaGrowBackBackup/_betaGrowBack durant el WINTER???
			if (_step % _config._betaGrowBack) 
			{
				_alphaGrowBack = _alphaGrowBackBackup;		
			}
			else
			{
				_alphaGrowBack = 0.0;
			}
		}
		else 
		{
			_alphaGrowBack = _alphaGrowBackBackup;
		}
		*/
	}
	// optional formula : r = r + alpha - (seasonFactor * f(t,beta) * alpha)
}

void SugarWorld::growBack()
{
	
	Engine::Point2D<int> index;
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)			
		{
			Engine::Point2D<int> index2(index - _boundaries._origin);
			int value = _config._growBackAlpha + getValue("sugar", index2);
			int max   = getMaxValueAt("sugar", index2);
			if (value > max)
			{
				value = max;
			}			
			setValue("sugar", index2, value);
		}
	}
	
	
}


void SugarWorld::pollutionDiffusion()
{
	std::string key = "pollution";
	
	Engine::Point2D<int> index;
	
	Engine::Point2D<int> indexNorth;
	Engine::Point2D<int> indexSouth;
	Engine::Point2D<int> indexEast;
	Engine::Point2D<int> indexWest;
	
	index._x = _boundaries._origin._x;
	index._y = _boundaries._origin._y;
	int max  = getValue(key,index);
	for(index._x = _boundaries._origin._x; index._x < _boundaries._origin._x + _boundaries._size._x; index._x++)		
	{
		for(index._y = _boundaries._origin._y; index._y < _boundaries._origin._y + _boundaries._size._y; index._y++)			
		{
			int value = getValue(key, index);				
			// PENDENT is there maximum for pollution? int maxValue = getMaxValue(key, index);
			indexNorth._x = index._x-1;
			indexSouth._x = index._x+1;
			indexEast._x  = index._x;
			indexWest._x  = index._x;
			indexNorth._y = index._y;
			indexSouth._y = index._y;
			indexEast._y  = index._y-1;
			indexWest._y  = index._y+1;
			
			value = (value 	+ getValue(key, index) 
							+ getValue(key, indexNorth) 
							+ getValue(key, indexSouth) 
							+ getValue(key, indexEast) 
							+ getValue(key, indexWest)) / 5;
						
			/*if (value > maxValue) 
			{
				value = maxValue;
			}*/
			setValue("tmpRaster", index, value);
			if (value > max)
			{
				max = value;
			}
		}
	}
	getDynamicRaster("pollution").setMaxValue(max);//set max for pollution
	// copyRaster "tmpRaster" -> "pollution"
	getDynamicRaster("pollution").setValue(getDynamicRaster("tmpRaster"));
}


// SUGAR : stepRaster = sugarscapeGrowback
void SugarWorld::stepRaster( const std::string & key)
{
	
//std::cerr << "step" << std::endl;
	
	// SUGAR RULE Sugarscape GrowBack
		// if (_seasonalChangeRule ....) // PENDENT
		
		if(key.compare("sugar")==0 && _config._growBackRule)
		{
			Engine::Point2D<int> index;
			for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
			{
				for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)			
				{
					Engine::Point2D<int> index2(index - _boundaries._origin);					
					int value = getValue(key, index2);				
					int maxValue = getMaxValueAt(key, index2);
					value = value + _config._growBackAlpha;
					if (value > maxValue) 
					{
						value = maxValue;						
					}
					setValue(key, index2, value);
					//setValue(key, index2, maxValue);
				}
			}
		}
	
		
}

/*
 * coded inside SugarAgent::step(), accessible from World::stepAgents()
 * 
void SugarWorld::stepAgents()
{
	for(Engine::World::AgentsList::iterator it=AgentsList.begin(); it!=AgentsList.end(); it++)
	{ 		
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{
			SugarAgent *a = (SugarAgent*)*it;
			// AGENT STEP
			if (_config._matingRule)
			{
				a->_agentMating();
			}
			
			if (a->_state == DEAD && _config._replacementRule)
			{
				replace();
			}
		}	
	}
}
*/
	
Engine::Agent * SugarWorld::createAgentFromPackage( const std::string & type, void * package )
{
	if(type.compare("SugarAgent")==0)
	{
		SugarAgentPackage * formattedPackage = 0;
		try
		{
			formattedPackage = (SugarAgentPackage*)package;
		}
		catch(std::exception & e)
		{
			std::cout << "SugarWorld::createAgentFromPackage, wrong cast with exception: " << e.what() << std::endl;
		}
		return (Engine::Agent*)(new SugarAgent(*formattedPackage));
	}
	else
	{	
		std::stringstream oss;
		oss << "SugarWorld::createAgentFromPackage - unknown agent type: " << type;
		throw Engine::Exception(oss.str());
	}
}

void * SugarWorld::createPackage( const std::string & type )
{
	if(type.compare("SugarAgent")==0)
	{
		return new SugarAgentPackage;
	}
	std::stringstream oss;
	oss << "SugarWorld::createPackage - unknown agent type: " << type;
	throw Engine::Exception(oss.str());
	return 0;
}

void SugarWorld::registerTypes()
{
	_types.insert( std::make_pair( "SugarAgent", createTypeSugarAgent()));
}

MPI_Datatype * SugarWorld::createTypeSugarAgent()
{
	SugarAgentPackage package;
	
	int blockLengths[16];		// PENDENT 16 constant : NUM_PARAMS ??? apply a #define, a Simulation.numBlocks.... 
	MPI_Datatype typelist[16];

	// block lengths and types
	// 32 chars
	blockLengths[0] = 32;
	typelist[0] = MPI_CHAR;	
	for(int i=1; i<16; i++)
	{
		blockLengths[i] = 1;
		typelist[i] = MPI_INT;
	}

	MPI_Aint displacements[16];	
	MPI_Aint startAddress;
	MPI_Aint address;	
	
	MPI_Address(package._id, &startAddress); 
	displacements[0] = 0;
	MPI_Address(&package._position._x, &address);
	displacements[1] = address-startAddress;
	MPI_Address(&package._position._y, &address);
	displacements[2] = address-startAddress;
	MPI_Address(&package._exists, &address);	
	displacements[3] = address-startAddress;
	MPI_Address(&package._sugar, &address);		
	displacements[4] = address-startAddress;
	MPI_Address(&package._initialEndowment, &address);	
	displacements[5] = address-startAddress;
	MPI_Address(&package._state, &address);		
	displacements[6] = address-startAddress;
	MPI_Address(&package._sex, &address);	
	displacements[7] = address-startAddress;
	MPI_Address(&package._age, &address);	
	displacements[8] = address-startAddress;
	MPI_Address(&package._beginChildBearingAge, &address);	
	displacements[9] = address-startAddress;
	MPI_Address(&package._beginPausiaAge, &address);	
	displacements[10] = address-startAddress;
	MPI_Address(&package._vision, &address);	
	displacements[11] = address-startAddress;
	MPI_Address(&package._deathAge, &address);	
	displacements[12] = address-startAddress;
	MPI_Address(&package._metabolism, &address);	
	
	MPI_Datatype * newDataType = new MPI_Datatype;
	MPI_Type_struct(11, blockLengths, displacements, typelist, newDataType);
	MPI_Type_commit(newDataType);
	return newDataType;
}


long int SugarWorld::getNewKey()
{
	return _agentKey++;
}

} // namespace SUG

