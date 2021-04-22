//
//  ArtificialDJ.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 20/04/2021.
//

#include "ArtificialDJ.hpp"

#define MIX_QUEUE_LENGTH (3)


ArtificialDJ::ArtificialDJ(TrackDataManager* dm) :
    juce::Thread("ArtificialDJ"), dataManager(dm)
{
    initialised.store(false);
}


void ArtificialDJ::run()
{
    
    
    while (!threadShouldExit())
    {
        // If no tracks loaded / mix queue empty, create a mix without referencing previous
        
        if (mixQueue.size() < MIX_QUEUE_LENGTH)
            generateMix(mixQueue.getFirst().nextTrack, *chooseTrack(true)); // TODO: not true
    }
}


MixData* ArtificialDJ::getNextMix(MixData* current)
{
    const juce::ScopedLock sl (lock);
    
    if (current)
        mixQueue.remove(current);
    
    return &mixQueue.getReference(0);
}


void ArtificialDJ::playPause()
{
    if (!initialised.load())
        initialise();
    
    const juce::ScopedLock sl (lock);
    
    if (!playing)
    {
        audioProcessor->play();
        playing = true;
    }
    else
    {
        audioProcessor->pause();
        playing = false;
    }
}


void ArtificialDJ::initialise()
{
    // TODO: maybe don't need pointers?
    TrackData* trackFirst = chooseTrack(true);
    TrackData* trackSecond = chooseTrack(true);
    generateMix(*trackFirst, *trackSecond);
//    generateMix(*trackFirst, *trackSecond);
    
    TrackProcessor* leader = audioProcessor->getProcessor(0);
    TrackProcessor* next = audioProcessor->getProcessor(1);
    
    leader->initialise(*trackFirst);
    
//    TrackData leadingTrack = mixQueue.getFirst().leadingTrack;
//    leader->getState()->bpm.currentValue = mixQueue.getReference(0).bpm;
//    leader->getState()->gain.currentValue = 1.0;
//    leader->getState()->track = leadingTrack;
//    leader->updateState();
//    leader->loadTrack();
//    leader->seek(0);
    
    next->getState()->leader = true;
    next->updateState();
    
    MixData mix = mixQueue.getReference(0);
    DBG("start: " << mix.start);
    DBG("end: " << mix.end);
    DBG("startNext: " << mix.startNext);
    DBG("endNext: " << mix.endNext);
    
    startThread();
}


TrackData* ArtificialDJ::chooseTrack(bool random)
{
    int randomChoice;
    TrackData* track = nullptr;
    
    // TODO: handle case where all tracks have been played, currrently infinite loop
    // Use separate unplayed list, which keeps updated with the normal one
    
    if (random)
    {
        do
        {
            randomChoice = rand() % dataManager->getTracks()->size();
            track = &dataManager->getTracks()->getReference(randomChoice);
        } while (!track->analysed || track->queued);
    }
    else
    {
        // TODO: Take into account BPM, Key, Energy
    }
    
    DBG("QUEUED: " << track->filename);
    track->queued = true;
    
    return track;
}

void ArtificialDJ::generateMix(TrackData leadingTrack, TrackData nextTrack)
{
    MixData mix;
    
    mix.leadingTrack = leadingTrack;
    mix.nextTrack = nextTrack;
    
    int mixLengthBeats = 16;
    
    int mixStartBeats = mix.leadingTrack.downbeat + 2 * mixLengthBeats;
    
    mix.start = mix.leadingTrack.getSampleOfBeat(mixStartBeats);
    mix.end = mix.leadingTrack.getSampleOfBeat(mixStartBeats + mixLengthBeats);
    
    mixStartBeats = mix.nextTrack.downbeat;
    
    mix.startNext = mix.nextTrack.getSampleOfBeat(mixStartBeats);
    mix.endNext = mix.nextTrack.getSampleOfBeat(mixStartBeats + mixLengthBeats);
    
    mix.bpm = double(mix.leadingTrack.bpm + mix.nextTrack.bpm) / 2;
    
    mixQueue.add(mix);
}
