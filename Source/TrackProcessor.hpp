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

#include "Track.hpp"

#include "ThirdParty/soundtouch/include/SoundTouch.h"

class ArtificialDJ;

class TrackProcessor
{
public:
    
    TrackProcessor(TrackDataManager* dm, ArtificialDJ* DJ);
    
    ~TrackProcessor() {}
    
    bool getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill, bool play = true);
    
    Track* getTrack() { return track.get(); }
    
    bool isLeader() { return getTrack()->leader; }
    
    void nextMix();
    
    bool isReady() { return ready; }
    
    void update();
    
    void loadNextTrack();
    
    void loadFirstTrack(TrackInfo trackInfo, bool leader);
    
    void prepare(int blockSize);
    
    void setPartner(TrackProcessor* p) { partner = p; }
    
private:
    
    int getAudioLength() { return track->audio->getNumSamples(); }
    
    void processShifts(int numSamples);
    
    void simpleCopy(int numSamples);
    
    juce::CriticalSection lock;
    
    TrackProcessor* partner;
    
    TrackDataManager* dataManager = nullptr;
    ArtificialDJ* dj = nullptr;
    
    bool ready;
    
    std::unique_ptr<Track> track;
    MixInfo currentMix;
    
    int shifterPlayhead = 0;
    
    juce::AudioBuffer<float> output;
    
    soundtouch::SoundTouch shifter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif /* TrackProcessor_hpp */
