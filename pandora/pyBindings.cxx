
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
#include <Point2D.hxx>
#include <StaticRaster.hxx>
#include <Raster.hxx>
#include <Simulation.hxx>
#include <World.hxx>
#include <Agent.hxx>
#include <AgentRecord.hxx>
#include <SimulationRecord.hxx>

#include <analysis/Results.hxx>
#include <analysis/Analysis.hxx>

#include <analysis/AgentMean.hxx>
#include <analysis/AgentSum.hxx>
#include <analysis/AgentNum.hxx>
#include <analysis/AgentHDFtoSHP.hxx>

#include <analysis/RasterSum.hxx>
#include <analysis/RasterMean.hxx>

#include <string>

typedef Engine::Point2D<int> Point2DInt;

class StaticRasterWrap : public Engine::StaticRaster, public boost::python::wrapper<Engine::StaticRaster>
{
public:
	void resize( const Point2DInt & size )
	{
		if (boost::python::override resize = this->get_override("resize")(size))			
		{
			resize( size );
			return;
		}
		Engine::StaticRaster::resize(size);
	}

	void default_resize( const Point2DInt & size )
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
	void resize( const Point2DInt & size )
	{
		if (boost::python::override resize = this->get_override("resize")(size))			
		{
			resize( size );
			return;
		}
		Engine::Raster::resize(size);
	}

	void default_resize( const Point2DInt & size )
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
	WorldWrap( const Engine::Simulation & simulation, const int & overlap, const bool & allowMultipleAgentsPerCell, const std::string & fileName ) : World( simulation, overlap, allowMultipleAgentsPerCell, fileName)
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
};

class AgentAnalysisWrap : public Analysis::AgentAnalysis, public boost::python::wrapper<Analysis::AgentAnalysis>
{
public:
	AgentAnalysisWrap( const std::string & name ) : AgentAnalysis(name)
	{
	}

	void computeAgent( const Engine::AgentRecord & record )
	{
		this->get_override("computeAgent")(record);
	}
};

class RasterAnalysisWrap : public Analysis::RasterAnalysis, public boost::python::wrapper<Analysis::RasterAnalysis>
{
public:
	RasterAnalysisWrap( const std::string & name ) : RasterAnalysis(name)
	{
	}

	void computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory )
	{
		this->get_override("computeRaster")(rasterHistory);
	}
};

class ResultsWrap : public Analysis::Results, public boost::python::wrapper<Analysis::Results>
{
public:
	ResultsWrap( const Engine::SimulationRecord & record, const std::string & outputFile, const std::string & type, const std::string & separator ) : Results(record, outputFile, type, separator)
	{
	}

	void concreteApply() const
	{
		this->get_override("concreteApply")();
	}
};

void passAnalysisOwnership( Analysis::Results & results, std::auto_ptr<Analysis::Analysis> analysisPtr )
{
	results.addAnalysis(analysisPtr.get());
	analysisPtr.release();
}

