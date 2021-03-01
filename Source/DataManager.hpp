//
//  DataManager.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#ifndef DataManager_hpp
#define DataManager_hpp

#include <JuceHeader.h>

typedef struct TrackData {
    juce::String filename;
    juce::String title;
    juce::String artist;
    int length; // Total number of samples, divide this by SUPPORTED_SAMPLERATE to get length in seconds
    int bpm = -1; // Tempo in beats per minute
    int key = -1; // Musical key signature TODO: how to represent camelot?
    int energy = -1; // Overall energy level TODO: how to represent energy? thinking about track distribution screen, maybe a numeric scale is best
} TrackData;


class DataManager
{
  public:
      
      DataManager() {}
      
      ~DataManager() {}
      
      
      
  private:
      
      
      JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DataManager)
};

#endif /* DataManager_hpp */
