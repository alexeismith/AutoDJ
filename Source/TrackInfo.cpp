//
//  TrackInfo.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#include "TrackInfo.hpp"

#include "CommonDefs.hpp"


int TrackInfo::getSampleOfBeat(int beat)
{
    return beat * getBeatPeriod() + beatPhase;
}


int TrackInfo::getBeatPeriod()
{
    return AutoDJ::getBeatPeriod(bpm);
}


int TrackInfo::getLengthSamples()
{
    return length * SUPPORTED_SAMPLERATE;
}


juce::String TrackInfo::getTitle()
{
    juce::String titleStr = juce::String(juce::CharPointer_UTF8(title));

    if (titleStr.isEmpty())
        return getFilename();
    
    return titleStr;
}
