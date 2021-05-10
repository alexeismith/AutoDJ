//
//  TimeStretcher.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 10/05/2021.
//

#ifndef TimeStretcher_hpp
#define TimeStretcher_hpp


#include <JuceHeader.h>

#include "ThirdParty/soundtouch/include/SoundTouch.h"


class TimeStretcher
{
public:
    
    TimeStretcher();
    
    ~TimeStretcher() {}
    
    void prepare(int blockSize);
    
    // Returns the number of input samples that correspond to the number output,
    // taking into account the stretch applied
    int process(juce::AudioBuffer<float>* input, juce::AudioBuffer<float>* output, int numSamples);
    
    void update(double bpmOriginal, double bpmTarget);
    
    void resetPlayhead(int sample = 0);
    
    double getTimeStretch() { return timeStretch; }
    
    void reset();
    
private:
    
    juce::AudioBuffer<float> buffer;
    
    soundtouch::SoundTouch shifter;
    double timeStretch = 1;
    int playhead = 0;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeStretcher)
};

#endif /* TimeStretcher_hpp */
