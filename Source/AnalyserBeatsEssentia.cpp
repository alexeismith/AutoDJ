//
//  AnalyserBeatsEssentia.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/05/2021.
//

#include "AnalyserBeatsEssentia.hpp"

#include "CommonDefs.hpp"

#include "ThirdParty/beatutils.h"

#include "percivalevaluatepulsetrains.h"

#include "PerformanceMeasure.hpp"
#include "BeatTests.hpp"

// TODO: define these elsewhere
#define MIN_TEMPO (90)
#define MAX_TEMPO (160)


#define STEP_SIZE_ONSETS (512)
#define STEP_SIZE_DOWNBEAT (4096)
#define DOWNBEAT_DECIMATION_FACTOR (16)

AnalyserBeatsEssentia::AnalyserBeatsEssentia(essentia::standard::AlgorithmFactory& factory)
{
    // Set up the audio processing algorithms based on this configuration defined in BeatTests.hpp
    
#ifdef BEATS_MULTIFEATURE
    rhythmExtractor.reset(factory.create("RhythmExtractor2013", "minTempo", MIN_TEMPO, "maxTempo", MAX_TEMPO, "method", "multifeature"));
#elif defined BEATS_DEGARA
    rhythmExtractor.reset(factory.create("RhythmExtractor2013", "minTempo", MIN_TEMPO, "maxTempo", MAX_TEMPO, "method", "degara"));
#elif defined BEATS_PERCIVAL
    percivalTempo.reset(factory.create("PercivalBpmEstimator", "minBPM", MIN_TEMPO, "maxBPM", MAX_TEMPO));
#endif

#ifdef PHASE_CORRECTION_PULSETRAIN
    onsetGlobal.reset(factory.create("OnsetDetectionGlobal", "hopSize", STEP_SIZE_ONSETS));
    percivalPulseTrains.reset(new essentia::standard::PercivalEvaluatePulseTrains());
#endif
    
    downBeat.reset(new DownBeat(SUPPORTED_SAMPLERATE, DOWNBEAT_DECIMATION_FACTOR, STEP_SIZE_DOWNBEAT));
    downBeat->setBeatsPerBar(BEATS_PER_BAR);
    
#if defined LOW_PASS_ALL
    filter.setCoefficients(juce::IIRCoefficients::makeLowPass(SUPPORTED_SAMPLERATE, 800, 1.0));
#elif defined LOW_PASS_PHASE
    filter.setCoefficients(juce::IIRCoefficients::makeBandPass(SUPPORTED_SAMPLERATE, 150, 1.0));
#elif defined LOW_PASS_DOWNBEAT
    filter.setCoefficients(juce::IIRCoefficients::makeLowPass(SUPPORTED_SAMPLERATE, 200, 1.0));
#endif
}


void AnalyserBeatsEssentia::analyse(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int& bpm, int& beatPhase, int& downbeat)
{
    reset();
    
    // If performing filtering, prepare the audio buffer in which it will take place
#if defined LOW_PASS_ALL || defined LOW_PASS_PHASE || defined LOW_PASS_DOWNBEAT
    filteredBuffer.setSize(1, audio->getNumSamples());
    // Copy input audio into filtered buffer
    filteredBuffer.copyFrom(0, 0, audio->getReadPointer(0), audio->getNumSamples());
#endif
    
    // If low-passing at the input stage, process the filtering and point 'audio' at the filtered buffer, rather than the input
#ifdef LOW_PASS_ALL
    filter.processSamples(filteredBuffer.getWritePointer(0), audio->getNumSamples());
    // Change audio pointer
    audio = &filteredBuffer;
#endif
    
    // Perform beat tracking to extract tempo and beat phase
    getTempo(audio, progress, bpm, beatPhase);
    
    progress->store(0.6);
    
    // If low-passing just before the downbeat stage, process the filtering now and point 'audio' at the filtered buffer
#ifdef LOW_PASS_DOWNBEAT
    filter.processSamples(filteredBuffer.getWritePointer(0), audio->getNumSamples());
    audio = &filteredBuffer;
#endif

    // Perform downbeat detection
    getDownbeat(audio, bpm, beatPhase, downbeat);
}


