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
    
    int getWindowWidth() { return windowWidth; }
    
private:
    
    void draw(juce::Array<juce::Colour>* colours, juce::Array<float>* levels) override;
    
    bool isBeat(int frameIndex, bool& downbeat) override;
    
    int windowStartX, windowWidth = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformScrollBar) ///< JUCE macro to add a memory leak detector
};

#endif /* WaveformScrollBar_hpp */
