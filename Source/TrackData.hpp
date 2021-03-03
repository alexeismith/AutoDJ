//
//  TrackData.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 03/03/2021.
//

#ifndef TrackData_h
#define TrackData_h

typedef struct TrackData {
    juce::String filename;
    juce::String artist;
    juce::String title;
    int length; // Total number of samples, divide this by SUPPORTED_SAMPLERATE to get length in seconds
    int bpm = -1; // Tempo in beats per minute
    int key = -1; // Musical key signature TODO: how to represent camelot?
    int energy = -1; // Overall energy level TODO: how to represent energy? thinking about track distribution screen, maybe a numeric scale is best
} TrackData;

#endif /* TrackData_h */
