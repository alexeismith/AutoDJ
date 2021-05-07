//
//  TrackEditor.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#ifndef TrackEditor_hpp
#define TrackEditor_hpp

#include "WaveformView.hpp"
#include "TrackDataManager.hpp"
#include "AnalyserSegments.hpp"

class TrackEditor : public juce::Component
{
public:
    
    TrackEditor(TrackDataManager* dm);
    
    ~TrackEditor() {}
    
    void resized() override;
    
    void paint(juce::Graphics& g) override;
    
    void load(Track track);
    
private:
    
    std::unique_ptr<WaveformView> waveform;
    
    Track track;
    
    TrackDataManager* dataManager = nullptr;
    
    juce::String message;
    
    
    std::unique_ptr<AnalyserSegments> analyserSegments;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackEditor)
};

#endif /* TrackEditor_hpp */
