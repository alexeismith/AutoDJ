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


// Enable the following macro to process only one audio channel
//#define STRETCHER_MONO


/**
 Handles time-stretching of track audio.
 Essentially a wrapper for the SoundTouch library, which requires interleaved input and output audio buffers.
 The SoundTouch library has processing latency, meaning extra samples have be given in order to receive a certain number of output samples.
 Therefore this class keeps its own playhead for tracking the number samples input.
 */
class TimeStretcher
{
public:
    
    /** Constructor. */
    TimeStretcher();
    
    /** Destructor. */
    ~TimeStretcher() {}
    
    /** Prepares the processing pipeline for a given audio buffer size.
    
    @param[in] blockSize Number of audio samples to expect in each processing block */
    void prepare(int blockSize);
    
    /** Time-stretches the supplied audio data.
     First, set the time stretch factor using update().
     The desired number of output samples is specified, while the number of input samples processed depends on the stretch factor.
     
     @param[in] input Pointer to input audio to be stretched
     @param[in] output Pointer to buffer in which to place output audio
     @param[in] numSamples Number of samples required in the output
     
     @return Number of input samples that correspond to the stretched output
     */
    int process(juce::AudioBuffer<float>* input, juce::AudioBuffer<float>* output, int numSamples);
    
    /** Updates the stretch factor, based on original and target tempos.
     
     @param[in] bpmOriginal Original tempo of the current track
     @param[in] bpmTarget Desired tempo for the current track */
    void update(double bpmOriginal, double bpmTarget);
    
    /** Resets the input playhead, with a optional position argument.
     
     @param[in] sample New playhead position, in audio samples */
    void resetPlayhead(int sample = 0);
    
    /** Fetches the current time stretch factor.
     
     @return The time stretch factor, where 1.0 is original tempo, >1.0 is faster, and <1.0 is slower */
    double getTimeStretch() { return timeStretch.load(); }
    
    /** Resets the stretcher ready for a new track to be processed. */
    void reset();
    
private:
    
    /** Interleaves the provided input audio data.
     
     @param[in,out] input Pointer to input audio buffer
     @param[in] numSamples Number of stereo input samples to interleave */
    void interleave(juce::AudioBuffer<float>* input, int numSamples);
    
    /** De-interleaves the provided output audio data.
    
    @param[in,out] input Pointer to output audio buffer
    @param[in] numSamples Number of stereo output samples to produce */
    void deinterleave(juce::AudioBuffer<float>* output, int numSamples);
    
    juce::AudioBuffer<float> inputInterleaved; ///< Intermediate buffer holding interleaved audio samples to be given to SoundTouch
    juce::AudioBuffer<float> outputInterleaved; ///< intermediate buffer to receive interleaved audio output from SoundTouch
    
    soundtouch::SoundTouch shifter; ///< Instance of SoundTouch time-stretcher
    
    std::atomic<double> timeStretch = 1.0; ///< Current time-stretch factor, where 1.0 is original tempo, >1.0 is faster, and <1.0 is slower
    int playhead = 0; ///< Current position in input audio (see class description)

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeStretcher) ///< JUCE macro to add a memory leak detector
};

#endif /* TimeStretcher_hpp */
