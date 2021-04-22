//
//  InterpolatedParameter.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#include "InterpolatedParameter.hpp"

#include <JuceHeader.h>


void InterpolatedParameter::update(int currentSample, int numSamples)
{
    if (startSample < 0 || currentSample < startSample) return;
    
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


void InterpolatedParameter::moveTo(double targetVal, int start, int numSamples)
{
    targetValue = targetVal;
    startSample = start;
    samplesRemaining = numSamples;
    
    if (startSample < 0)
        currentValue = targetValue;
    
    if (samplesRemaining > 0)
        rateOfChange = (targetValue - currentValue) / samplesRemaining;
}
