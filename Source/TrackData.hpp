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
    juce::String artist = "";
    juce::String title = "";
    int length = 0; // Total length in seconds
    bool analysed = false; // Indicates whether analysis has been performed to extract bpm, key and energy
    int bpm = -1; // Tempo in beats per minute
    int key = -1; // Musical key signature TODO: how to represent camelot?
    int energy = -1; // Overall energy level TODO: how to represent energy? thinking about track distribution screen, maybe a numeric scale is best
} TrackData;

#endif /* TrackData_h */
