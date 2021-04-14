//
//  LibraryComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#ifndef LibraryComponent_hpp
#define LibraryComponent_hpp

#include "TrackDataManager.hpp"
#include "AnalysisManager.hpp"
#include "QueueTableComponent.hpp"
#include "CommonDefs.hpp"
#include "WaveformComponent.hpp"

//TODO: temp?
#include "AudioProcessor.hpp"

class LibraryComponent : public juce::Component, public juce::Button::Listener
{
public:
    
    LibraryComponent(AudioProcessor* p);
    
    ~LibraryComponent() {}
    
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    
private:
    
    void chooseFolder();
    
    std::unique_ptr<TrackTableComponent> trackTable;
    
    std::unique_ptr<juce::Button> chooseFolderBtn;
    
    TrackDataManager dataManager;
    std::unique_ptr<AnalysisManager> analysisManager;
    
    std::unique_ptr<WaveformComponent> waveform;
    
    AudioProcessor* audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LibraryComponent)
};

#endif /* LibraryComponent_hpp */
