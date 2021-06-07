//
//  CommonDefs.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/03/2021.
//

#include "CommonDefs.hpp"

namespace AutoDJ {


juce::String getLengthString(int secs)
{
    // If the seconds field will be <10
    if (secs % 60 < 10)
        // Return a MM:SS time string with a leading 0 in the seconds field
        return juce::String(floor(secs / 60)) + ":0" + juce::String(secs % 60);
    else
        // Return a MM:SS time string
        return juce::String(floor(secs / 60)) + ":" + juce::String(secs % 60);
}


juce::String getGrooveString(float groove)
{
    // Return a string that indicates the magnitude of the given
    // groove value, based on typical dance music ranges
    
    if (groove < 1.2)
        return "Low";
    else if (groove < 1.5)
        return "Med";
    else
        return "High";
}


int getBeatPeriod(int bpm)
{
    // Beat period is the number of samples per beat
    
    // Find the number of samples per minute,
    // divided by the number of beats per minute
    return round((60.0 * SUPPORTED_SAMPLERATE) / bpm);
}


} /* namespace AutoDJ */
