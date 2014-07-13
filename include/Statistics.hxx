
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

namespace Engine
{

class Statistics
{
	typedef boost::mt19937 RandomEngine;
	RandomEngine _randomGenerator;
	static const int _distributionSize = 100000;

	// general random indexs
	boost::uniform_int<> _randomNumbers;
	mutable boost::variate_generator< RandomEngine, boost::uniform_int<> > _nextRandomNumber;

	// TODO fix expo and normal distributions!
	std::vector<float> _exponentialDistribution;
	void generateExponentialDistribution();

	std::vector<float> _normalDistribution;
	void generateNormalDistribution();
public:
	Statistics();
	float getExponentialDistValue( float min, float max ) const;
	float getNormalDistValue( float min, float max ) const;

	// uniform dist does not need to generate numbers, as randomNumbers itself is a 
	int getUniformDistValue( int min, int max ) const;
	//! Gets a random number from /dev/urandom to be used as a seed.
	uint64_t getNewSeed();

};

} // namespace Engine

#endif // __Statistics_hxx__

