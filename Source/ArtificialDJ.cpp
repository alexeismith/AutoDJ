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
    
    chooser.reset(new TrackChooser(dataManager));
}


void ArtificialDJ::run()
{
    initialise();
    
    while (!threadShouldExit())
    {
        if (mixQueue.size() < MIX_QUEUE_LENGTH)
            generateMix(mixQueue.getFirst().nextTrack, chooser->chooseTrack());
        
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
        generateMix(current.nextTrack, chooser->chooseTrack());
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
    for (int i = 0; i < 100; i++)
        chooser->initialise();
    
    TrackInfo trackFirst = chooser->chooseTrack();
    TrackInfo trackSecond = chooser->chooseTrack();
    generateMix(trackFirst, trackSecond);
    
    TrackProcessor* leader = audioProcessor->getTrackProcessor(0);
    TrackProcessor* next = audioProcessor->getTrackProcessor(1);
    
    leader->loadFirstTrack(trackFirst, true);
    next->loadFirstTrack(trackSecond, false);
    
    initialised.store(true);
    
    playPause();
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
