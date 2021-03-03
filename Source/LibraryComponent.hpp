//
//  LibraryComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#ifndef LibraryComponent_hpp
#define LibraryComponent_hpp

#include <JuceHeader.h>
#include "TrackDataManager.hpp"


class LibraryComponent : public juce::Component
{
public:
    
    LibraryComponent();
    
    ~LibraryComponent() {}
    
    void resized() override;
    
private:
    
    juce::TimeSliceThread thread {"BackgroundUpdateThread"};
    std::unique_ptr<juce::FileListComponent> fileList;
    std::unique_ptr<juce::DirectoryContentsList> dirContents;
    
    TrackDataManager dataManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LibraryComponent)
};

#endif /* LibraryComponent_hpp */
