//
//  ArtificialDJ.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 20/04/2021.
//

#include "ArtificialDJ.hpp"

#define MIX_QUEUE_LENGTH (2)


ArtificialDJ::ArtificialDJ(TrackDataManager* dm) :
    juce::Thread("ArtificialDJ"), dataManager(dm)
{
    initialised.store(false);
    
    chooser.reset(new TrackChooser(dataManager, &randomGenerator));
}


void ArtificialDJ::run()
{
    initialise();
    
    while (!threadShouldExit() && !ending)
    {
        if (mixQueue.size() < MIX_QUEUE_LENGTH)
            generateMix();
        
        sleep(100);
    }
}


MixInfo ArtificialDJ::getNextMix(MixInfo current)
{
    const juce::ScopedLock sl(lock);
    
    removeMix(current);
    
    if (mixQueue.size() == 0)
    {
        if (ending == true)
            return MixInfo();
        
        jassert(false); // Mix queue was empty!
        generateMix();
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


void ArtificialDJ::removeMix(MixInfo mix)
{
    if (mixQueue.size() > 0)
    {
        if (mixQueue.getUnchecked(0).id == mix.id)
        {
            mixQueue.remove(0);
        }
    }
}


void ArtificialDJ::initialise()
{
    TrackProcessor* leader = audioProcessor->getTrackProcessor(0);
    TrackProcessor* next = audioProcessor->getTrackProcessor(1);
    
    chooser->initialise();
    
    TrackInfo* firstTrack = chooser->chooseTrack();
    prevTrack = firstTrack;
    
    generateMix();
    
    leader->loadFirstTrack(firstTrack, true, dataManager->loadAudio(firstTrack->getFilename()));
    next->loadFirstTrack(prevTrack, false);
    
    initialised.store(true);
    
    playPause();
}


void ArtificialDJ::generateMix()
{
    MixInfo mix;
    
    mix.id = mixIdCounter;
    mixIdCounter += 1;
    
    TrackInfo* nextTrack = chooser->chooseTrack();
    
    mix.leadingTrack = prevTrack;
    
    if (nextTrack == nullptr)
    {
        mix.nextTrack->bpm = mix.leadingTrack->bpm; // TODO: nextTrack uninitialised here?
        ending = true;
    }
    else
    {
        mix.nextTrack = nextTrack;
        mix.nextTrackAudio = dataManager->loadAudio(nextTrack->getFilename(), true);
    }
    
    int mixLengthBeats = 16;
    
    int mixStartBeats = mix.leadingTrack->downbeat + 2 * mixLengthBeats;
    
    mix.start = mix.leadingTrack->getSampleOfBeat(mixStartBeats);
    mix.end = mix.leadingTrack->getSampleOfBeat(mixStartBeats + mixLengthBeats);
    
    mixStartBeats = mix.nextTrack->downbeat;
    
    mix.startNext = mix.nextTrack->getSampleOfBeat(mixStartBeats);
    mix.endNext = mix.nextTrack->getSampleOfBeat(mixStartBeats + mixLengthBeats);
    
    mix.bpm = double(mix.leadingTrack->bpm + mix.nextTrack->bpm) / 2;
    
    mixQueue.add(mix);
    
    prevTrack = nextTrack;
}
