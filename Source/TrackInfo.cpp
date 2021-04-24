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
    return beat * getBeatPeriod();
}


int TrackInfo::getBeatPeriod()
{
    return round((60.0 * SUPPORTED_SAMPLERATE) / bpm);
}


int TrackInfo::getLengthSamples()
{
    return length * SUPPORTED_SAMPLERATE;
}