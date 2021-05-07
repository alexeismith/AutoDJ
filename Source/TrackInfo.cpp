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


int TrackInfo::getNearestDownbeat(int sample)
{
    if (sample <= getFirstDownbeat())
        return getFirstDownbeat();
    
    int samplesPerBar = getBeatPeriod() * BEATS_PER_BAR;
    sample -= getFirstDownbeat();
    int bar = round(double(sample) / samplesPerBar);
    
    int nearestDownbeat = bar * samplesPerBar + getFirstDownbeat();
    
    if (nearestDownbeat > getLengthSamples())
        nearestDownbeat -= samplesPerBar;
    
    return nearestDownbeat;
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


juce::String TrackInfo::getArtistTitle()
{
    juce::String artistStr = juce::String(juce::CharPointer_UTF8(artist));
    juce::String titleStr = juce::String(juce::CharPointer_UTF8(title));
    
    if (artistStr.isEmpty() || titleStr.isEmpty())
        return getFilename();
    
    return artistStr + " - " + titleStr;
}
