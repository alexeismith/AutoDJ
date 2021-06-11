//
//  WaveformLoader.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#include "WaveformLoader.hpp"

#include "CommonDefs.hpp"


WaveformLoader::WaveformLoader(DataManager* dm, WaveformComponent* wave, WaveformScrollBar* bar, bool hide) :
    juce::Thread("WaveformLoader"), dataManager(dm), waveform(wave), scrollBar(bar), hideWhenEmpty(hide)
{
    // Generate coefficients for the low-, band- and high-pass IIR filters
    filterLow.setCoefficients(juce::IIRCoefficients::makeLowPass(SUPPORTED_SAMPLERATE, 200, 1.0));
    filterMid.setCoefficients(juce::IIRCoefficients::makeBandPass(SUPPORTED_SAMPLERATE, 500, 1.0));
    filterHigh.setCoefficients(juce::IIRCoefficients::makeHighPass(SUPPORTED_SAMPLERATE, 10000, 1.0));
    
    reset();
}


void WaveformLoader::load(Track* t, bool force)
{
    const juce::ScopedLock sl(lock);
    
    if (newTrack.info != nullptr && !force)
        if (t->info->hash == newTrack.info->hash)
            return;
    
    newTrack = *t;
    
    newRequest = true;
    
    startThread();
}


void WaveformLoader::run()
{
    // Loop the load as long as there is a new load request
    while (newRequest)
    {
        // Set the request flag as noticed
        newRequest = false;
        
        // Reset ready for a new audio to be analysed
        reset();
        
        // Analyse the audio to create the waveform magnitude and colours
        process();
        
        // If audio was loaded, release it
        if (loadedAudio)
        {
            dataManager->releaseAudio(track.audio);
            loadedAudio = false;
        }
    }
    
    // Make an asynchronous call to the JUCE message/UI thread to update the waveforms
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
    
    // Fetch the new track
    track = newTrack;
    
    // If there is a new load request, abort this one
    if (newRequest) return;
    
    // If there is no audio already, load the mono track audio
    if (track.audio == nullptr)
    {
        jassert(dataManager != nullptr);
        track.audio = dataManager->loadAudio(track.info->getFilename(), true);
        loadedAudio = true;
    }
    
    // If there is a new load request, abort this one
    if (newRequest) return;
    
    // Frames of audio are analysed to produce the waveform
    // Calculate how many frames there will be - determined by track length and frame size
    numSamples = track.audio->getNumSamples();
    numFrames = numSamples / WAVEFORM_FRAME_SIZE;
    
    // Adjust the audio processing buffers to accomodate the audio
    processBuffers.setSize(4, numSamples);
    
    // Copy frame data into processing buffers
    memcpy(processBuffers.getWritePointer(0), track.audio->getReadPointer(0), numSamples * sizeof(float));
    memcpy(processBuffers.getWritePointer(1), track.audio->getReadPointer(0), numSamples * sizeof(float));
    memcpy(processBuffers.getWritePointer(2), track.audio->getReadPointer(0), numSamples * sizeof(float));
    memcpy(processBuffers.getWritePointer(3), track.audio->getReadPointer(0), numSamples * sizeof(float));
    
    // Apply low-, band- and high-pass filters to buffers 1-3
    filterLow.processSamples(processBuffers.getWritePointer(1), numSamples);
    filterMid.processSamples(processBuffers.getWritePointer(2), numSamples);
    filterHigh.processSamples(processBuffers.getWritePointer(3), numSamples);
    
    // Convert to absolute values
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(0), processBuffers.getReadPointer(0), numSamples);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(1), processBuffers.getReadPointer(1), numSamples);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(2), processBuffers.getReadPointer(2), numSamples);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(3), processBuffers.getReadPointer(3), numSamples);
    
    // Generate every waveform frame
    for (int i = 0; i < numFrames; i++)
        processFrame(i);
    
    // If there is a new load request, abort this one
    if (newRequest) return;
    
    // Pass the generated frames to the waveforms
    waveform->load(&track, &colours, &levels);
    scrollBar->load(&track, &colours, &levels);
}


void WaveformLoader::processFrame(int index)
{
    int startSample = index * WAVEFORM_FRAME_SIZE;
    
    // The magnitude of the waveform frame is the maximum value of the unfiltered audio
    levels.add(juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(0, startSample), WAVEFORM_FRAME_SIZE));
    
    // The magnitudes of the colours red, green and blue are determined by
    // the low-, band- and high-pass filtered audio respectively
    float low = juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(1, startSample), WAVEFORM_FRAME_SIZE);
    float mid = juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(2, startSample), WAVEFORM_FRAME_SIZE);
    float high = juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(3, startSample), WAVEFORM_FRAME_SIZE);
    
    // Scale the values to an appropriate range
    float multiplier = 2.f;
    low *= multiplier;
    mid *= multiplier;
    high *= multiplier;
    
    // Brighten them slightly
    low += 0.1f;
    mid += 0.1f;
    high += 0.1f;
    
    // Limit the colours to 1.0
    low = juce::jmin(low, 1.f);
    mid = juce::jmin(mid, 1.f);
    high = juce::jmin(high, 1.f);
    
    // Mix the red, green and blue components into the colour for this frame
    // and add it to the colours array
    colours.add(juce::Colour(low*255, mid*255, high*255));
}


void WaveformLoader::reset()
{
    // If the waveforms should hide when data is not loaded, make an
    // asynchronous call to the message/UI thread to set them invisible
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
