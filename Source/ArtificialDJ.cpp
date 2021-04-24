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
            generateMix(mixQueue.getFirst().nextTrack, chooseTrack(true)); // TODO: not true
    }
}


MixInfo* ArtificialDJ::getNextMix(MixInfo* current)
{
    const juce::ScopedLock sl(lock);
    
    if (current)
    {
        mixQueue.remove(current);
    }
    
    return &mixQueue.getReference(0);
}


void ArtificialDJ::playPause()
{
    if (!initialised.load())
        initialise();
    
    const juce::ScopedLock sl(lock);
    
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
    TrackInfo trackFirst = chooseTrack(true);
    TrackInfo trackSecond = chooseTrack(true);
    generateMix(trackFirst, trackSecond);
    
    TrackProcessor* leader = audioProcessor->getProcessor(0);
    TrackProcessor* next = audioProcessor->getProcessor(1);
    
    leader->loadFirstTrack(trackFirst);
    
    next->getTrack()->leader = true;
    next->update();
    
    startThread();
}


TrackInfo ArtificialDJ::chooseTrack(bool random)
{
    int randomChoice;
    TrackInfo* track = nullptr;
    
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
    
    return *track;
}

void ArtificialDJ::generateMix(TrackInfo leadingTrack, TrackInfo nextTrack)
{
    MixInfo mix;
    
    mix.leadingTrack = leadingTrack;
    mix.nextTrack = nextTrack;
    mix.nextTrackAudio = dataManager->loadAudio(nextTrack.filename);
    
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
