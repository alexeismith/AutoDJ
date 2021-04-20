//
//  TrackProcessor.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 19/04/2021.
//

#ifndef TrackProcessor_hpp
#define TrackProcessor_hpp

#include <JuceHeader.h>

#include "TrackDataManager.hpp"

#include "ThirdParty/soundtouch/include/SoundTouch.h"

class TrackProcessor
{
public:
    
    TrackProcessor(TrackDataManager* dm);
    
    ~TrackProcessor() {}
    
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    
    void load(TrackData track);
    
    void seek(int sample) { inputPlayhead = sample; }
    
    void seekClip(int sample, int length);
    
    void newTempoShift(double shiftBpm, int byInputSample = -1);
    
    void newPitchShift(double shiftSemitones, int byInputSample = -1);
    
private:
    
    void reset();
    
    void processShifts(const juce::AudioSourceChannelInfo& bufferToFill);
    
    void newShift(double shift, int byInputSample, double& current, double& target, double& rate, int& samplesRemaining);
    
    void updateShifts(int samplesRequested);
    
    void updateShift(double& current, double target, double rate, int& samplesRemaining, int samplesRequested);
    
    juce::CriticalSection lock;
    
    TrackDataManager* dataManager = nullptr;
    
    bool ready;
    
    TrackData currentTrack;
    
    juce::AudioBuffer<float> input;
    
    int inputLength;
    int inputPlayhead = -1;
    
    soundtouch::SoundTouch shifter;
    
    double shiftBpmCurrent;
    double shiftBpmTarget;
    double shiftBpmRate;
    int shiftBpmSamplesRemaining = -1;
    
    double shiftPitchCurrent;
    double shiftPitchTarget;
    double shiftPitchRate;
    int shiftPitchSamplesRemaining = -1;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif /* TrackProcessor_hpp */
