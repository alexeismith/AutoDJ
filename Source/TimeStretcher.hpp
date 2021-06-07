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


//#define STRETCHER_MONO


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
    
    double getTimeStretch() { return timeStretch.load(); }
    
    void reset();
    
private:
    
    void interleave(juce::AudioBuffer<float>* input, int numSamples);
    
    void deinterleave(juce::AudioBuffer<float>* output, int numSamples);
    
    juce::AudioBuffer<float> inputInterleaved;
    juce::AudioBuffer<float> outputInterleaved;
    
    soundtouch::SoundTouch shifter;
    std::atomic<double> timeStretch = 1.0;
    int playhead = 0;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeStretcher) ///< JUCE macro to add a memory leak detector
};

#endif /* TimeStretcher_hpp */
