//
//  WaveformLoader.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#include "WaveformLoader.hpp"

#include "CommonDefs.hpp"


WaveformLoader::WaveformLoader(TrackDataManager* dm, WaveformComponent* wave, WaveformScrollBar* bar, bool hide) :
    juce::Thread("WaveformLoader"), dataManager(dm), waveform(wave), scrollBar(bar), hideWhenEmpty(hide)
{
    filterLow.setCoefficients(juce::IIRCoefficients::makeLowPass(SUPPORTED_SAMPLERATE, 200, 1.0));
    filterMid.setCoefficients(juce::IIRCoefficients::makeBandPass(SUPPORTED_SAMPLERATE, 500, 1.0));
    filterHigh.setCoefficients(juce::IIRCoefficients::makeHighPass(SUPPORTED_SAMPLERATE, 10000, 1.0));
    
    reset();
}


void WaveformLoader::load(Track* t)
{
    newRequest.store(true);
    
    newTrack = t;
    
    startThread();
}


void WaveformLoader::run()
{
    while (newRequest.load())
    {
        newRequest.store(false);
        reset();
        process();
    }
    
    juce::MessageManager::callAsync(std::function<void()>([this]() {
        if (hideWhenEmpty)
        {
            waveform->setVisible(true);
            scrollBar->setVisible(true);
        }
        waveform->repaint();
        scrollBar->repaint();
    }));
}


void WaveformLoader::process()
{
    int numSamples, numFrames;
    
    track = newTrack;
    
    if (newRequest.load()) return;
    
    if (track->audio == nullptr)
    {
        jassert(dataManager != nullptr);
        track->audio = dataManager->loadAudio(track->info->getFilename());
    }
    
    if (newRequest.load()) return;
    
    numSamples = track->audio->getNumSamples();
    numFrames = numSamples / WAVEFORM_FRAME_SIZE;
    
    processBuffers.setSize(4, numSamples);
    
    // Copy frame data into filter buffers
    memcpy(processBuffers.getWritePointer(0), track->audio->getReadPointer(0), numSamples * sizeof(float));
    memcpy(processBuffers.getWritePointer(1), track->audio->getReadPointer(0), numSamples * sizeof(float));
    memcpy(processBuffers.getWritePointer(2), track->audio->getReadPointer(0), numSamples * sizeof(float));
    memcpy(processBuffers.getWritePointer(3), track->audio->getReadPointer(0), numSamples * sizeof(float));
    
    // Apply filters to buffers 1-3
    filterLow.processSamples(processBuffers.getWritePointer(1), numSamples);
    filterMid.processSamples(processBuffers.getWritePointer(2), numSamples);
    filterHigh.processSamples(processBuffers.getWritePointer(3), numSamples);
    
    // Convert to absolute values
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(0), processBuffers.getReadPointer(0), numSamples);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(1), processBuffers.getReadPointer(1), numSamples);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(2), processBuffers.getReadPointer(2), numSamples);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(3), processBuffers.getReadPointer(3), numSamples);
    
    for (int i = 0; i < numFrames; i++)
        pushFrame(i);
    
    if (newRequest.load()) return;
    
    waveform->load(track, &colours, &levels);
    scrollBar->load(track, &colours, &levels);
}


void WaveformLoader::pushFrame(int index)
{
    int startSample = index * WAVEFORM_FRAME_SIZE;
    
    levels.add(juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(0, startSample), WAVEFORM_FRAME_SIZE));
    
    float low = juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(1, startSample), WAVEFORM_FRAME_SIZE);
    float mid = juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(2, startSample), WAVEFORM_FRAME_SIZE);
    float high = juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(3, startSample), WAVEFORM_FRAME_SIZE);
    
    float multiplier = 2.f;
    
    low *= multiplier;
    mid *= multiplier;
    high *= multiplier;
    
    low += 0.1f;
    mid += 0.1f;
    high += 0.1f;
    
    low = juce::jmin(low, 1.f);
    mid = juce::jmin(mid, 1.f);
    high = juce::jmin(high, 1.f);
    
    colours.add(juce::Colour(low*255, mid*255, high*255));
}


void WaveformLoader::reset()
{
    if (hideWhenEmpty)
    {
        juce::MessageManager::callAsync(std::function<void()>([this]() {
            waveform->setVisible(false);
            scrollBar->setVisible(false);
        }));
    }
    
    waveform->reset();
    scrollBar->reset();
    
    filterLow.reset();
    filterMid.reset();
    filterHigh.reset();
    
    levels.clear();
    colours.clear();
}
