//
//  TrackProcessor.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 19/04/2021.
//

#ifndef TrackProcessor_hpp
#define TrackProcessor_hpp

#include <JuceHeader.h>

#include "DeckComponent.hpp"

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
    
    Track* getTrack();
    
    bool isLeader() { return getTrack()->leader; }
    
    void nextMix();
    
    bool isReady() { return ready; }
    
    void update();
    
    void loadNextTrack();
    
    void loadFirstTrack(TrackInfo trackInfo, bool leader);
    
    void prepare(int blockSize);
    
    void setPartner(TrackProcessor* p) { partner = p; }
    
    double getTimeStretch() { return timeStretch; }
    
    void setDeck(DeckComponent* d) { deck = d; }
    
    void updateDeck();
    
private:
    
    int getAudioLength() { return track->audio->getNumSamples(); }
    
    void processShifts(int numSamples);
    
    void simpleCopy(int numSamples);
    
    void updateShifts();
    
    juce::CriticalSection lock;
    
    DeckComponent* deck = nullptr;
    TrackProcessor* partner = nullptr;
    TrackDataManager* dataManager = nullptr;
    ArtificialDJ* dj = nullptr;
    
    bool deckNeedsTrackUpdate = true;
    
    std::atomic<bool> ready;
    
    std::unique_ptr<Track> track;
    MixInfo currentMix;
    
    int shifterPlayhead = 0;
    
    juce::AudioBuffer<float> output;
    
    soundtouch::SoundTouch shifter;
    double timeStretch = 1;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif /* TrackProcessor_hpp */
