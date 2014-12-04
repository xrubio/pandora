
/*
 * Copyright (c) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software; 
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or (at your option) any later version.
 * 
 * Pandora is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <typedefs.hxx>
#include <Point2D.hxx>
#include <Size.hxx>
#include <Rectangle.hxx>
#include <StaticRaster.hxx>
#include <DynamicRaster.hxx>
#include <World.hxx>
#include <Config.hxx>
#include <Agent.hxx>
#include <AgentRecord.hxx>
#include <SimulationRecord.hxx>

#include <analysis/Output.hxx>
#include <analysis/Analysis.hxx>
#include <analysis/IndividualStats.hxx>
#include <analysis/GlobalAgentStats.hxx>
#include <analysis/GlobalRasterStats.hxx>

#include <analysis/AgentMean.hxx>
#include <analysis/AgentSum.hxx>
#include <analysis/AgentNum.hxx>
#include <analysis/AgentStdDev.hxx>
#include <analysis/AgentHDFtoSHP.hxx>

#include <analysis/RasterSum.hxx>
#include <analysis/RasterMean.hxx>
#include <SpacePartition.hxx>
#include <OpenMPSingleNode.hxx>
#include <Scheduler.hxx>
#include <GeneralState.hxx>
#include <ShpLoader.hxx>
#include <RasterLoader.hxx>

#include <string>
#include <memory>

typedef Engine::Point2D<int> Point2DInt;
typedef Engine::Size<int> SizeInt;
typedef Engine::Rectangle<int> RectangleInt;


class ConfigWrap : public Engine::Config, public boost::python::wrapper<Engine::Config>
{
public:
	ConfigWrap( const Engine::Size<int> & size = Engine::Size<int>(0,0), const int & numSteps = 1, const std::string & resultsFile = "data/results.h5", const int & serializerResolution = 1 ) : Config(size, numSteps, resultsFile, serializerResolution)
    {
    }
	ConfigWrap( const std::string & configFile ) : Config(configFile)
    {
    }
    
    void loadParams()
	{
		if (boost::python::override loadParams = this->get_override("loadParams"))
		{
			loadParams();
			return;
		}
		Engine::Config::loadParams();
	}

	void default_LoadParams()
	{
		Engine::Config::loadParams();
	}
};

class AgentWrap : public Engine::Agent, public boost::python::wrapper<Engine::Agent>
{
public:
	AgentWrap( const std::string & id ) : Agent(id)
	{
	}

	void serialize()
	{
		this->get_override("serialize")();
	}
	void updateState()
	{
		if (boost::python::override updateState = this->get_override("updateState"))
		{
			updateState();
			return;
		}
		Engine::Agent::updateState();
	}

	void default_UpdateState()
	{
		Engine::Agent::updateState();
	}

	void * fillPackage()
	{
		return 0;
	}

	void sendVectorAttributes(int target)
	{
	}
	
	void receiveVectorAttributes(int origin)
	{		
	}

	void serializeIntAttribute( const std::string & key, int value )
	{
		serializeAttribute(key, value);
	}

	void serializeFloatAttribute( const std::string & key, float value )
	{
		serializeAttribute(key, value);
	}

    void serializeStringAttribute( const std::string & key, const std::string & value )
    {
        serializeAttribute(key,value);
    }
	
	void registerAttributes()
	{
		if (boost::python::override registerAttributes = this->get_override("registerAttributes"))
		{
			registerAttributes();
			return;
		}
		Engine::Agent::registerAttributes();
	}

	void default_RegisterAttributes()
	{
		Engine::Agent::registerAttributes();
	}
};

class WorldWrap : public Engine::World, public boost::python::wrapper<Engine::World>
{
public:
	// Scheduler for python is always default
	WorldWrap(std::shared_ptr<ConfigWrap> config, Engine::Scheduler * scheduler = 0, const bool & allowMultipleAgentsPerCell = true) : World( 0, scheduler, allowMultipleAgentsPerCell )
	{
        if(config)
        {
            config->loadFile();
        }
        // workaround; pointer ownership trasmission during constructor does not work in boost::python
        configureSharedPtr(config);
	}

	void createRasters()
	{      
        if (boost::python::override createRasters = this->get_override("createRasters"))
		{
			createRasters();
			return;
		}
		Engine::World::createRasters();
	}	
    void default_createRasters()
    {
        World::createRasters();
    }

	void createAgents()
	{
        if (boost::python::override createAgents = this->get_override("createAgents"))
		{
			createAgents();
			return;
		}
		Engine::World::createAgents();
	}
    void default_createAgents()
    {
        World::createAgents();
    }

	void stepEnvironment()
	{
		if (boost::python::override stepEnvironment = this->get_override("stepEnvironment"))
		{
			stepEnvironment();
			return;
		}
		Engine::World::stepEnvironment();
	}
	void default_StepEnvironment()
	{
		World::stepEnvironment();
	}

	// needed to avoid expliciting default params
	void registerDynamicRasterSimple( const std::string & key, const bool & serialize )
	{
		Engine::World::registerDynamicRaster(key, serialize, -1);			
	}
	
	// needed to avoid expliciting default params
	void registerStaticRasterSimple( const std::string & key, const bool & serialize )
	{
		Engine::World::registerStaticRaster(key, serialize, -1);			
	}
	
	void addAgentSimple( std::shared_ptr<AgentWrap> agent)
	{
        agent->setWorld(this);
    	_agents.push_back(agent);
	}

    void configureSharedPtr( std::shared_ptr<ConfigWrap> config )
    {
        _config = config;
    }

	std::vector<std::string> getNeighboursIds( Engine::Agent & target, const double & radius, const std::string & type="all" )
	{
		std::vector<std::string> agentIds;
		Engine::AgentsVector agents = getNeighbours(&target, radius, type);
		agentIds.resize(agents.size());
		for(size_t i=0; i<agents.size(); i++)
		{
			agentIds.at(i) = agents.at(i)->getId();
		}
		return agentIds;
	}

	std::vector<std::string> getAgentIds(  const Engine::Point2D<int> & position, const std::string & type="all" )
	{
		std::vector<std::string> agentIds;
		Engine::AgentsVector agents = getAgent(position, type);
		agentIds.resize(agents.size());
		for(size_t i=0; i<agents.size(); i++)
		{
			agentIds.at(i) = agents.at(i)->getId();
		}
		return agentIds;
	}
	
	void initializeNoArguments()
	{
		initialize(0,0);
	}
};

class AgentAnalysisWrap : public PostProcess::AgentAnalysis, public boost::python::wrapper<PostProcess::AgentAnalysis>
{
public:
	AgentAnalysisWrap( const std::string & name, bool writeResults ) : AgentAnalysis(name, writeResults)
	{
	}

	void computeAgent( const Engine::AgentRecord & record )
	{
		this->get_override("computeAgent")(record);
	}
};

class RasterAnalysisWrap : public PostProcess::RasterAnalysis, public boost::python::wrapper<PostProcess::RasterAnalysis>
{
public:
	RasterAnalysisWrap( const std::string & name, bool writeResults ) : RasterAnalysis(name, writeResults)
	{
	}

	void computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory )
	{
		this->get_override("computeRaster")(rasterHistory);
	}
};

void passAgentAnalysisOwnership( PostProcess::GlobalAgentStats & results, std::shared_ptr<PostProcess::AgentAnalysis> analysisPtr )
{
	results.addAnalysis(analysisPtr);
}

void passRasterAnalysisOwnership( PostProcess::GlobalRasterStats & results, std::shared_ptr<PostProcess::RasterAnalysis> analysisPtr )
{
	results.addAnalysis(analysisPtr);
}

std::string printPoint( const Point2DInt & point )
{
	std::stringstream stream;
	stream << point;
	return stream.str();
}

bool equalsPoint( const Point2DInt & pointA, const Point2DInt & pointB )
{
	return pointA==pointB;
}

bool notEqualsPoint( const Point2DInt & pointA, const Point2DInt & pointB )
{
	return pointA!=pointB;

}

std::string printSize( const SizeInt & size )
{
	std::stringstream stream;
	stream << size;
	return stream.str();
}

bool equalsSize( const SizeInt & sizeA, const SizeInt & sizeB )
{
	return sizeA==sizeB;
}

bool notEqualsSize( const SizeInt & sizeA, const SizeInt & sizeB )
{
	return sizeA!=sizeB;
}

std::string printRectangle( const RectangleInt & rectangle )
{
	std::stringstream stream;
	stream << rectangle;
	return stream.str();
}

bool equalsRectangle( const RectangleInt & rectangleA, const RectangleInt & rectangleB )
{
	return rectangleA==rectangleB;
}

bool notEqualsRectangle( const RectangleInt & rectangleA, const RectangleInt & rectangleB )
{
	return rectangleA!=rectangleB;
}

// overloaded methods
Engine::DynamicRaster & (Engine::World::*getDynamicRaster)(const std::string&) = &Engine::World::getDynamicRaster;
Engine::StaticRaster & (Engine::World::*getStaticRaster)(const std::string&) = &Engine::World::getStaticRaster;

int (Engine::World::*getValue)(const std::string&, const Engine::Point2D<int> &) const = &Engine::World::getValue;
void (Engine::World::*setValue)(const std::string&, const Engine::Point2D<int> &, int) = &Engine::World::setValue;
void (Engine::World::*setMaxValue)(const std::string&, const Engine::Point2D<int> &, int) = &Engine::World::setMaxValue;

Engine::Agent * (Engine::World::*getAgent)(const std::string &) = &Engine::World::getAgent;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(fillGDALRasterOverloads, fillGDALRaster, 2, 3)

BOOST_PYTHON_MODULE(libpyPandora)
{
	boost::python::class_< Point2DInt >("Point2DIntStub", boost::python::init<const int & , const int & >() )
		.def_readwrite("_x", &Point2DInt::_x) 
		.def_readwrite("_y", &Point2DInt::_y) 
		.def("distance", &Point2DInt::distance)
		.def("__str__", printPoint)
		.def("__eq__", equalsPoint)
		.def("__ne__", notEqualsPoint)
		.def("clone", &Point2DInt::clone)
	;	
	
	boost::python::class_< SizeInt>("SizeIntStub", boost::python::init<const int & , const int & >() )
		.def_readwrite("_width", &SizeInt::_width) 
		.def_readwrite("_height", &SizeInt::_height) 
		.def("__str__", printSize)
		.def("__eq__", equalsSize)
		.def("__ne__", notEqualsSize)
		.def("clone", &SizeInt::clone)
	;	

	boost::python::class_< RectangleInt>("RectangleIntStub", boost::python::init< const SizeInt &, const Point2DInt & >() )
		.def_readwrite("_origin", &RectangleInt::_origin) 
		.def_readwrite("_size", &RectangleInt::_size) 
		.def("contains", &RectangleInt::contains) 
		.def("__str__", printRectangle)
		.def("__eq__", equalsRectangle)
		.def("__ne__", notEqualsRectangle)
		.add_property("left", &RectangleInt::left)
		.add_property("right", &RectangleInt::right)
		.add_property("top", &RectangleInt::top)
		.add_property("bottom", &RectangleInt::bottom)
		.def("clone", &RectangleInt::clone)
		;

    boost::python::class_< Engine::StaticRaster, std::shared_ptr< Engine::StaticRaster> >("StaticRasterStub")
		.def("resize", &Engine::StaticRaster::resize)
		.def("getSize", &Engine::StaticRaster::getSize)
		.def("getValue", boost::python::make_function(&Engine::StaticRaster::getValue, boost::python::return_value_policy<boost::python::copy_const_reference>()))

	;
    
    boost::python::class_< Engine::DynamicRaster, std::shared_ptr< Engine::DynamicRaster>, boost::python::bases<Engine::StaticRaster> >("DynamicRasterStub")
		.def("setInitValues", &Engine::DynamicRaster::setInitValues) 
		.def("setValue", &Engine::DynamicRaster::setValue)	
		.def("resize", &Engine::DynamicRaster::resize)
		.def("getSize", &Engine::DynamicRaster::getSize)
		.def("getValue", boost::python::make_function(&Engine::DynamicRaster::getValue, boost::python::return_value_policy<boost::python::copy_const_reference>()))
	;


	boost::python::class_< ConfigWrap, std::shared_ptr<ConfigWrap>, boost::noncopyable >("ConfigStub", boost::python::init< const Engine::Size<int> &, const int &, const std::string &, const int & >())
        .def(boost::python::init< const std::string &>())
		.add_property("size", boost::python::make_function(&ConfigWrap::getSize, boost::python::return_value_policy<boost::python::reference_existing_object>()))
		.add_property("numSteps", boost::python::make_function(&ConfigWrap::getNumSteps, boost::python::return_value_policy<boost::python::copy_const_reference>()))
		.add_property("serializerResolution", boost::python::make_function(&ConfigWrap::getSerializeResolution, boost::python::return_value_policy<boost::python::copy_const_reference>()))
		.def("loadParams", &Engine::Config::loadParams, &ConfigWrap::default_LoadParams)
		.def("getParamStr", &Engine::Config::getParamStr)
		.def("getParamInt", &Engine::Config::getParamInt)
		.def("getParamLongInt", &Engine::Config::getParamLongInt)
		.def("getParamFloat", &Engine::Config::getParamFloat)
		.def("getParamBool", &Engine::Config::getParamBool)
	;

	boost::python::class_< Engine::ShpLoader>("ShpLoaderStub")
		.def("open", &Engine::ShpLoader::open)	
		.def("getNumLayers", &Engine::ShpLoader::getNumLayers)	
		.def("setActiveLayer", &Engine::ShpLoader::setActiveLayer)	
		.def("getNumFeatures", &Engine::ShpLoader::getNumFeatures)	
		.def("getPosition", &Engine::ShpLoader::getPosition)	
		.def("getFieldAsString", &Engine::ShpLoader::getFieldAsString)	
		.def("getFieldAsInt", &Engine::ShpLoader::getFieldAsInt)	
		.def("getFieldAsFloat", &Engine::ShpLoader::getFieldAsFloat)	
	;

    boost::python::class_< Engine::RasterLoader>("RasterLoaderStub")
	.def("fillGDALRaster", &Engine::RasterLoader::fillGDALRaster, fillGDALRasterOverloads())	
	;

    
    boost::python::class_< Engine::GeneralState, std::shared_ptr<Engine::GeneralState>, boost::noncopyable>("GeneralStateStub", boost::python::no_init)
		.add_property("instance", boost::python::make_function(&Engine::GeneralState::instance, boost::python::return_value_policy<boost::python::reference_existing_object>()))
		.def("shpLoader", boost::python::make_function(&Engine::GeneralState::shpLoader, boost::python::return_value_policy<boost::python::reference_existing_object>()))
		.def("rasterLoader", boost::python::make_function(&Engine::GeneralState::rasterLoader, boost::python::return_value_policy<boost::python::reference_existing_object>()))
    ;

	boost::python::class_< AgentWrap, std::shared_ptr<AgentWrap>, boost::noncopyable >("AgentStub", boost::python::init< const std::string & > () )
		.def("updateState", &Engine::Agent::updateState, &AgentWrap::default_UpdateState)
		.def("registerAttributes", &Engine::Agent::registerAttributes, &AgentWrap::default_RegisterAttributes)
		.def("getWorld", &Engine::Agent::getWorldRef, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.def("serialize", boost::python::pure_virtual(&Engine::Agent::serialize))
		.def("registerIntAttribute", &Engine::Agent::registerIntAttribute)
		.def("serializeIntAttribute", &AgentWrap::serializeIntAttribute)
        .def("registerStringAttribute", &Engine::Agent::registerStringAttribute)
		.def("serializeStringAttribute", &AgentWrap::serializeStringAttribute)
        .def("registerFloatAttribute", &Engine::Agent::registerFloatAttribute)
		.def("serializeFloatAttribute", &AgentWrap::serializeFloatAttribute)
		.def("remove", &Engine::Agent::remove)
		.def("setRandomPosition", &Engine::Agent::setRandomPosition)
		.add_property("id", boost::python::make_function(&Engine::Agent::getId, boost::python::return_value_policy<boost::python::copy_const_reference>()))
		.add_property("exists", &Engine::Agent::exists)
		.add_property("_type", &Engine::Agent::getType)
		.add_property("position", boost::python::make_function(&Engine::Agent::getPosition, boost::python::return_value_policy<boost::python::reference_existing_object>()), &Engine::Agent::setPosition )
	;
	
	boost::python::class_< std::vector<std::string> >("StringVector").def(boost::python::vector_indexing_suite< std::vector<std::string> >());
	boost::python::class_< std::vector<int> >("IntVector").def(boost::python::vector_indexing_suite< std::vector<int> >());
	
	boost::python::class_< WorldWrap, boost::noncopyable >("WorldStub", boost::python::init< std::shared_ptr<ConfigWrap>, Engine::Scheduler *, const bool & >()[boost::python::with_custodian_and_ward<1,2>(),boost::python::with_custodian_and_ward<1,3>()])
		.def("createRasters", &Engine::World::createRasters, &WorldWrap::default_createRasters)
		.def("createAgents", &Engine::World::createAgents, &WorldWrap::default_createAgents)
		.def("stepEnvironment", &Engine::World::stepEnvironment, &WorldWrap::default_StepEnvironment)
		.def("initialize", &WorldWrap::initializeNoArguments)
		.def("checkPosition", &Engine::World::checkPosition)
		.def("registerDynamicRaster", &WorldWrap::registerDynamicRasterSimple)	
		.def("registerStaticRaster", &WorldWrap::registerStaticRasterSimple)	
		.def("getDynamicRaster", getDynamicRaster, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.def("getStaticRaster", getStaticRaster, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.def("run", &Engine::World::run)
		.def("useSpacePartition", &Engine::World::useSpacePartition, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.staticmethod("useSpacePartition")
		.def("useOpenMPSingleNode", &Engine::World::useOpenMPSingleNode, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.staticmethod("useOpenMPSingleNode")
		.def("addAgent", &WorldWrap::addAgentSimple,boost::python::with_custodian_and_ward<1,2>())
		.def("setValue", setValue)
		.def("setMaxValue", setMaxValue)
		.def("getValue", getValue)
		.def("getAgentIds", &WorldWrap::getAgentIds)
		.def("getNeighboursIds", &WorldWrap::getNeighboursIds)
		.def("getAgent", getAgent, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.def("getNumberOfRasters", &Engine::World::getNumberOfRasters)
		.def("getNumberOfAgents", &Engine::World::getNumberOfAgents)
		.def("getBoundaries", &Engine::World::getBoundaries, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.add_property("currentStep", &Engine::World::getCurrentStep)
		.add_property("config", boost::python::make_function(&Engine::World::getConfig, boost::python::return_value_policy<boost::python::reference_existing_object>()))
	;	
	
	boost::python::class_< Engine::SpacePartition, std::shared_ptr<Engine::SpacePartition> >("SpacePartitionStub", boost::python::init< const int &, bool >())
	;
	
	boost::python::implicitly_convertible< std::shared_ptr< Engine::SpacePartition >, std::shared_ptr< Engine::Scheduler > >();	

	boost::python::class_< Engine::OpenMPSingleNode, std::shared_ptr<Engine::OpenMPSingleNode> >("OpenMPSingleNodeStub", boost::python::init<>())
	;
	
	boost::python::implicitly_convertible< std::shared_ptr< Engine::OpenMPSingleNode >, std::shared_ptr< Engine::Scheduler > >();	


	boost::python::class_< Engine::SimulationRecord>("SimulationRecordStub", boost::python::init< int, bool >())
		.def("loadHDF5", &Engine::SimulationRecord::loadHDF5)
	;

	boost::python::class_< PostProcess::GlobalAgentStats>("GlobalAgentsStatsStub", boost::python::init< const std::string & > ())
		.def("addAnalysis", &passAgentAnalysisOwnership,boost::python::with_custodian_and_ward<1,2>())
		.def("applyTo", &PostProcess::GlobalAgentStats::apply)
	;
	
	boost::python::class_< PostProcess::GlobalRasterStats>("GlobalRasterStatsStub", boost::python::init< const std::string & > ())
		.def("addAnalysis", &passRasterAnalysisOwnership,boost::python::with_custodian_and_ward<1,2>())
		.def("applyTo", &PostProcess::GlobalRasterStats::apply)
	;

	boost::python::class_< PostProcess::Analysis, std::shared_ptr<PostProcess::Analysis> >("AnalysisStub", boost::python::init< const std::string &, bool >() )
	;
	
	boost::python::class_< AgentAnalysisWrap, std::shared_ptr<AgentAnalysisWrap> , boost::python::bases<PostProcess::Analysis>, boost::noncopyable >("AgentAnalysisStub", boost::python::init< const std::string &, bool >() )
		.def("computeAgent", boost::python::pure_virtual(&PostProcess::AgentAnalysis::computeAgent))
	;
	boost::python::implicitly_convertible< std::shared_ptr< PostProcess::AgentAnalysis >, std::shared_ptr< PostProcess::Analysis > >();	

	// raster analysis
	boost::python::class_< RasterAnalysisWrap, std::shared_ptr<RasterAnalysisWrap> , boost::python::bases<PostProcess::Analysis>, boost::noncopyable >("RasterAnalysisStub", boost::python::init< const std::string &, bool >() )
		.def("computeRaster", boost::python::pure_virtual(&PostProcess::RasterAnalysis::computeRaster))
	;
	boost::python::implicitly_convertible< std::shared_ptr< PostProcess::RasterAnalysis >, std::shared_ptr< PostProcess::Analysis > >();	

	boost::python::class_< PostProcess::AgentNum, std::shared_ptr< PostProcess::AgentNum> , boost::python::bases<PostProcess::AgentAnalysis> >("AgentNumStub");
	boost::python::implicitly_convertible< std::shared_ptr< PostProcess::AgentNum >, std::shared_ptr< PostProcess::AgentAnalysis > >();

	boost::python::class_< PostProcess::AgentMean, std::shared_ptr< PostProcess::AgentMean> , boost::python::bases<PostProcess::AgentAnalysis> >("AgentMeanStub", boost::python::init< const std::string & >() );
	boost::python::implicitly_convertible< std::shared_ptr< PostProcess::AgentMean >, std::shared_ptr< PostProcess::AgentAnalysis > >();

	boost::python::class_< PostProcess::AgentSum, std::shared_ptr< PostProcess::AgentSum> , boost::python::bases<PostProcess::AgentAnalysis> >("AgentSumStub", boost::python::init< const std::string & >() );
	boost::python::implicitly_convertible< std::shared_ptr< PostProcess::AgentSum >, std::shared_ptr< PostProcess::AgentAnalysis > >();
	
	boost::python::class_< PostProcess::AgentStdDev, std::shared_ptr< PostProcess::AgentStdDev> , boost::python::bases<PostProcess::AgentAnalysis> >("AgentStdDevStub", boost::python::init< const std::string & >() );
	boost::python::implicitly_convertible< std::shared_ptr< PostProcess::AgentStdDev >, std::shared_ptr< PostProcess::AgentAnalysis > >();
	
	boost::python::class_< PostProcess::RasterMean, std::shared_ptr< PostProcess::RasterMean> , boost::python::bases<PostProcess::RasterAnalysis> >("RasterMeanStub");
	boost::python::implicitly_convertible< std::shared_ptr< PostProcess::RasterMean>, std::shared_ptr< PostProcess::RasterAnalysis > >();

	boost::python::class_< PostProcess::RasterSum, std::shared_ptr< PostProcess::RasterSum> , boost::python::bases<PostProcess::RasterAnalysis> >("RasterSumStub");
	boost::python::implicitly_convertible< std::shared_ptr< PostProcess::RasterSum>, std::shared_ptr< PostProcess::RasterAnalysis > >();

	boost::python::implicitly_convertible< std::shared_ptr<Engine::DynamicRaster>, std::shared_ptr< Engine::StaticRaster > >();
	
	/*	
	boost::python::class_< Analysis::AgentHDFtoSHP, std::shared_ptr< Analysis::AgentHDFtoSHP> , boost::python::bases<Analysis::AgentAnalysis> >("AgentHDFtoSHPStub", boost::python::init< const std::string &, int >() )
	;
	boost::python::implicitly_convertible< std::shared_ptr< Analysis::AgentHDFtoSHP >, std::shared_ptr< Analysis::AgentAnalysis > >();
	*/
}

