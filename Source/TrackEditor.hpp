//
//  TrackEditor.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#ifndef TrackEditor_hpp
#define TrackEditor_hpp

#include "WaveformView.hpp"
#include "DataManager.hpp"
#include "AnalyserSegments.hpp"

class TrackEditor : public juce::Component
{
public:
    
    TrackEditor(DataManager* dm);
    
    ~TrackEditor() {}
    
    void resized() override;
    
    void paint(juce::Graphics& g) override;
    
    void load(Track track);
    
private:
    
    std::unique_ptr<WaveformView> waveform;
    
    Track track;
    
    DataManager* dataManager = nullptr;
    
    juce::String message;
    
    
    std::unique_ptr<AnalyserSegments> analyserSegments;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackEditor) ///< JUCE macro to add a memory leak detector
};

#endif /* TrackEditor_hpp */
