
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

#ifndef __Statistics_hxx__
#define __Statistics_hxx__

#include <boost/random.hpp>
#include <vector>
#include <map>
#include <tuple>

namespace Engine
{

class Statistics
{
	typedef boost::mt19937 RandomEngine;
	typedef boost::variate_generator<boost::mt19937&,boost::gamma_distribution<> >  TypeGenerator;

	RandomEngine _randomGenerator;
	static const int _distributionSize = 100000;

	// general random indexs
	boost::uniform_int<> _randomNumbers;
	mutable boost::variate_generator< RandomEngine, boost::uniform_int<> > _nextRandomNumber;

    boost::uniform_01<RandomEngine> _next01Number;

	// TODO fix expo and normal distributions!
	std::vector<float> _exponentialDistribution;
	void generateExponentialDistribution();

	std::vector<float> _normalDistribution;
	void generateNormalDistribution();

	std::map<std::string, std::tuple<TypeGenerator,TypeGenerator,double> > _mapBetaDistributions;

public:
	Statistics();
	float getExponentialDistValue( float min, float max ) const;
	float getNormalDistValueMinMax( float min, float max ) const;
	float getNormalDistValue( float mean, float sd );

	// uniform dist does not need to generate numbers, as randomNumbers itself is a 
	int getUniformDistValue( int min, int max ) const;
	// uniform float distribution between 0 and 1
	float getUniformDistValue();
	//! Gets a random number from /dev/urandom to be used as a seed.
	uint64_t getNewSeed();

	void addBetaDistribution(std::string name, double alpha, double beta, double scale);
	double getBetaDistributionValue(std::string name);

};

} // namespace Engine

#endif // __Statistics_hxx__

