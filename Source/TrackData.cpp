//
//  TrackData.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#include "TrackData.hpp"

#include "CommonDefs.hpp"


int TrackData::getSampleOfBeat(int beat)
{
    return beat * getBeatPeriod();
}


int TrackData::getBeatPeriod()
{
    return round((60.0 * SUPPORTED_SAMPLERATE) / bpm);
}


int TrackData::getLengthSamples()
{
    return length * SUPPORTED_SAMPLERATE;
}


//int TrackData::getLastDownbeat()
//{
//    int numBeats = floor((getLengthSamples() - beatPhase) / getBeatPeriod());
//    return floor((numBeats - downbeat) / BEATS_PER_BAR);
//}
