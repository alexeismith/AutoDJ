//
//  TrackProcessor.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 19/04/2021.
//

#ifndef TrackProcessor_hpp
#define TrackProcessor_hpp

#include "DataManager.hpp"
#include "TimeStretcher.hpp"

#include "Track.hpp"

#include "ThirdParty/soundtouch/include/SoundTouch.h"

class ArtificialDJ;

class TrackLoadThread;

class TrackProcessor
{
public:
    
    /** Constructor. */
    TrackProcessor(DataManager* dm, ArtificialDJ* DJ);
    
    /** Destructor. */
    ~TrackProcessor() {}
    
    int getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    
    Track* getTrack() { return track.get(); }
    
    bool isLeader() { return getTrack()->leader; }
    
    void nextMix();
    
    bool isReady(bool& mixEnd);
    
    bool mixEnded() { return mixEnd.load(); } 
    
    void loadNextTrack();
    
    void loadFirstTrack(TrackInfo* trackInfo, bool leader, juce::AudioBuffer<float>* audio = nullptr);
    
    void prepare(int blockSize);
    
    void setPartner(TrackProcessor* p) { partner = p; }
    
    double getTimeStretch() { return stretcher->getTimeStretch(); }
    
    Track* getNewTrack();
    
    MixInfo getCurrentMix() { return currentMix; }
    
    void cue(int leaderPlayhead);
    
    void skipToNextEvent();
    
    void reset();
    
private:
    
    void setHighPass(int frequency);
    
    void update(int numSamples);
    
    void resetPlayhead(int sample = 0);
    
    int getAudioLength() { return track->audio->getNumSamples(); }
    
    void processShifts(int numSamples);
    
    
    juce::CriticalSection lock;

    TrackProcessor* partner = nullptr; ///< Pointer to 
    DataManager* dataManager = nullptr; ///< Pointer to app's track data manager
    ArtificialDJ* dj = nullptr; ///< Pointer to artificial DJ brain
    
    std::atomic<bool> newTrack = true;
    std::atomic<bool> mixEnd = false;
    
    bool trackEnd = false;
    bool play = false;
    
    std::atomic<bool> ready;
    
    std::unique_ptr<Track> track;
    MixInfo currentMix;
    
    juce::AudioBuffer<float> output;
    
    std::unique_ptr<TimeStretcher> stretcher;
    
    juce::IIRFilter highPassFilterL, highPassFilterR;
    bool filterOn = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackProcessor) ///< JUCE macro to add a memory leak detector
};

#endif /* TrackProcessor_hpp */
