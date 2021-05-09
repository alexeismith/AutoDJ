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

class TrackLoadThread;

class TrackProcessor
{
public:
    
    TrackProcessor(TrackDataManager* dm, ArtificialDJ* DJ);
    
    ~TrackProcessor() {}
    
    int getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    
    Track* getTrack() { return track.get(); }
    
    bool isLeader() { return getTrack()->leader; }
    
    void nextMix();
    
    bool isReady() { return ready; }
    
    void loadNextTrack();
    
    void loadFirstTrack(TrackInfo* trackInfo, bool leader, juce::AudioBuffer<float>* audio = nullptr);
    
    void prepare(int blockSize);
    
    void setPartner(TrackProcessor* p) { partner = p; }
    
    double getTimeStretch() { return timeStretch; }
    
    Track* getNewTrack();
    
    void syncWithLeader(int leaderPlayhead);
    
    void skipToNextEvent();
    
private:
    
    void update(int numSamples);
    
    void resetPlayhead(int sample = 0);
    
    int getAudioLength() { return track->audio->getNumSamples(); }
    
    void processShifts(int numSamples);
    
    void simpleCopy(int numSamples);
    
    void updateShifts();
    
    juce::CriticalSection lock;
    
    std::unique_ptr<TrackLoadThread> trackLoader;

    TrackProcessor* partner = nullptr;
    TrackDataManager* dataManager = nullptr;
    ArtificialDJ* dj = nullptr;
    
    bool trackEnd = false;
    bool newTrack = true;
    bool play = false;
    
    std::atomic<bool> ready;
    
    std::unique_ptr<Track> track;
    MixInfo currentMix;
    
    int shifterPlayhead = 0;
    
    juce::AudioBuffer<float> output;
    
    soundtouch::SoundTouch shifter;
    double timeStretch = 1;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};


class TrackLoadThread : public juce::Thread
{
public:
    
    TrackLoadThread(TrackProcessor* processor) :
        juce::Thread("TrackLoad"), trackProcessor(processor) {}
    
    ~TrackLoadThread() { stopThread(1000); }
    
    void run() { trackProcessor->loadNextTrack(); }
    
private:
    
    TrackProcessor* trackProcessor = nullptr;
    
};

#endif /* TrackProcessor_hpp */
