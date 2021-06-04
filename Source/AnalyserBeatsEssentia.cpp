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
    
#if defined LOW_PASS_ALL || defined LOW_PASS_PHASE || defined LOW_PASS_DOWNBEAT
    filteredBuffer.setSize(1, audio->getNumSamples());
    filteredBuffer.copyFrom(0, 0, audio->getReadPointer(0), audio->getNumSamples());
#endif
    
#ifdef LOW_PASS_ALL
    filter.processSamples(filteredBuffer.getWritePointer(0), audio->getNumSamples());
    audio = &filteredBuffer;
#endif
    
    getTempo(audio, progress, bpm, beatPhase);
    
#ifdef LOW_PASS_DOWNBEAT
    filter.processSamples(filteredBuffer.getWritePointer(0), audio->getNumSamples());
    audio = &filteredBuffer;
#endif
    
    progress->store(0.6);
    
    getDownbeat(audio, bpm, beatPhase, downbeat);
}


void AnalyserBeatsEssentia::reset()
{
    downBeat->resetAudioBuffer();
    
    filteredBuffer.clear();
    
#if defined BEATS_MULTIFEATURE || defined BEATS_DEGARA
    rhythmExtractor->reset();
#elif defined BEATS_PERCIVAL
    percivalTempo->reset();
#endif
    
#ifdef PHASE_CORRECTION_PULSETRAIN
    onsetGlobal->reset();
    percivalPulseTrains->reset();
#endif

#if defined LOW_PASS_ALL || defined LOW_PASS_PHASE || defined LOW_PASS_DOWNBEAT
    filter.reset();
#endif
}


void AnalyserBeatsEssentia::getTempo(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int& bpm, int& beatPhase)
{
    // Tempo analysis...
    
#if defined BEATS_MULTIFEATURE || defined BEATS_DEGARA
    
    std::vector<float> buffer(audio->getReadPointer(0), audio->getReadPointer(0) + audio->getNumSamples());
    std::vector<float> ticks;
    std::vector<float> estimates;
    std::vector<float> bpmIntervals;
    std::vector<double> beats;

    float bpmFloat;
    float confidence;
    
    rhythmExtractor->input("signal").set(buffer);

    rhythmExtractor->output("bpm").set(bpmFloat);
    rhythmExtractor->output("ticks").set(ticks);
    rhythmExtractor->output("confidence").set(confidence);
    rhythmExtractor->output("estimates").set(estimates);
    rhythmExtractor->output("bpmIntervals").set(bpmIntervals);

    rhythmExtractor->compute();

    bpm = round(bpmFloat);
    
#elif defined BEATS_PERCIVAL

    std::vector<float> buffer(audio->getReadPointer(0), audio->getReadPointer(0) + audio->getNumSamples());

    float bpmFloat;

    percivalTempo->input("signal").set(buffer);
    percivalTempo->output("bpm").set(bpmFloat);

    percivalTempo->compute();

    bpm = round(bpmFloat);
    
#else
    jassert(false); // Must define a beat tracking method!
#endif
    
    progress->store(0.3);
    
    // Beat phase analysis...
    
    for (auto tick : ticks)
        beats.push_back(tick*SUPPORTED_SAMPLERATE);

    processBeats(beats, bpm, beatPhase);
    
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
    std::vector<BeatUtils::ConstRegion> constantRegions = BeatUtils::retrieveConstRegions(beats, SUPPORTED_SAMPLERATE);

    double firstBeat = 0;
    bpm = BeatUtils::makeConstBpm(constantRegions, SUPPORTED_SAMPLERATE, &firstBeat);
    firstBeat = BeatUtils::adjustPhase(firstBeat, bpm, SUPPORTED_SAMPLERATE, beats);
    
    // 'Rewind' firstBeat to start of track
    double beatLength = AutoDJ::getBeatPeriod(bpm);
    beatPhase = firstBeat - floor(firstBeat / beatLength) * beatLength;
}


void AnalyserBeatsEssentia::pulseTrainsPhase(juce::AudioBuffer<float>* audio, int bpm, int& beatPhase)
{
    std::vector<float> buffer(audio->getReadPointer(0), audio->getReadPointer(0) + audio->getNumSamples());
    std::vector<float> onsetSignal;
    
    onsetGlobal->input("signal").set(buffer);
    onsetGlobal->output("onsetDetections").set(onsetSignal);
    
    onsetGlobal->compute();
    
    std::vector<float> bpmLag;
    bpmLag.push_back((60 * (44100 / STEP_SIZE_ONSETS)) / bpm);
    float phaseNew;

    percivalPulseTrains->input("oss").set(onsetSignal);
    percivalPulseTrains->input("positions").set(bpmLag);
    percivalPulseTrains->output("lag").set(phaseNew);
    
    percivalPulseTrains->compute();
    
    phaseNew *= STEP_SIZE_ONSETS;
    
    int beatPeriod = AutoDJ::getBeatPeriod(bpm);
    int offbeat = beatPhase + beatPeriod/2;
    offbeat %= beatPeriod;
    
    if (abs(offbeat - phaseNew) < beatPeriod*0.15)
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
