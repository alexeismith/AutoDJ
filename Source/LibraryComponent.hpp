//
//  LibraryComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#ifndef LibraryComponent_hpp
#define LibraryComponent_hpp

#include "TrackDataManager.hpp"
#include "TrackTableComponent.hpp"
#include "CommonDefs.hpp"

class LibraryComponent : public juce::Component, public juce::Button::Listener
{
public:
    
    LibraryComponent();
    
    ~LibraryComponent() {}
    
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    
private:
    
    void chooseFolder();
    
//    std::unique_ptr<juce::FileListComponent> fileList;
    std::unique_ptr<TrackTableComponent> trackTable;
    
    std::unique_ptr<juce::Button> chooseFolderBtn;
    
    TrackDataManager dataManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LibraryComponent)
};

#endif /* LibraryComponent_hpp */
