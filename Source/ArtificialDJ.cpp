//
//  ArtificialDJ.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 20/04/2021.
//

#include "ArtificialDJ.hpp"

#define MIX_QUEUE_LENGTH (3)


ArtificialDJ::ArtificialDJ(DataManager* dm) :
    juce::Thread("ArtificialDJ"), dataManager(dm)
{
    initialised.store(false);
    
    chooser.reset(new TrackChooser(dataManager, &randomGenerator));
}


void ArtificialDJ::run()
{
    // First entry into this thread, so we must initialise the mix queue
    initialise();
    
    // While the thread is allowed to continue...
    while (!threadShouldExit())
    {
        // If the mix queue is not full, generate a new mix
        if (mixQueue.size() < MIX_QUEUE_LENGTH)
            generateMix();
        
        // Pause this thread for 1 second
        sleep(1000);
    }
}


MixInfo ArtificialDJ::getNextMix(MixInfo current)
{
    const juce::ScopedLock sl(lock);
    
    if (ending.load())
        endingConfirm.store(true);
    
    removeMix(current);
    
    if (mixQueue.size() == 0)
    {
        if (ending.load())
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


void ArtificialDJ::reset()
{
    stopThread(5000);
    
    playing = false;
    
    initialised.store(false);
    ending.store(false);
    endingConfirm.store(false);
    
    mixIdCounter = 0;
    
    leadingTrack = nullptr;
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
    // Fetch the track processors
    // (Doesn't matter which one becomes the leader)
    TrackProcessor* leader = audioProcessor->getTrackProcessor(0);
    TrackProcessor* follower = audioProcessor->getTrackProcessor(1);
    
    // Initialise the track chooser
    chooser->initialise();
    
    // Choose the first track to play
    TrackInfo* firstTrack = chooser->chooseTrack();
    // This track will lead the mix, until a transition to the next track is fully completed
    leadingTrack = firstTrack;
    
    // Load the audio for the first track
    juce::AudioBuffer<float>* firstTrackAudio = dataManager->loadAudio(firstTrack->getFilename());
    // Find musical segments in the first track
    leadingTrackSegments = segmenter.analyse(firstTrack, firstTrackAudio);
    
    // Generate the first transition (this picks the second track)
    generateMix();
    
    // Tell the processors to load the track information and prepare to play
    leader->loadFirstTrack(firstTrack, true, firstTrackAudio);
    follower->loadFirstTrack(leadingTrack, false);
    
    // Initialisation is complete
    initialised.store(true);
    
    // We are currently paused, so play
    playPause();
}


void ArtificialDJ::generateMixSimple()
{
    MixInfo mix;
    
    if (endingConfirm.load())
        return;
    
    // Set ID for the new mix
    mix.id = mixIdCounter;
    mixIdCounter += 1;
    
    // Set the leading track as the previously chosen one
    mix.leadingTrack = leadingTrack;
    
    // Choose a new track to play
    TrackInfo* nextTrack = chooser->chooseTrack();

    // If nextTrack is null, there are no more tracks to play
    if (nextTrack == nullptr)
    {
        if (ending.load())
            return;
        
        DBG("ENDING MIX");
        
        // Set the end of mix flag
        ending.store(true);
        // Set the mix bpm to that of the leading track
        mix.bpm = mix.leadingTrack->bpm;
        // Set the start and end of this final mix to the end of the leading track, so it simply plays all the way through
        mix.leaderStart = mix.leaderEnd = mix.leadingTrack->getLengthSamples();
        // Add the final mix to the mix queue
        mixQueue.add(mix);
        return;
    }
    
    if (ending.load())
    {
        mixQueue.removeLast();
        ending.store(false);
        DBG("CANELLED MIX END");
    }
    
    // Otherwise, we can proceed normally...
    
    mix.nextTrack = nextTrack;
    
    // Set the mix bpm to half way between each track
    mix.bpm = double(mix.leadingTrack->bpm + nextTrack->bpm) / 2;
    
    // Load the audio for the next track
    mix.nextTrackAudio = dataManager->loadAudio(nextTrack->getFilename());
    
    // Choose a constant mix length (see generateMixComplex() for intelligent mixing)
    int mixLengthBeats = 16;
    
    // LEADING TRACK START --------------------------------------------------------------
    
    // We will start the transition 2xmixLengthBeats into the leading track
    int mixStartBeats = mix.leadingTrack->downbeat + 2 * mixLengthBeats;
    
    mix.leaderStart = mix.leadingTrack->getSampleOfBeat(mixStartBeats);
    mix.leaderEnd = mix.leadingTrack->getSampleOfBeat(mixStartBeats + mixLengthBeats);
    
    // NEXT TRACK START ----------------------------------------------------------------
    
    // We will start the next track at its first downbeat
    mixStartBeats = mix.nextTrack->downbeat;
    
    mix.followerStart = mix.nextTrack->getSampleOfBeat(mixStartBeats);
    mix.followerEnd = mix.nextTrack->getSampleOfBeat(mixStartBeats + mixLengthBeats);
    
    // FINALISE ------------------------------------------------------------------------
    
    mixQueue.add(mix);
    
    leadingTrack = nextTrack;
}


void ArtificialDJ::generateMixComplex()
{
    MixInfo mix;
    
    if (endingConfirm.load())
        return;

    // Set ID for the new mix
    mix.id = mixIdCounter;
    mixIdCounter += 1;

    // Set the leading track as the previously chosen one
    mix.leadingTrack = leadingTrack;

    // Choose a new track to play
    TrackInfo* nextTrack = chooser->chooseTrack();

    // If nextTrack is null, there are no more tracks to play
    if (nextTrack == nullptr)
    {
        if (ending.load())
            return;
        
        DBG("ENDING MIX");
        
        // Set the end of mix flag
        ending.store(true);
        // Set the mix bpm to that of the leading track
        mix.bpm = mix.leadingTrack->bpm;
        // Set the start and end of this final mix to the end of the leading track, so it simply plays all the way through
        mix.leaderStart = mix.leaderEnd = mix.leadingTrack->getLengthSamples();
        // Add the final mix to the mix queue
        mixQueue.add(mix);
        return;
    }
    
    if (ending.load())
    {
        mixQueue.removeLast();
        ending.store(false);
        DBG("CANELLED MIX END");
    }

    // Otherwise, we can proceed normally...

    mix.nextTrack = nextTrack;
    
    // Set the mix bpm to half way between each track
    mix.bpm = double(mix.leadingTrack->bpm + nextTrack->bpm) / 2;

    // Load the audio for the next track
    mix.nextTrackAudio = dataManager->loadAudio(nextTrack->getFilename());
    // Use segmentation analyse to classify sections within the track
    juce::Array<int> nextTrackSegments = segmenter.analyse(nextTrack, mix.nextTrackAudio);


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
    mix.leaderStart = mixStartMinimum + startOffset;
    // Find the track segment nearest this point
    mix.leaderStart = segmenter.findClosestSegment(leadingTrack, &leadingTrackSegments, mix.leaderStart, mixStartMinimum, mixStartMaximum);

    // NEXT TRACK START ----------------------------------------------------------------

    // The latest cue point we can set for the next track is a quarter of the way through
    int nextTrackStartMax = nextTrack->getLengthSamples() / 4;

    // Choose a random sample within this range
    mix.followerStart = double(nextTrackStartMax) * randomGenerator.getGaussian(0.2, 0.5, 0.0);
    // Find the track segment nearest this point
    mix.followerStart = segmenter.findClosestSegment(nextTrack, &nextTrackSegments, mix.followerStart, 0, nextTrackStartMax);

    // MIX LENGTH ----------------------------------------------------------------------

    // Recalculate the amount of leading track we have left, based on the chosen mix start
    leadingTrackAvailable = leadingTrack->getLengthSamples() - mix.leaderStart;
    
    // Find the largest multiple of 4 bars that fits into this space
    int largestMultiple4 = leadingTrackAvailable / (leadingTrack->getBarLength() * 4);
    
    // Limit to 24 bar max mix length
    largestMultiple4 = juce::jmin(6, largestMultiple4);
    
    int lengthCandidate = -1;
    bool isSegmentInLeader, isSegmentInFollower;
    
    // Loop through the possible mix lengths to see if any land on a segment in both tracks
    for (int i = 2; i < largestMultiple4 + 1; i++)
    {
        // For this length, find the point in each track that the mix would end
        mix.leaderEnd = mix.leaderStart + (i * (leadingTrack->getBarLength() * 4));
        mix.followerEnd = mix.followerStart + (i * (nextTrack->getBarLength() * 4));
        
        // Find whether these points are segments
        isSegmentInLeader = segmenter.isSegment(&leadingTrackSegments, mix.leaderEnd);
        isSegmentInFollower = segmenter.isSegment(&nextTrackSegments, mix.followerEnd);
        
        // If they are both segments, choose this mix length and break the loop
        if (isSegmentInLeader && isSegmentInFollower)
        {
            lengthCandidate = i;
            break;
        }
        // Otherwise, store this result and keep looping
        else if (isSegmentInLeader || isSegmentInFollower)
        {
            lengthCandidate = i;
        }
    }
    
    // If no segments were found, choose a random mix length, weighted towards the maximum
    if (lengthCandidate == -1)
    {
        DBG("RANDOM LENGTH");
        // Get a Gaussian random sample between 0-1, weighted towards 1
        double multiplier = juce::jlimit(0.0, 1.0, randomGenerator.getGaussian(0.2, 0.5, 0.6));
        // Choose the length using the random multiplier
        lengthCandidate = round(float(largestMultiple4) * multiplier);
        
        // If possible, ensure the length is at least 8 bars
        if (largestMultiple4 >= 2)
            lengthCandidate = juce::jmax(lengthCandidate, 2);
    }
        
    // Set the chosen candidate as the mix end point in both tracks
    mix.leaderEnd = mix.leaderStart + (lengthCandidate * (leadingTrack->getBarLength() * 4));
    mix.followerEnd = mix.followerStart + (lengthCandidate * (nextTrack->getBarLength() * 4));
    
    jassert(mix.leaderEnd <= leadingTrack->getLengthSamples()); // Check there is enough leading track to mix

    // FINALISE ------------------------------------------------------------------------

    mixQueue.add(mix);

    // Store the data for the new track to be mixed in
    // This is so the information can be used to generate the next mix
    leadingTrack = mix.nextTrack;
    leadingTrackSegments = nextTrackSegments;
}
