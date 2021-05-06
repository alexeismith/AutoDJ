//
//  TrackEditor.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#ifndef TrackEditor_hpp
#define TrackEditor_hpp

#include "WaveformView.hpp"

class TrackEditor : juce::Component
{
public:
    
    TrackEditor();
    
    ~TrackEditor() {}
    
    void load(Track* track);
    
private:
    
    std::unique_ptr<WaveformView> waveform;
    
    Track* track;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackEditor)
};

#endif /* TrackEditor_hpp */
