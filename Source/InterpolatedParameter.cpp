//
//  InterpolatedParameter.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#include "InterpolatedParameter.hpp"


InterpolatedParameter::InterpolatedParameter(double currentVal, double targetVal, int numSamples)
{
    currentValue = currentVal;
    targetValue = targetVal;
    samplesRemaining = numSamples;
    
    rateOfChange = (targetValue - currentValue) / samplesRemaining;
}


void InterpolatedParameter::update(int numSamples)
{
    if (samplesRemaining < 0) return;
    
    samplesRemaining -= numSamples;
    
    if (samplesRemaining <= 0)
    {
        currentValue = targetValue;
        samplesRemaining = -1;
    }
    else
    {
        currentValue += rateOfChange * numSamples;
    }
}
