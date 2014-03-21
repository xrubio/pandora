
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
#include <Raster.hxx>
#include <Simulation.hxx>
#include <World.hxx>
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
#include <Scheduler.hxx>

#include <string>

typedef Engine::Point2D<int> Point2DInt;
typedef Engine::Size<int> SizeInt;
typedef Engine::Rectangle<int> RectangleInt;

class StaticRasterWrap : public Engine::StaticRaster, public boost::python::wrapper<Engine::StaticRaster>
{
public:
	void resize( const SizeInt & size )
	{
		if (boost::python::override resize = this->get_override("resize")(size))			
		{
			resize( size );
			return;
		}
		Engine::StaticRaster::resize(size);
	}

	void default_resize( const SizeInt & size )
	{
		Engine::StaticRaster::resize(size);
	}
	
	const int & getValue( Point2DInt position ) const
	{
		if (boost::python::override getValue = this->get_override("getValue")(position))
		{
			return getValue(position);
		}
		return Engine::StaticRaster::getValue(position);
	}

	const int & default_getValue(Point2DInt position) const
	{
		return Engine::StaticRaster::getValue(position);
	}
};

class RasterWrap : public Engine::Raster, public boost::python::wrapper<Engine::Raster>
{
public:
	void resize( const SizeInt & size )
	{
		if (boost::python::override resize = this->get_override("resize")(size))			
		{
			resize( size );
			return;
		}
		Engine::Raster::resize(size);
	}

	void default_resize( const SizeInt & size )
	{
		Engine::Raster::resize(size);
	}
	
	const int & getValue( Point2DInt position ) const
	{
		if (boost::python::override getValue = this->get_override("getValue")(position))
		{
			return getValue(position);
		}
		return Engine::Raster::getValue(position);
	}

