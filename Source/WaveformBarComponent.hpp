//
//  WaveformBarComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 04/05/2021.
//

#ifndef WaveformBarComponent_hpp
#define WaveformBarComponent_hpp

#include "WaveformComponent.hpp"

// NOTE: need a separate instance for each audio channel to be visualised, because the IIR filters depend on previous samples
class WaveformBarComponent : public WaveformComponent
{
public:
    
    WaveformBarComponent();
    
    ~WaveformBarComponent() {}
    
    void draw(int playhead, double timeStretch, double gain) override;
    
private:
    
    bool isBeat(int frameIndex, bool& downbeat) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformBarComponent)
};

#endif /* WaveformBarComponent_hpp */
