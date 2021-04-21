//
//  ArtificialDJ.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 20/04/2021.
//

#include "ArtificialDJ.hpp"

#define MIX_QUEUE_LENGTH (3)


ArtificialDJ::ArtificialDJ(AudioProcessor* p, TrackDataManager* dm) :
    juce::Thread("ArtificialDJ"), audioProcessor(p), dataManager(dm)
{
    
}


void ArtificialDJ::run()
{
    while (!threadShouldExit())
    {
        // If no tracks loaded / mix queue empty, create a mix without referencing previous
        
        if (mixQueue.size() < MIX_QUEUE_LENGTH)
            generateMix(chooseTrack());
    }
}


void ArtificialDJ::updateMix(MixState& state)
{
    const juce::ScopedLock sl (lock);
    
    MixData* mix = &mixQueue.getReference(0);
    
    int mixEnd = state.leader ? mix->end : mix->endNext;
    
    if (state.currentSample >= mixEnd)
    {
        if (state.leader)
            state.finish = true;
        else
            state.leader = true;
    }
    
    
    
    
}

TrackData ArtificialDJ::chooseTrack()
{
    int randomChoice;
    TrackData track;
    
    // Take into account: Unplayed, BPM, Key, Energy
    
    do {
        randomChoice = rand() % dataManager->getTracks()->size();
        track = dataManager->getTracks()->getReference(randomChoice);
    } while (!track.analysed);
    
    return track;
}

MixData ArtificialDJ::generateMix(TrackData nextTrack)
{
    MixData mix;
    
    //    if (!mixQueue.isEmpty())
    mix.leadingTrack = mixQueue.getLast().nextTrack; // TODO: handle when queue is empty
    
    mix.nextTrack = nextTrack;
    
    int mixLengthBeats = 16;
    
    int mixStartBeats = mix.leadingTrack.downbeat + 3 * mixLengthBeats;
    
    mix.start = mix.leadingTrack.getSampleOfBeat(mixStartBeats);
    mix.end = mix.leadingTrack.getSampleOfBeat(mixStartBeats + mixLengthBeats);
    
    mixStartBeats = mix.nextTrack.downbeat;
    
    mix.startNext = mix.nextTrack.getSampleOfBeat(mixStartBeats);
    mix.endNext = mix.nextTrack.getSampleOfBeat(mixStartBeats + mixLengthBeats);
    
    mix.bpm = double(mix.leadingTrack.bpm + mix.nextTrack.bpm) / 2;
    
    return mix;
}