	const int & default_getValue(Point2DInt position) const
	{
		return Engine::Raster::getValue(position);
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
	WorldWrap(const Engine::Simulation & simulation, Engine::Scheduler * scheduler = 0, const bool & allowMultipleAgentsPerCell = true) : World( simulation,scheduler,allowMultipleAgentsPerCell )
	{
	}

	void createRasters()
	{
		this->get_override("createRasters")();
	}	
	void createAgents()
	{
		this->get_override("createAgents")();
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
	
	void addAgentSimple( Engine::Agent * agent )
	{
		Engine::World::addAgent(agent, true);
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

void passAgentAnalysisOwnership( PostProcess::GlobalAgentStats & results, std::auto_ptr<PostProcess::AgentAnalysis> analysisPtr )
{
	results.addAnalysis(analysisPtr.get());
	analysisPtr.release();
}

void passRasterAnalysisOwnership( PostProcess::GlobalRasterStats & results, std::auto_ptr<PostProcess::RasterAnalysis> analysisPtr )
{
	results.addAnalysis(analysisPtr.get());
	analysisPtr.release();
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
Engine::Raster & (Engine::World::*getDynamicRaster)(const std::string&) = &Engine::World::getDynamicRaster;
Engine::StaticRaster & (Engine::World::*getStaticRaster)(const std::string&) = &Engine::World::getStaticRaster;

int (Engine::World::*getValue)(const std::string&, const Engine::Point2D<int> &) const = &Engine::World::getValue;
void (Engine::World::*setValue)(const std::string&, const Engine::Point2D<int> &, int) = &Engine::World::setValue;

Engine::Agent * (Engine::World::*getAgent)(const std::string &) = &Engine::World::getAgent;

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
		.def("clone", &RectangleInt::clone)
		;

	boost::python::class_< StaticRasterWrap, boost::noncopyable >("StaticRasterStub")
		.def("resize", &Engine::StaticRaster::resize, &StaticRasterWrap::default_resize) 
		.def("getSize", &Engine::StaticRaster::getSize)
		.def("getValue", &Engine::StaticRaster::getValue, &StaticRasterWrap::default_getValue, boost::python::return_value_policy<boost::python::copy_const_reference>())
	;

	boost::python::class_< RasterWrap, boost::noncopyable >("RasterStub")
		.def("setInitValues", &Engine::Raster::setInitValues) 
		.def("setValue", &Engine::Raster::setValue)	
		.def("resize", &Engine::Raster::resize, &RasterWrap::default_resize) 
		.def("getSize", &Engine::Raster::getSize)
		.def("getValue", &Engine::Raster::getValue, &RasterWrap::default_getValue, boost::python::return_value_policy<boost::python::copy_const_reference>())
	;

	boost::python::class_< Engine::Simulation >("SimulationStub", boost::python::init< const Engine::Size<int> &, const int &, const int & >() )	
		.add_property("size", boost::python::make_function(&Engine::Simulation::getSize, boost::python::return_value_policy<boost::python::reference_existing_object>()))
		.add_property("numSteps", boost::python::make_function(&Engine::Simulation::getNumSteps, boost::python::return_value_policy<boost::python::copy_const_reference>()))
	;
	
	boost::python::class_< AgentWrap, std::auto_ptr<AgentWrap>, boost::noncopyable >("AgentStub", boost::python::init< const std::string & > () )
		.def("updateState", &Engine::Agent::updateState, &AgentWrap::default_UpdateState)
		.def("registerAttributes", &Engine::Agent::registerAttributes, &AgentWrap::default_RegisterAttributes)
		.def("getWorld", &Engine::Agent::getWorldRef, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.def("serialize", boost::python::pure_virtual(&Engine::Agent::serialize))
		.def("registerIntAttribute", &Engine::Agent::registerIntAttribute)
		.def("serializeIntAttribute", &AgentWrap::serializeIntAttribute)
		.def("remove", &Engine::Agent::remove)
		.def("setRandomPosition", &Engine::Agent::setRandomPosition)
		.add_property("id", boost::python::make_function(&Engine::Agent::getId, boost::python::return_value_policy<boost::python::copy_const_reference>()))
		.add_property("exists", &Engine::Agent::exists)
		.add_property("_type", &Engine::Agent::getType)
		.add_property("position", boost::python::make_function(&Engine::Agent::getPosition, boost::python::return_value_policy<boost::python::reference_existing_object>()), &Engine::Agent::setPosition )
	;
	
	boost::python::class_< std::vector<std::string> >("StringVector").def(boost::python::vector_indexing_suite< std::vector<std::string> >());
	boost::python::class_< std::vector<int> >("IntVector").def(boost::python::vector_indexing_suite< std::vector<int> >());
	
	boost::python::class_< WorldWrap, boost::noncopyable >("WorldStub", boost::python::init< const Engine::Simulation &, Engine::Scheduler *, const bool & >()[boost::python::with_custodian_and_ward<1,3>()])
		.def("createRasters", boost::python::pure_virtual(&Engine::World::createRasters))
		.def("createAgents", boost::python::pure_virtual(&Engine::World::createAgents))
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
		.def("addAgentStub", &WorldWrap::addAgentSimple)
		.def("setValue", setValue)
		.def("getValue", getValue)
		.def("getAgentIds", &WorldWrap::getAgentIds)
		.def("getNeighboursIds", &WorldWrap::getNeighboursIds)
		.def("getAgent", getAgent, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.def("getSimulation", &Engine::World::getSimulation, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.add_property("currentStep", &Engine::World::getCurrentStep)
	;	
	
	boost::python::class_< Engine::SpacePartition, std::auto_ptr<Engine::SpacePartition> >("SpacePartitionStub", boost::python::init< const int &, const std::string &, bool >())
	;
	
	boost::python::implicitly_convertible< std::auto_ptr< Engine::SpacePartition >, std::auto_ptr< Engine::Scheduler > >();	

	boost::python::class_< Engine::SimulationRecord>("SimulationRecordStub", boost::python::init< int, bool >())
		.def("loadHDF5", &Engine::SimulationRecord::loadHDF5)
	;

	boost::python::class_< PostProcess::GlobalAgentStats>("GlobalAgentsStatsStub", boost::python::init< const std::string & > ())
		.def("addAnalysis", &passAgentAnalysisOwnership)
		.def("applyTo", &PostProcess::GlobalAgentStats::apply)
	;
	
	boost::python::class_< PostProcess::GlobalRasterStats>("GlobalRasterStatsStub", boost::python::init< const std::string & > ())
		.def("addAnalysis", &passRasterAnalysisOwnership)
		.def("applyTo", &PostProcess::GlobalRasterStats::apply)
	;

	boost::python::class_< PostProcess::Analysis, std::auto_ptr<PostProcess::Analysis> >("AnalysisStub", boost::python::init< const std::string &, bool >() )
	;
	
	boost::python::class_< AgentAnalysisWrap, std::auto_ptr<AgentAnalysisWrap> , boost::python::bases<PostProcess::Analysis>, boost::noncopyable >("AgentAnalysisStub", boost::python::init< const std::string &, bool >() )
		.def("computeAgent", boost::python::pure_virtual(&PostProcess::AgentAnalysis::computeAgent))
	;
	boost::python::implicitly_convertible< std::auto_ptr< PostProcess::AgentAnalysis >, std::auto_ptr< PostProcess::Analysis > >();	

	// raster analysis
	boost::python::class_< RasterAnalysisWrap, std::auto_ptr<RasterAnalysisWrap> , boost::python::bases<PostProcess::Analysis>, boost::noncopyable >("RasterAnalysisStub", boost::python::init< const std::string &, bool >() )
		.def("computeRaster", boost::python::pure_virtual(&PostProcess::RasterAnalysis::computeRaster))
	;
	boost::python::implicitly_convertible< std::auto_ptr< PostProcess::RasterAnalysis >, std::auto_ptr< PostProcess::Analysis > >();	

	boost::python::class_< PostProcess::AgentNum, std::auto_ptr< PostProcess::AgentNum> , boost::python::bases<PostProcess::AgentAnalysis> >("AgentNumStub");
	boost::python::implicitly_convertible< std::auto_ptr< PostProcess::AgentNum >, std::auto_ptr< PostProcess::AgentAnalysis > >();

	boost::python::class_< PostProcess::AgentMean, std::auto_ptr< PostProcess::AgentMean> , boost::python::bases<PostProcess::AgentAnalysis> >("AgentMeanStub", boost::python::init< const std::string & >() );
	boost::python::implicitly_convertible< std::auto_ptr< PostProcess::AgentMean >, std::auto_ptr< PostProcess::AgentAnalysis > >();

	boost::python::class_< PostProcess::AgentSum, std::auto_ptr< PostProcess::AgentSum> , boost::python::bases<PostProcess::AgentAnalysis> >("AgentSumStub", boost::python::init< const std::string & >() );
	boost::python::implicitly_convertible< std::auto_ptr< PostProcess::AgentSum >, std::auto_ptr< PostProcess::AgentAnalysis > >();
	
	boost::python::class_< PostProcess::AgentStdDev, std::auto_ptr< PostProcess::AgentStdDev> , boost::python::bases<PostProcess::AgentAnalysis> >("AgentStdDevStub", boost::python::init< const std::string & >() );
	boost::python::implicitly_convertible< std::auto_ptr< PostProcess::AgentStdDev >, std::auto_ptr< PostProcess::AgentAnalysis > >();
	
	boost::python::class_< PostProcess::RasterMean, std::auto_ptr< PostProcess::RasterMean> , boost::python::bases<PostProcess::RasterAnalysis> >("RasterMeanStub");
	boost::python::implicitly_convertible< std::auto_ptr< PostProcess::RasterMean>, std::auto_ptr< PostProcess::RasterAnalysis > >();

	boost::python::class_< PostProcess::RasterSum, std::auto_ptr< PostProcess::RasterSum> , boost::python::bases<PostProcess::RasterAnalysis> >("RasterSumStub");
	boost::python::implicitly_convertible< std::auto_ptr< PostProcess::RasterSum>, std::auto_ptr< PostProcess::RasterAnalysis > >();
	
	
	/*	
	boost::python::class_< Analysis::AgentHDFtoSHP, std::auto_ptr< Analysis::AgentHDFtoSHP> , boost::python::bases<Analysis::AgentAnalysis> >("AgentHDFtoSHPStub", boost::python::init< const std::string &, int >() )
	;
	boost::python::implicitly_convertible< std::auto_ptr< Analysis::AgentHDFtoSHP >, std::auto_ptr< Analysis::AgentAnalysis > >();
	*/
}

