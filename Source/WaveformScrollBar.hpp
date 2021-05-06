//
//  WaveformScrollBar.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 04/05/2021.
//

#ifndef WaveformScrollBar_hpp
#define WaveformScrollBar_hpp

#include "WaveformComponent.hpp"

#define WAVEFORM_BAR_HEIGHT (30)

// NOTE: must be same width as the WaveformComponent its paired to, in order for windowed section to be correct
class WaveformScrollBar : public WaveformComponent
{
public:
    
    WaveformScrollBar();
    
    ~WaveformScrollBar() {}
    
    void paint(juce::Graphics& g) override;
    
    void resized() override;
    
    void update(int playhead, double timeStretch, double gain = 1.0) override;
    
    void insertMarker(int sample) { markers.add(sample); } // TODO: temp
    
private:
    
    void draw(juce::Array<juce::Colour>* colours, juce::Array<float>* levels) override;
    
    bool isBeat(int frameIndex, bool& downbeat) override;
    
    int windowStartX, windowWidth;
    
    juce::Array<int> markers; // TODO: temp
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformScrollBar)
};

#endif /* WaveformScrollBar_hpp */
