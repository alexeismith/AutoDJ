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
}


double RandomGenerator::getGaussian(double stdDev, double rangeLimit, double shift)
{
    std::normal_distribution<double> distribution(0, stdDev);
    double value = distribution(algorithm);
    
    if (rangeLimit > 0.0)
        value = juce::jlimit(-rangeLimit, rangeLimit, value);
    
    return value + shift;
}
