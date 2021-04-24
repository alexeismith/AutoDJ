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

#include "TrackState.hpp"

#include "ThirdParty/soundtouch/include/SoundTouch.h"

class ArtificialDJ;

class TrackProcessor
{
public:
    
    TrackProcessor(TrackDataManager* dm, ArtificialDJ* dj);
    
    ~TrackProcessor() {}
    
    bool getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill, bool play = true);
    
    void loadTrack();
    
    // TODO: remove
    void seek(int sample) { playhead = sample; }
    void seekClip(int sample, int length);
    
    TrackState* getState() { return state.get(); }
    
    bool isLeader() { return getState()->leader; }
    
    bool isReady() { return ready; }
    
    void updateState();
    
    void initialise(TrackData track);
    
    void prepare(int blockSize);
    
private:
    
    void processShifts(int numSamples);
    
    void simpleCopy(int numSamples);
    
    juce::CriticalSection lock;
    
    TrackDataManager* dataManager = nullptr;
    
    bool ready;
    
    std::unique_ptr<TrackState> state;
    
    juce::AudioBuffer<float> input;
    int inputLength;
    int inputPlayhead = 0;
    
    juce::AudioBuffer<float> output;
    
    int playhead = 0;
    
    soundtouch::SoundTouch shifter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif /* TrackProcessor_hpp */
