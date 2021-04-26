//
//  AnimationThread.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 26/04/2021.
//

#ifndef AnimationThread_hpp
#define AnimationThread_hpp

#include <JuceHeader.h>

#include "WaveformComponent.hpp"

typedef struct AnimationInfo
{
    std::atomic<int> playhead = 0;
    std::atomic<double> timeStretch = 1.0;
    std::atomic<double> gain = 0.0;
} AnimationInfo;


class AnimationThread : public juce::HighResolutionTimer
{
public:
    
    AnimationThread() {}
    
    ~AnimationThread() {}
    
    void hiResTimerCallback();
    
    void update(int deckId, int playhead, double timeStretch, double gain);
    
private:
    
    AnimationInfo info1;
    AnimationInfo info2;
    
    WaveformComponent* waveform1;
    WaveformComponent* waveform2;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimationThread)
};

#endif /* AnimationThread_hpp */
