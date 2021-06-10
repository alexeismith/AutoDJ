//
//  TrackChooser.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 28/04/2021.
//

#include "TrackChooser.hpp"

#include "CommonDefs.hpp"

#define NUM_CANDIDATES (5)


TrackChooser::TrackChooser(DataManager* dm, RandomGenerator* random) :
    dataManager(dm), randomGenerator(random)
{
    sorter = dataManager->getSorter();
}


void TrackChooser::initialise()
{
    AnalysisResults analysisResults = dataManager->getAnalysisResults();
    
    // BPM INITIALISATION...
    
    // Define some approximate constant to help initialisation...
    // 124BPM is about the average for house
    const int averageHouseBpm = 124;
    // House ranges from about 116 - 130BPM;
    const int houseBpmRange = 14;
    
    // Calculate a Gaussian random value in the range +-(houseBpmRange/2)
    int bpmShift = round(float(houseBpmRange/2) * randomGenerator->getGaussian(0.7, 0.5));
    
    // The initial bpm will be the average house BPM plus this shift
    currentBpm = averageHouseBpm + bpmShift;
    
    
    // GROOVE INITIALISATION...
    
    // Get the range of groove available
    float grooveRange = analysisResults.maxGroove - analysisResults.minGroove;
    // Choose a random value in this range, weighted towards its centre
    currentGroove = analysisResults.minGroove + grooveRange * randomGenerator->getGaussian(0.2, 0.5, 0.5);
    
    
    // KEY INITIALISATION...
    
    // Simply initialise to a random key (valid range is 1 to 24)
    currentKey = randomGenerator->getInt(1, 24);
    
    DBG("Init BPM: " << currentBpm << " Init Groove: " << currentGroove);
}

TrackInfo* TrackChooser::chooseTrack()
{
    juce::Array<TrackInfo*> candidates;
    TrackInfo* candidate;
    TrackInfo* result;
    
    int numCandidates = juce::jmin(NUM_CANDIDATES, dataManager->getNumTracksReady());

    if (numCandidates <= 0)
        return nullptr;
    
    updatePosition();
    
    // Fetch candidate tracks from the track sorter
    // This returns the track nearest in both BPM and groove
    for (int i = 0; i < numCandidates; i++)
    {
        candidate = sorter->removeClosestTrack(currentBpm, currentGroove);
        if (candidate == nullptr)
            break;
        candidates.add(candidate);
    }
    
    // If no tracks were returned, there are no more to play
    if (candidates.isEmpty())
        return nullptr;
    
    // Notify the data manager that a track will be queued
    dataManager->trackQueued();
    
    // If only one track was returned, choose that one
    if (candidates.size() == 1)
    {
        printChoice(candidates.getFirst());
        return candidates.getFirst();
    }
    
    // Sort the candidates by key compatibility with the current key
    KeySorter keySorter((CamelotKey)currentKey);
    candidates.sort(keySorter);
    
    // Make a random choice between the first two tracks in the sorted array
    int choice = randomGenerator->getInt(0, 1);
    result = candidates.getUnchecked(choice);
    
    // Add the other candidates back into the tree, so they are present for future choices
    // First, remove the chosen track from the candidates array
    candidates.remove(choice);
    // Add others into tree
    for (auto track : candidates)
        sorter->addTrack(track);
    
    printChoice(result);
    
    // Update the current BPM, groove and key to that of the new track
    currentBpm = result->bpm;
    currentGroove = result->groove;
    currentKey = result->key;
    
    return result;
}


void TrackChooser::printChoice(TrackInfo* track)
{
    DBG("QUEUED " << track->getFilename() << \
        " bpm: " << track->bpm << \
        " groove: " << track->groove << \
        " key: " << CamelotKey(track->key).getName());
}


void TrackChooser::updatePosition()
{
    const double randomness = 0.15;
    
    // Adjust BPM and groove acceleration, using a normal distribution with standard deviation of 0.5
    accelerationBpm += randomGenerator->getGaussian(0.5) * randomness;
    accelerationGroove += randomGenerator->getGaussian(0.5) * randomness;
    
    // Add the acceleration to the velocity
    velocityBpm += accelerationBpm;
    velocityGroove += accelerationGroove;
    
    // Limit velocity to +-1
    velocityBpm = juce::jlimit(-1.0, 1.0, velocityBpm);
    velocityGroove = juce::jlimit(-1.0, 1.0, velocityGroove);
    
    // Apply the velocity for this update
    currentBpm += velocityBpm;
    currentGroove += velocityGroove;
}