BOOST_PYTHON_MODULE(libpyPandora)
{
	boost::python::class_< Point2DInt >("Point2DIntStub", boost::python::init<const int & , const int & >() )
		.def_readwrite("_x", &Point2DInt::_x) 
		.def_readwrite("_y", &Point2DInt::_y) 
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

	boost::python::class_< Engine::Simulation >("SimulationStub", boost::python::init< const int &, const int & >() )
	;

	boost::python::class_< WorldWrap, boost::noncopyable >("WorldStub", boost::python::init< const Engine::Simulation & , const int &, const bool &, const std::string & >() )
		.def("createRasters", boost::python::pure_virtual(&Engine::World::createRasters))
		.def("createAgents", boost::python::pure_virtual(&Engine::World::createAgents))
		.def("stepEnvironment", &Engine::World::stepEnvironment, &WorldWrap::default_StepEnvironment)
		.def("initialize", &Engine::World::initialize)
		.def("checkPosition", &Engine::World::checkPosition)
		.def("registerDynamicRaster", &Engine::World::registerDynamicRaster)
		.def("getDynamicRaster", &Engine::World::getDynamicRaster, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.def("run", &Engine::World::run)
		.def("addAgentStub", &WorldWrap::addAgent)
		.def("setValue", &Engine::World::setValue)
		.def("getValue", &Engine::World::getValue)
		.add_property("currentStep", &Engine::World::getCurrentStep)
	;
	
	boost::python::class_< AgentWrap, std::auto_ptr<AgentWrap>, boost::noncopyable >("AgentStub", boost::python::init< const std::string & > () )
		.def("updateState", &Engine::Agent::updateState, &AgentWrap::default_UpdateState)
		.def("registerAttributes", &Engine::Agent::registerAttributes, &AgentWrap::default_RegisterAttributes)
		.def("getWorld", &Engine::Agent::getWorldRef, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.def("serialize", boost::python::pure_virtual(&Engine::Agent::serialize))
		.def("registerIntAttribute", &Engine::Agent::registerIntAttribute)
		.def("serializeIntAttribute", &AgentWrap::serializeIntAttribute)
		.def("setRandomPosition", &Engine::Agent::setRandomPosition)
		.add_property("id", boost::python::make_function(&Engine::Agent::getId, boost::python::return_value_policy<boost::python::copy_const_reference>()))
		.add_property("position", boost::python::make_function(&Engine::Agent::getPosition, boost::python::return_value_policy<boost::python::reference_existing_object>()), &Engine::Agent::setPosition )
	;
	
	boost::python::class_< Engine::SimulationRecord>("SimulationRecordStub")
		.def("loadHDF5", &Engine::SimulationRecord::loadHDF5)
	;

	// analysis base
	boost::python::class_< Analysis::Analysis, std::auto_ptr<Analysis::Analysis> >("AnalysisStub", boost::python::init< const std::string & >() )
	;
	// agent analysis
	boost::python::class_< AgentAnalysisWrap, std::auto_ptr<AgentAnalysisWrap> , boost::python::bases<Analysis::Analysis>, boost::noncopyable >("AgentAnalysisStub", boost::python::init< const std::string & >() )
		.def("computeAgent", boost::python::pure_virtual(&Analysis::AgentAnalysis::computeAgent))
	;
	boost::python::implicitly_convertible< std::auto_ptr< Analysis::AgentAnalysis >, std::auto_ptr< Analysis::Analysis > >();	

	// raster analysis
	boost::python::class_< RasterAnalysisWrap, std::auto_ptr<RasterAnalysisWrap> , boost::python::bases<Analysis::Analysis>, boost::noncopyable >("RasterAnalysisStub", boost::python::init< const std::string & >() )
		.def("computeRaster", boost::python::pure_virtual(&Analysis::RasterAnalysis::computeRaster))
	;
	boost::python::implicitly_convertible< std::auto_ptr< Analysis::RasterAnalysis >, std::auto_ptr< Analysis::Analysis > >();	
	
	// concrete agent analysis
	boost::python::class_< Analysis::AgentMean, std::auto_ptr< Analysis::AgentMean> , boost::python::bases<Analysis::AgentAnalysis> >("AgentMeanStub", boost::python::init< const std::string & >() )
	;
	boost::python::implicitly_convertible< std::auto_ptr< Analysis::AgentMean >, std::auto_ptr< Analysis::AgentAnalysis > >();
	
	boost::python::class_< Analysis::AgentSum, std::auto_ptr< Analysis::AgentSum> , boost::python::bases<Analysis::AgentAnalysis> >("AgentSumStub", boost::python::init< const std::string & >() )
	;
	boost::python::implicitly_convertible< std::auto_ptr< Analysis::AgentSum >, std::auto_ptr< Analysis::AgentAnalysis > >();

	boost::python::class_< Analysis::AgentNum, std::auto_ptr< Analysis::AgentNum> , boost::python::bases<Analysis::AgentAnalysis> >("AgentNumStub")
	;
	boost::python::implicitly_convertible< std::auto_ptr< Analysis::AgentNum >, std::auto_ptr< Analysis::AgentAnalysis > >();
	
	boost::python::class_< Analysis::AgentHDFtoSHP, std::auto_ptr< Analysis::AgentHDFtoSHP> , boost::python::bases<Analysis::AgentAnalysis> >("AgentHDFtoSHPStub", boost::python::init< const std::string &, int >() )
	;
	boost::python::implicitly_convertible< std::auto_ptr< Analysis::AgentHDFtoSHP >, std::auto_ptr< Analysis::AgentAnalysis > >();

	// concrete raster analysis
	boost::python::class_< Analysis::RasterMean, std::auto_ptr< Analysis::RasterMean> , boost::python::bases<Analysis::RasterAnalysis> >("RasterMeanStub")
	;
	boost::python::implicitly_convertible< std::auto_ptr< Analysis::RasterMean >, std::auto_ptr< Analysis::RasterAnalysis > >();

	boost::python::class_< Analysis::RasterSum, std::auto_ptr< Analysis::RasterSum> , boost::python::bases<Analysis::RasterAnalysis> >("RasterSumStub")
	;
	boost::python::implicitly_convertible< std::auto_ptr< Analysis::RasterSum >, std::auto_ptr< Analysis::RasterAnalysis > >();

	// Results classes
	boost::python::class_< ResultsWrap, boost::noncopyable >("ResultsStub", boost::python::init< const Engine::SimulationRecord & , const std::string &, const std::string &, const std::string & >() )
	.def("compute", &Analysis::Results::apply)
	.def("addAnalysis", &passAnalysisOwnership)
	.def("concreteApply", boost::python::pure_virtual(&Analysis::Results::concreteApply))
	;	

	boost::python::class_< Analysis::AgentResults, boost::python::bases< Analysis::Results> >("AgentResultsStub", boost::python::init< const Engine::SimulationRecord & , const std::string &, const std::string &, const std::string & >() )
	;
	boost::python::class_< Analysis::RasterResults, boost::python::bases< Analysis::Results> >("RasterResultsStub", boost::python::init< const Engine::SimulationRecord & , const std::string &, const std::string &, const std::string & >() )
	;	

}

