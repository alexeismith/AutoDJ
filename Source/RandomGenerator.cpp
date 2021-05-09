//
//  RandomGenerator.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 07/05/2021.
//

#include "RandomGenerator.hpp"

#include <JuceHeader.h>


RandomGenerator::RandomGenerator()
{
    algorithm.seed((unsigned int)juce::Time::currentTimeMillis());
    
    // TODO: temp
//    for (int i = 0; i < 100; i++)
//        DBG(getInt(1,24));
}


double RandomGenerator::getGaussian(double stdDev, double rangeLimit, double shift)
{
    std::normal_distribution<double> distribution(0, stdDev);
    double value = distribution(algorithm);
    
    if (rangeLimit > 0.0)
        value = juce::jlimit(-rangeLimit, rangeLimit, value);
    
    return value + shift;
}


bool RandomGenerator::getBool()
{
    std::uniform_int_distribution<> distribution(0, 1);
    return bool(distribution(algorithm));
}


int RandomGenerator::getInt(int min, int max)
{
    jassert(min < max);
    
    std::uniform_int_distribution<> distribution(min, max);
    return distribution(algorithm);
}
