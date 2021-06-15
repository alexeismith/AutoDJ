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
    if (!analysed) jassert(false); // Track must be analysed to calculate this!
    
    return beat * getBeatPeriod() + beatPhase;
}


int TrackInfo::getNearestDownbeat(int sample)
{
    if (!analysed) jassert(false); // Track must be analysed to calculate this!
    
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
    if (!analysed) jassert(false); // Track must be analysed to calculate this!
    
    return AutoDJ::getBeatPeriod(bpm);
}


int TrackInfo::getBarLength()
{
    if (!analysed) jassert(false); // Track must be analysed to calculate this!
    
    return getBeatPeriod() * BEATS_PER_BAR;
}


int TrackInfo::getLengthSamples()
{
    return length * SUPPORTED_SAMPLERATE;
}


juce::String TrackInfo::getTitle()
{
    juce::String titleStr = juce::String(juce::CharPointer_UTF8(title));

    if (titleStr.isEmpty())
        titleStr = getFilename();
    
    if (titleStr.endsWith(".mp3") || titleStr.endsWith(".wav"))
        titleStr = titleStr.dropLastCharacters(4);
    
    return titleStr;
}


juce::String TrackInfo::getArtistTitle()
{
    juce::String artistStr = juce::String(juce::CharPointer_UTF8(artist));
    juce::String titleStr = juce::String(juce::CharPointer_UTF8(title));
    juce::String filenameStr;
    
    if (artistStr.isEmpty() || titleStr.isEmpty())
    {
        filenameStr = getFilename();
        
        if (filenameStr.endsWith(".mp3") || filenameStr.endsWith(".wav"))
            filenameStr = filenameStr.dropLastCharacters(4);
        
        return filenameStr;
    }
    
    return artistStr + " - " + titleStr;
}
