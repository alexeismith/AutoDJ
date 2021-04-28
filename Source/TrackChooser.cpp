//
//  TrackChooser.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 28/04/2021.
//

#include "TrackChooser.hpp"

#define NUM_CANDIDATES (10)


TrackChooser::TrackChooser(TrackDataManager* dm)
{
    dataManager = dm;
    sorter = dataManager->getSorter();
}


void TrackChooser::initialise()
{
    int sizeBpm = sorter->getSortedBpm().size();
    int sizeKey = sorter->getSortedKey().size();
    
    int indexBpm = round(sizeBpm * getRandomGaussian(0.2, 0.5, 0.5));
    int indexKey = round(sizeKey * getRandomGaussian(0.2, 0.5, 0.5));
    
    indexBpm = juce::jmin(indexBpm, sizeBpm-1);
    indexKey = juce::jmin(indexKey, sizeKey-1);
    
    currentBpm = sorter->getSortedBpm().getUnchecked(indexBpm)->bpm;
    currentKey = sorter->getSortedKey().getUnchecked(indexKey)->key;
    
    for (int i = 0; i < 100; i++)
        DBG(getRandomGaussian());
}


TrackInfo TrackChooser::chooseTrackComplex()
{
    TrackInfo* result;
    
    juce::Array<double> bpms;
    juce::Array<double> keys;
    double offsetBpm;
    double offsetKey;
    
    int numCandidates = NUM_CANDIDATES;
    
    numCandidates = juce::jmin(numCandidates, dataManager->getNumTracks(true)); // TODO: limit to num UNPLAYED tracks
    
    updatePosition();
    
    for (int i = 0; i < numCandidates; i++)
    {
        offsetBpm = getRandomGaussian();
        offsetKey = getRandomGaussian();
        
        bpms.add(currentBpm + offsetBpm);
        keys.add(currentKey + offsetKey);
    }
    
    // Find UNQUEUED tracks nearest these points
    
    
    
    // Find one with the same key, or related key, or random
    
    
    dataManager->markTrackQueued(result);
    
    return *result;
}


TrackInfo TrackChooser::chooseTrackRandom()
{
    bool random = true;
    
    int randomChoice;
    TrackInfo* track = nullptr;
    
    // TODO: handle case where all tracks have been played, currrently infinite loop
    // Use separate unplayed list, which keeps updated with the normal one
    
    if (random)
    {
        do
        {
            randomChoice = rand() % dataManager->getNumTracks();
            track = &dataManager->getTracks()[randomChoice];
        } while (!track->analysed || track->queued);
//        } while (!track->analysed); TODO: remove
    }
    else
    {
        // TODO: Take into account BPM, Key, Energy
    }
    
//    DBG("QUEUED: " << track->getFilename());
    track->queued = true;
    
    return *track;
}


double TrackChooser::getRandomGaussian(double stdDev, double rangeLimit, double shift)
{
    std::normal_distribution<double> distribution(0, stdDev);
    double value = distribution(randomGenerator);
    
    if (rangeLimit > 0.0)
        value = juce::jlimit(-rangeLimit, rangeLimit, value);
    
    return value + shift;
}


void TrackChooser::updatePosition()
{
    const double momentum = 0.9;
    
    accelerationBpm = accelerationBpm*momentum + getRandomGaussian(0.5)*(1.0 - momentum);
    accelerationKey = accelerationKey*momentum + getRandomGaussian(0.5)*(1.0 - momentum);
    
    velocityBpm += accelerationBpm;
    velocityKey += accelerationKey;
    
    velocityBpm = juce::jlimit(-1.0, 1.0, velocityBpm);
    velocityKey = juce::jlimit(-1.0, 1.0, velocityKey);
    
    currentBpm += velocityBpm;
    currentKey += velocityKey;
    
    DBG("velocityBpm: " << velocityBpm << " velocityKey: " << velocityKey);
}
