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
    
    chooser.reset(new TrackChooser(dataManager, &randomGenerator));
}


void ArtificialDJ::run()
{
    initialise();
    
    while (!threadShouldExit() && !ending)
    {
        if (mixQueue.size() < MIX_QUEUE_LENGTH)
            generateMix();
        
        sleep(1000);
    }
}


MixInfo ArtificialDJ::getNextMix(MixInfo current)
{
    const juce::ScopedLock sl(lock);
    
    removeMix(current);
    
    if (mixQueue.size() == 0)
    {
        if (ending)
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


bool ArtificialDJ::isMixReady(int minimumReady)
{
    if (!initialised.load())
        return false;
    
    const juce::ScopedLock sl(lock);
    
    return (mixQueue.size() >= minimumReady);
}


bool ArtificialDJ::canSkip()
{
    if (!initialised.load())
        return false;
    
    if (!playing)
        return false;
    
    return isMixReady(2);
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
    leadingTrack = firstTrack;
    
    juce::AudioBuffer<float>* firstTrackAudio = dataManager->loadAudio(firstTrack->getFilename());
    leadingTrackSegments = segmenter.analyse(firstTrack, firstTrackAudio);
    
    generateMix();
    
    leader->loadFirstTrack(firstTrack, true, firstTrackAudio);
    next->loadFirstTrack(leadingTrack, false);
    
    initialised.store(true);
    
    playPause();
}


void ArtificialDJ::generateMixSimple()
{
    MixInfo mix;
    
    mix.id = mixIdCounter;
    mixIdCounter += 1;
    
    mix.leadingTrack = leadingTrack;
    
    TrackInfo* nextTrack = chooser->chooseTrack();
    
    // If nextTrack is null, there are no more tracks to play
    if (nextTrack == nullptr)
    {
        // Set the end of mix flag
        ending = true;
        // Set the mix bpm to that of the leading track
        mix.bpm = mix.leadingTrack->bpm;
        // Set the start of this final mix to the end of the leading track, so it simply plays all the way through
        mix.start = mix.leadingTrack->getLengthSamples();
        // Add the final mix to the mix queue
        mixQueue.add(mix);
        return;
    }
    
    // Otherwise, we can proceed normally...
    
    mix.nextTrack = nextTrack;
    mix.nextTrackAudio = dataManager->loadAudio(nextTrack->getFilename(), true);
    
    int mixLengthBeats = 16;
    
    int mixStartBeats = mix.leadingTrack->downbeat + 2 * mixLengthBeats;
    
    mix.start = mix.leadingTrack->getSampleOfBeat(mixStartBeats);
    mix.end = mix.leadingTrack->getSampleOfBeat(mixStartBeats + mixLengthBeats);
    
    mixStartBeats = mix.nextTrack->downbeat;
    
    mix.startNext = mix.nextTrack->getSampleOfBeat(mixStartBeats);
    mix.endNext = mix.nextTrack->getSampleOfBeat(mixStartBeats + mixLengthBeats);
    
    mix.bpm = double(mix.leadingTrack->bpm + mix.nextTrack->bpm) / 2;
    
    mixQueue.add(mix);
    
    leadingTrack = nextTrack;
}


void ArtificialDJ::generateMixComplex()
{
    MixInfo mix;

    // Set ID for the new mix
    mix.id = mixIdCounter;
    mixIdCounter += 1;

    // Set the leading track as the previously chosen one
    mix.leadingTrack = leadingTrack;

    // Choose a new track to play
    TrackInfo* nextTrack = chooser->chooseTrack();

    // If nextTrack is null, there are no more tracks to play
    if (mix.nextTrack == nullptr)
    {
        // Set the end of mix flag
        ending = true;
        // Set the mix bpm to that of the leading track
        mix.bpm = mix.leadingTrack->bpm;
        // Set the start of this final mix to the end of the leading track, so it simply plays all the way through
        mix.start = mix.leadingTrack->getLengthSamples();
        // Add the final mix to the mix queue
        mixQueue.add(mix);
        return;
    }

    // Otherwise, we can proceed normally...

    mix.nextTrack = nextTrack;

    // Load the audio for the next track
    mix.nextTrackAudio = dataManager->loadAudio(nextTrack->getFilename());
    // Use segmentation analyse to classify sections within the track
    juce::Array<int> nextTrackSegments = segmenter.analyse(nextTrack, mix.nextTrackAudio);

    // Set the mix bpm to half way between each track
    mix.bpm = double(mix.leadingTrack->bpm + nextTrack->bpm) / 2;


    // LEADING TRACK START --------------------------------------------------------------

    // The earliest we will start this mix is three fifths through the leading track
    int mixStartMinimum = 4 * leadingTrack->getLengthSamples() / 7;

    // Find how much of the leading track remains available for this mix
    int leadingTrackAvailable = leadingTrack->getLengthSamples() - mixStartMinimum;

    // The latest this mix can start is halfway through the available range
    int mixStartMaximum = mixStartMinimum + leadingTrackAvailable/2;

    // Choose a random sample within this range
    int startOffset = double(leadingTrackAvailable/2) * randomGenerator.getGaussian(0.2, 0.5, 0.5);

    // Initialise the mix start as the minimum, plus the random offset
    mix.start = mixStartMinimum + startOffset;
    // Find the track segment nearest this point
    mix.start = segmenter.findClosestSegment(leadingTrack, &leadingTrackSegments, mix.start, mixStartMinimum, mixStartMaximum);

    // NEXT TRACK START ----------------------------------------------------------------

    // The latest cue point we can set for the next track is a quarter of the way through
    int nextTrackStartMax = nextTrack->getLengthSamples() / 4;

    // Choose a random sample within this range
    mix.startNext = double(nextTrackStartMax) * randomGenerator.getGaussian(0.2, 0.5, 0.0);
    // Find the track segment nearest this point
    mix.startNext = segmenter.findClosestSegment(nextTrack, &nextTrackSegments, mix.startNext, 0, nextTrackStartMax);

    // MIX LENGTH ----------------------------------------------------------------------

    // Recalculate the amount of leading track we have left, based on the chosen mix start
    leadingTrackAvailable = leadingTrack->getLengthSamples() - mix.start;
    
    // Find the largest multiple of 4 bars that fits into this space
    int largestMultiple4 = leadingTrackAvailable / (leadingTrack->getBarLength() * 4);
    
    jassert(largestMultiple4 >= 2);
    
    // Limit to 24 bar max mix length
    largestMultiple4 = juce::jmin(6, largestMultiple4);
    
    int lengthCandidate = -1;
    bool isSegmentLeading, isSegmentNext;
    
    // Loop through the possible mix lengths to see if any land on a segment in both tracks
    for (int i = 2; i < largestMultiple4 + 1; i++)
    {
        // For this length, find the point in each track that the mix would end
        mix.end = mix.start + (i * (leadingTrack->getBarLength() * 4));
        mix.endNext = mix.startNext + (i * (nextTrack->getBarLength() * 4));
        
        // Find whether these points are segments
        isSegmentLeading = segmenter.isSegment(&leadingTrackSegments, mix.end);
        isSegmentNext = segmenter.isSegment(&nextTrackSegments, mix.endNext);
        
        // If they are both segments, choose this mix length and break the loop
        if (isSegmentLeading && isSegmentNext)
        {
            lengthCandidate = i;
            break;
        }
        // Otherwise, store this result and keep looping
        else if (isSegmentLeading || isSegmentNext)
        {
            lengthCandidate = i;
        }
    }
    
    // If no segments were found, choose a random mix length, weighted towards the maximum
    if (lengthCandidate == -1)
    {
        // Get a Gaussian random sample between 0-1, weighted towards 1
        double multiplier = juce::jmin(randomGenerator.getGaussian(0.2, 0.5, 0.7), 1.0);
        // Choose the length using the random multiplier
        lengthCandidate = round(float(largestMultiple4) * multiplier);
        // Ensure the length is at least 2
        lengthCandidate = juce::jmax(lengthCandidate, 2);
    }
        
    // Set the chosen candidate as the mix end point in both tracks
    mix.end = mix.start + (lengthCandidate * (leadingTrack->getBarLength() * 4));
    mix.endNext = mix.startNext + (lengthCandidate * (nextTrack->getBarLength() * 4));
    
    jassert(mix.end <= leadingTrack->getLengthSamples()); // Check there is enough leading track to mix

    // FINALISE ------------------------------------------------------------------------

    mixQueue.add(mix);

    // Store the data for the new track to be mixed in
    // This is so the information can be used to generate the next mix
    leadingTrack = mix.nextTrack;
    leadingTrackSegments = nextTrackSegments;
}
