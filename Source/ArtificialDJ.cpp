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
    initialise();
    
    while (!threadShouldExit())
    {
        if (mixQueue.size() < MIX_QUEUE_LENGTH)
            generateMix(mixQueue.getFirst().nextTrack, chooseTrack(true)); // TODO: not true
        
        sleep(100);
    }
}


MixInfo ArtificialDJ::getNextMix(MixInfo current)
{
    const juce::ScopedLock sl(lock);
    
    if (mixQueue.getReference(0).id == current.id)
    {
        mixQueue.remove(0);
    }
    
    if (mixQueue.size() == 0)
    {
        jassert(false); // Mix queue was empty!
        generateMix(current.nextTrack, chooseTrack(true));
    }
    
    return mixQueue.getUnchecked(0);
}


bool ArtificialDJ::playPause()
{
    if (!initialised.load())
    {
        startThread();
        return false;
    }
        
    
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
    
    return true;
}


void ArtificialDJ::initialise()
{
    
    
    // TODO: maybe don't need pointers?
    TrackInfo trackFirst = chooseTrack(true);
    TrackInfo trackSecond = chooseTrack(true);
    generateMix(trackFirst, trackSecond);
    
    TrackProcessor* leader = audioProcessor->getTrackProcessor(0);
    TrackProcessor* next = audioProcessor->getTrackProcessor(1);
    
    leader->loadFirstTrack(trackFirst, true);
    next->loadFirstTrack(trackSecond, false);
    
    initialised.store(true);
    
    playPause();
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
//        } while (!track->analysed); TODO: remove
    }
    else
    {
        // TODO: Take into account BPM, Key, Energy
    }
    
//    DBG("QUEUED: " << track->filename);
    track->queued = true;
    
    return *track;
}

void ArtificialDJ::generateMix(TrackInfo leadingTrack, TrackInfo nextTrack)
{
    MixInfo mix;
    
    mix.id = mixIdCounter;
    mixIdCounter += 1;
    
    mix.leadingTrack = leadingTrack;
    mix.nextTrack = nextTrack;
    mix.nextTrackAudio = dataManager->loadAudio(nextTrack.filename, true);
    
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
