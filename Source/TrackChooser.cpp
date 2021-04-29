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
    
    randomGenerator.seed((unsigned int)juce::Time::currentTimeMillis());
}


void TrackChooser::initialise()
{
    int sizeBpm = sorter->getSortedBpm().size();
    int sizeKey = sorter->getSortedKey().size();
    
    int indexBpm = round(float(sizeBpm) * getRandomGaussian(0.2, 0.5, 0.5));
    int indexKey = round(float(sizeKey) * getRandomGaussian(0.2, 0.5, 0.5));
    
    indexBpm = juce::jmin(indexBpm, sizeBpm-1);
    indexKey = juce::jmin(indexKey, sizeKey-1);
    
    currentBpm = sorter->getSortedBpm().getUnchecked(indexBpm)->bpm;
    currentKey = sorter->getSortedKey().getUnchecked(indexKey)->key;
}


TrackInfo* TrackChooser::chooseTrack()
{
    juce::Array<TrackInfo*> candidates;
    TrackInfo* result;
    
    int numCandidates = juce::jmin(NUM_CANDIDATES, dataManager->getNumTracksReady());
    
    if (numCandidates <= 0)
        return nullptr;
    
    updatePosition();
    
    for (int i = 0; i < numCandidates; i++)
        candidates.add(sorter->findClosestAndRemove(currentBpm, currentKey));
    
    // Find one with the same key, or related key, or random
    // Either sort array or iterate through it and remove the result
    
    result = candidates.getUnchecked(0);
    
    DBG("QUEUED " << result->getFilename() << " bpm: " << result->bpm << " key: " << result->key);
    
    currentBpm = result->bpm;
    currentKey = result->key;
    
    // Add the other candidates back into the tree, so they are present for future choices
    for (int i = 1; i < candidates.size(); i++)
    {
        sorter->addAnalysed(candidates.getUnchecked(i));
    }
    
    dataManager->trackQueued();
    
    return result;
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
    
//    DBG("velocityBpm: " << velocityBpm << " velocityKey: " << velocityKey);
}
