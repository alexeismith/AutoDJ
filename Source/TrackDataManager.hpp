//
//  TrackDataManager.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#ifndef DataManager_hpp
#define DataManager_hpp

#include <JuceHeader.h>
#include "SqlDatabase.hpp"

#define DATABASE_FILENAME ("AutoDjData.db")

class TrackDataManager
{
public:
      
    TrackDataManager();
      
    ~TrackDataManager() {}
    
    void initialise(juce::File directory);
    
    juce::DirectoryContentsList& getContents() { return *dirContents.get(); }
      
private:
    
    void printTrackData(TrackData data);
    
    void parseFiles();
    
    TrackData getTrackData(juce::File file);
    
    juce::String getChecksum(juce::File file);

    
    juce::AudioFormatManager formatManager;
    juce::WildcardFileFilter fileFilter;
    SqlDatabase database;
    
    juce::TimeSliceThread thread {"BackgroundUpdateThread"};
    std::unique_ptr<juce::DirectoryContentsList> dirContents;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackDataManager)
};

#endif /* TrackDataManager_hpp */