void AnalyserBeatsEssentia::reset()
{
    // Only reset the relevant processing objects because reset itself takes time,
    // which could add up over a batch of tracks
    
#if defined BEATS_MULTIFEATURE || defined BEATS_DEGARA
    rhythmExtractor->reset();
#elif defined BEATS_PERCIVAL
    percivalTempo->reset();
#endif
    
#ifdef PHASE_CORRECTION_PULSETRAIN
    onsetGlobal->reset();
    percivalPulseTrains->reset();
#endif
    
    downBeat->resetAudioBuffer();

#if defined LOW_PASS_ALL || defined LOW_PASS_PHASE || defined LOW_PASS_DOWNBEAT
    filter.reset();
#endif
    filteredBuffer.clear();
}


void AnalyserBeatsEssentia::getTempo(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int& bpm, int& beatPhase)
{
    // Tempo analysis...
    
#if defined BEATS_MULTIFEATURE || defined BEATS_DEGARA
    
    // Convert input buffer to a std::vector, ready to give to Essentia
    std::vector<float> buffer(audio->getReadPointer(0), audio->getReadPointer(0) + audio->getNumSamples());
    
    // Instantiate output variables to give to Essentia
    float bpmFloat;
    float confidence;
    std::vector<float> ticks;
    std::vector<float> estimates;
    std::vector<float> bpmIntervals;
    std::vector<double> beats;
    
    // Set the algorithm's input and outputs...
    
    rhythmExtractor->input("signal").set(buffer);

    rhythmExtractor->output("bpm").set(bpmFloat);
    rhythmExtractor->output("confidence").set(confidence);
    rhythmExtractor->output("ticks").set(ticks);
    rhythmExtractor->output("estimates").set(estimates);
    rhythmExtractor->output("bpmIntervals").set(bpmIntervals);

    // Perform beat tracking (during which, the output data is placed in the above variables)
    rhythmExtractor->compute();

    // Round the BPM estimate to an integer
    bpm = round(bpmFloat);
    
#elif defined BEATS_PERCIVAL

    // Convert input buffer to a std::vector, ready to give to Essentia
    std::vector<float> buffer(audio->getReadPointer(0), audio->getReadPointer(0) + audio->getNumSamples());
    // Instantiate output variable to give to Essentia
    float bpmFloat;

    // Set the algorithm's input and output
    percivalTempo->input("signal").set(buffer);
    percivalTempo->output("bpm").set(bpmFloat);

    // Perform beat tracking (during which, the output data is placed in the above variable)
    percivalTempo->compute();

    // Round the BPM estimate to an integer
    bpm = round(bpmFloat);
    
#else
    jassert(false); // Must define a beat tracking method!
#endif
    
    progress->store(0.3);
    
    // Beat phase analysis...
    // (Currently no phase available using percival method)
#if defined BEATS_MULTIFEATURE || defined BEATS_DEGARA
    
    // The array of ticks output by Essentia is like a beat grid, but in terms of seconds
    // We need a beat grid in terms of audio samples, so multiply each value by the sample rate
    for (auto tick : ticks)
        beats.push_back(tick*SUPPORTED_SAMPLERATE);

    processBeats(beats, bpm, beatPhase);
#endif
    
    // Phase correction using Percival pulse trains...
#ifdef PHASE_CORRECTION_PULSETRAIN
#ifdef LOW_PASS_PHASE
    filter.processSamples(filteredBuffer.getWritePointer(0), audio->getNumSamples());
    audio = &filteredBuffer;
#endif
    
    pulseTrainsPhase(audio, bpm, beatPhase);
#endif
    
    progress->store(0.5);
}


void AnalyserBeatsEssentia::processBeats(std::vector<double> beats, int bpm, int& beatPhase)
{
    // Find sections in the provided beat grid that have constant tempo
    std::vector<BeatUtils::ConstRegion> constantRegions = BeatUtils::retrieveConstRegions(beats, SUPPORTED_SAMPLERATE);

    // Declare a variable in which to store the beat phase
    // makeConstBpm() needs a double, so can't use beatPhase argument that was passed in
    double phase = 0;
    
    // Use the constant regions of the beat grid to find an overall BPM and beat phase
    bpm = BeatUtils::makeConstBpm(constantRegions, SUPPORTED_SAMPLERATE, &phase);
    // Adjust the phase (not sure what this does, but it improves the phase estimate)
    phase = BeatUtils::adjustPhase(phase, bpm, SUPPORTED_SAMPLERATE, beats);
    
    // 'Rewind' the phase to the first beat in the track...
    int beatLength = AutoDJ::getBeatPeriod(bpm);
    beatPhase = int(phase) % beatLength;
}


