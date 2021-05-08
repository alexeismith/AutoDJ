//
//  TrackChooser.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 28/04/2021.
//

#include "TrackChooser.hpp"

#include "CommonDefs.hpp"

#define NUM_CANDIDATES (1)


TrackChooser::TrackChooser(TrackDataManager* dm)
{
    dataManager = dm;
    sorter = dataManager->getSorter();
    
    randomGenerator.seed((unsigned int)juce::Time::currentTimeMillis());
}


void TrackChooser::initialise()
{
    // TODO: This method is prone to picking an extreme starting BPM of there are outlier tracks
    
    AnalysisResults analysisResults = dataManager->getAnalysisResults();
    
    float bpmMultiplier = analysisResults.maxBpm - analysisResults.minBpm;
    float grooveMultiplier = analysisResults.maxGroove - analysisResults.minGroove;
    
    currentBpm = analysisResults.minBpm + bpmMultiplier * getRandomGaussian(0.2, 0.5, 0.5);
    currentGroove = analysisResults.minGroove +grooveMultiplier * getRandomGaussian(0.2, 0.5, 0.5);
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
        candidates.add(sorter->findClosestAndRemove(currentBpm, currentGroove));
    
    // Find one with the same key, or related key, or random
    // Either sort array or iterate through it and remove the result
    
    result = candidates.getUnchecked(0);
    
    if (result == nullptr)
        return result;
    
    DBG("QUEUED " << result->getFilename() << " bpm: " << result->bpm << " groove: " << result->groove);
//    DBG("QUEUED bpm: " << result->bpm << " groove: " << result->groove);
    
    currentBpm = result->bpm;
    currentGroove = result->groove;
    
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
    accelerationGroove = accelerationGroove*momentum + getRandomGaussian(0.5)*(1.0 - momentum);
    
    velocityBpm += accelerationBpm;
    velocityGroove += accelerationGroove;
    
    velocityBpm = juce::jlimit(-1.0, 1.0, velocityBpm);
    velocityGroove = juce::jlimit(-1.0, 1.0, velocityGroove);
    
    currentBpm += velocityBpm;
    currentGroove += velocityGroove;
    
    // TODO: check the values are within a valid range
    
//    DBG("velocityBpm: " << velocityBpm << " velocityGroove: " << velocityGroove);
}
