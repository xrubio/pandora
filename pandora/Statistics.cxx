
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

#include <Statistics.hxx>
#include <Exceptions.hxx>

#include <limits>
#include <sstream>
#include <boost/generator_iterator.hpp>
#include <fstream>

namespace Engine
{

Statistics::Statistics() :  _randomGenerator(getNewSeed()), _randomNumbers(0, _distributionSize-1), _nextRandomNumber(_randomGenerator,_randomNumbers)
{
	generateExponentialDistribution();
	generateNormalDistribution();
}

void Statistics::generateExponentialDistribution()
{
	boost::exponential_distribution<> distribution; 
	boost::variate_generator< RandomEngine, boost::exponential_distribution<> > die(_randomGenerator, distribution);

	_exponentialDistribution.resize(_distributionSize);

	float min = std::numeric_limits<float>::max();
	float max = 0;
	for(int i=0; i<_distributionSize; i++)
	{
		_exponentialDistribution[i] = die();
		if(_exponentialDistribution[i]<min)
		{
			min = _exponentialDistribution[i];
		}
		if(_exponentialDistribution[i]>max)
		{
			max = _exponentialDistribution[i];
		}
	}

	// translate and scale
	float diff = max - min;
	for(int i=0; i<_distributionSize; i++)
	{
		_exponentialDistribution[i] -= min;
		_exponentialDistribution[i] /= diff;
	}
}

void Statistics::generateNormalDistribution()
{
	boost::normal_distribution<> distribution; 
	boost::variate_generator< RandomEngine, boost::normal_distribution<> > die(_randomGenerator, distribution);

	_normalDistribution.resize(_distributionSize);

	float min = std::numeric_limits<float>::max();
	float max = 0;
	for(int i=0; i<_distributionSize; i++)
	{
		_normalDistribution[i] = die();
		if(_normalDistribution[i]<min)
		{
			min = _normalDistribution[i];
		}
		if(_normalDistribution[i]>max)
		{
			max = _normalDistribution[i];
		}
	}

	// translate and scale
	float diff = max - min;
	for(int i=0; i<_distributionSize; i++)
	{
		_normalDistribution[i] -= min;
		_normalDistribution[i] /= diff;
	}
}

float Statistics::getExponentialDistValue( float min, float max ) const
{
	if(_exponentialDistribution.size()!=_distributionSize)
	{
		std::stringstream oss;
		oss << " Statistics::getExponentialDistValue - getting value with min: " << min << " and max: " << max << " before initializing distribution";		
		throw Exception(oss.str());
	}
	int index = _nextRandomNumber();
	float value = _exponentialDistribution[index];
	float diff = max - min;
	value *= diff;
	value += min;
	return value;
}

float Statistics::getNormalDistValue( float min, float max ) const 
{
	if(_normalDistribution.size()!=_distributionSize)
	{
		std::stringstream oss;
		oss << " Statistics::getNormalDistValue - getting value with min: " << min << " and max: " << max << " before initializing distribution";		
		throw Exception(oss.str());
	}
	int index = _nextRandomNumber();
	float value = _normalDistribution[index];
	float diff = max - min;
	value *= diff;
	value += min;
	return value;
}

int Statistics::getUniformDistValue( int min, int max ) const
{	
	float value = float(_nextRandomNumber())/float(_distributionSize-1);
	float diff = max - min;
	value *= diff;
	value += min;
	if(value < 0.0f)
	{
		return (int)(value-0.5f);
	}
	return (int)(value+0.5f);
}

uint64_t Statistics::getNewSeed()
{
	uint64_t seed;
	std::ifstream urandom;
	urandom.open("/dev/urandom");
	urandom.read(reinterpret_cast<char*> (&seed), sizeof(seed));
	urandom.close();
	//std::cout << "seed: " << seed << std::endl;
	return seed;
}

} // namespace Engine