void AnalyserBeatsEssentia::pulseTrainsPhase(juce::AudioBuffer<float>* audio, int bpm, int& beatPhase)
{
    // Convert input buffer to a std::vector, ready to give to Essentia
    std::vector<float> buffer(audio->getReadPointer(0), audio->getReadPointer(0) + audio->getNumSamples());
    // Instantiate array for Essentia to store the onset signal in
    std::vector<float> onsetSignal;
    
    // Set the onset algorithm's input and output
    onsetGlobal->input("signal").set(buffer);
    onsetGlobal->output("onsetDetections").set(onsetSignal);
    
    // Generate onset strength signal (OSS)
    onsetGlobal->compute();
    
    // Get the beat period in terms of onset frames
    // (Number of onset frames per beat)
    std::vector<float> beatPeriodOss;
    beatPeriodOss.push_back((60 * (44100 / STEP_SIZE_ONSETS)) / bpm);
    
    // Instantiate an output variable for the pulse train phase
    float phasePulses;

    // Set the pulse train algorithm's inputs and output
    percivalPulseTrains->input("oss").set(onsetSignal);
    percivalPulseTrains->input("positions").set(beatPeriodOss); // This input is inteded to be a number of beat period (tempo) candidates, but we just pass in our final estimate
    percivalPulseTrains->output("lag").set(phasePulses);
    
    // Perform cross correlation of OSS and pulse train at various phases
    percivalPulseTrains->compute();
    
    // The output phase is the pulse train alignment that correlated most highly with the OSS
    // It is measured in OSS frames, so we must multiply by the step size
    // between these frames to get it in terms of audio samples
    phasePulses *= STEP_SIZE_ONSETS;
    
    // Now we check whether the pulse train phase is near the off-beat of our original phase estimate: 'beatPhase'
    
    // Find the number of samples per beat
    int beatPeriod = AutoDJ::getBeatPeriod(bpm);
    // Find the off-beat of the original beat phase estimate
    int offbeat = beatPhase + beatPeriod/2;
    // Ensure the phase value doesn't include any whole beats
    offbeat %= beatPeriod;
    
    // If the pulse train phase is close to the off-beat phase (within 15% of the beat period)...
    if (abs(offbeat - phasePulses) < beatPeriod*0.15)
        // Take the off-beat as the final phase estimate
        // (Pulse train phase is not precise enough to use as final estimate, it is just for correction)
        beatPhase = offbeat;
}


void AnalyserBeatsEssentia::getDownbeat(juce::AudioBuffer<float>* audio, int bpm, int beatPhase, int& downbeat)
{
    // The downbeat algorithm requires a grid of beat position, so we need to construct one
    std::vector<double> beats;
    
    // Find the number of downbeat frames that will fit into the audio length
    int numFrames = (audio->getNumSamples() - STEP_SIZE_DOWNBEAT) / STEP_SIZE_DOWNBEAT;
    
    // For each frame to be analysed by the downbeat algorithm
    for (int i = 0; i < numFrames; i++)
    {
        // If the frame contains a beat, add the frame number to the beat grid array
        if (isBeat(i, bpm, beatPhase))
        {
            beats.push_back(i);
        }
    }
    
    for (int i = 0; i < numFrames; i++)
        downBeat->pushAudioBlock(audio->getReadPointer(0, i*STEP_SIZE_DOWNBEAT));
    
    std::vector<int> downbeats;
    size_t downLength = 0;
    const float *downsampled = downBeat->getBufferedAudio(downLength);
    // Find downbeats using QM-DSP algorithm
    downBeat->findDownBeats(downsampled, downLength, beats, downbeats);
    
    downbeat = downbeats.front();

    // The following can be used to fetch the spectral difference for each beat
//    std::vector<double> beatsd;
//    downBeat->getBeatSD(beatsd);
}


bool AnalyserBeatsEssentia::isBeat(int frame, int bpm, int beatPhase)
{
    int frameStart = frame * STEP_SIZE_DOWNBEAT;
    int frameEnd = frameStart + STEP_SIZE_DOWNBEAT - 1;
    
    double beatLength = AutoDJ::getBeatPeriod(bpm);
    
    frameStart -= beatPhase;
    frameEnd -= beatPhase;
    
    if (floor(frameEnd/beatLength) - floor(frameStart/beatLength) > 0)
        return true;
    
    return false;
}
