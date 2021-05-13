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


#define STEP_SIZE (512)
#define FRAME_LENGTH (STEP_SIZE*2)
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

#ifdef PHASE_PULSETRAIN
    onsetGlobal.reset(factory.create("OnsetDetectionGlobal", "hopSize", STEP_SIZE));
    percivalPulseTrains.reset(new essentia::standard::PercivalEvaluatePulseTrains());
#endif
    
    downBeat.reset(new DownBeat(SUPPORTED_SAMPLERATE, DOWNBEAT_DECIMATION_FACTOR, STEP_SIZE));
    downBeat->setBeatsPerBar(BEATS_PER_BAR);
    
#if defined LOW_PASS_ALL
    filter.setCoefficients(juce::IIRCoefficients::makeLowPass(SUPPORTED_SAMPLERATE, 800, 1.0));
#elif defined LOW_PASS_PHASE_DOWNBEAT || defined LOW_PASS_DOWNBEAT
    filter.setCoefficients(juce::IIRCoefficients::makeBandPass(SUPPORTED_SAMPLERATE, 150, 1.0));
#endif
}


void AnalyserBeatsEssentia::analyse(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int& bpm, int& beatPhase, int& downbeat)
{
    reset();
    
    int numFrames = (audio->getNumSamples() - FRAME_LENGTH) / STEP_SIZE;
    
    getTempo(audio, progress, numFrames, bpm, beatPhase);
    
#ifdef LOW_PASS_DOWNBEAT
    filter.processSamples(audio->getWritePointer(0), audio->getNumSamples());
#endif
    
    getDownbeat(audio, numFrames, bpm, beatPhase, downbeat);
}


void AnalyserBeatsEssentia::reset()
{
    downBeat->resetAudioBuffer();
    
#if defined BEATS_MULTIFEATURE || defined BEATS_DEGARA
    rhythmExtractor->reset();
#elif defined BEATS_PERCIVAL
    percivalTempo->reset();
#endif
    
#ifdef PHASE_PULSETRAIN
    onsetGlobal->reset();
    percivalPulseTrains->reset();
#endif

#if defined LOW_PASS_ALL || defined LOW_PASS_PHASE || defined LOW_PASS_DOWNBEAT
    filter.reset();
#endif
}


void AnalyserBeatsEssentia::getTempo(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int numFrames, int& bpm, int& beatPhase)
{
#ifdef LOW_PASS_ALL
        filter.processSamples(audio->getWritePointer(0), audio->getNumSamples());
#endif
    
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
    
    // Beat phase analysis...
    
#ifdef PHASE_MIXXX
    
    for (auto tick : ticks)
        beats.push_back(tick*SUPPORTED_SAMPLERATE);

    processBeats(beats, bpm, beatPhase);
    
#elif defined PHASE_PULSETRAIN
    
    for (auto tick : ticks)
        beats.push_back(tick*SUPPORTED_SAMPLERATE);

    processBeats(beats, bpm, beatPhase);
    
    pulseTrainsPhase(audio, bpm, beatPhase);
    
#else
    jassert(false); // Must define a beat phase method!
#endif
}


void AnalyserBeatsEssentia::processBeats(std::vector<double> beats, int bpm, int& beatPhase)
{
    std::vector<BeatUtils::ConstRegion> constantRegions = BeatUtils::retrieveConstRegions(beats, SUPPORTED_SAMPLERATE);

    double firstBeat = 0;
    bpm = BeatUtils::makeConstBpm(constantRegions, SUPPORTED_SAMPLERATE, &firstBeat);
    firstBeat = BeatUtils::adjustPhase(firstBeat, bpm, SUPPORTED_SAMPLERATE, beats);
    
    // 'Rewind' firstBeat to start of track
    double beatLength = 60 * SUPPORTED_SAMPLERATE / bpm;
    beatPhase = firstBeat - floor(firstBeat / beatLength) * beatLength;
}


void AnalyserBeatsEssentia::pulseTrainsPhase(juce::AudioBuffer<float>* audio, int bpm, int& beatPhase)
{
    #ifdef LOW_PASS_PHASE_DOWNBEAT
        filter.processSamples(audio->getWritePointer(0), audio->getNumSamples());
    #endif
    
    std::vector<float> buffer(audio->getReadPointer(0), audio->getReadPointer(0) + audio->getNumSamples());
    std::vector<float> onsetSignal;
    
    onsetGlobal->input("signal").set(buffer);
    onsetGlobal->output("onsetDetections").set(onsetSignal);
    
    onsetGlobal->compute();
    
    std::vector<float> bpmLag;
    bpmLag.push_back((60 * (44100 / STEP_SIZE)) / bpm);
    float phaseNew;

    percivalPulseTrains->input("oss").set(onsetSignal);
    percivalPulseTrains->input("positions").set(bpmLag);
    percivalPulseTrains->output("lag").set(phaseNew);
    
    percivalPulseTrains->compute();
    
    phaseNew *= STEP_SIZE;
    
    int beatPeriod = AutoDJ::getBeatPeriod(bpm);
    int offbeat = beatPhase + beatPeriod/2;
    offbeat %= beatPeriod;
    
    if (abs(offbeat - phaseNew) < beatPeriod*0.15)
        beatPhase = offbeat;
}


void AnalyserBeatsEssentia::getDownbeat(juce::AudioBuffer<float>* audio, int numFrames, int bpm, int beatPhase, int& downbeat)
{
    std::vector<double> beats;
    
    for (int i = 0; i < numFrames; i++)
    {
        if (isBeat(i, bpm, beatPhase))
        {
            beats.push_back(i);
        }
    }
    
    for (int i = 0; i < numFrames; i++)
        downBeat->pushAudioBlock(audio->getReadPointer(0, i*STEP_SIZE));
    
    std::vector<int> downbeats;
    size_t downLength = 0;
    const float *downsampled = downBeat->getBufferedAudio(downLength);
    downBeat->findDownBeats(downsampled, downLength, beats, downbeats);
    
    downbeat = downbeats.front();

    // The following can be used to fetch the spectral difference for each beat
//    std::vector<double> beatsd;
//    downBeat->getBeatSD(beatsd);
}


bool AnalyserBeatsEssentia::isBeat(int frame, int bpm, int beatPhase)
{
    int frameStart = frame * STEP_SIZE;
    int frameEnd = frameStart + STEP_SIZE - 1;
    
    double beatLength = round(60 * SUPPORTED_SAMPLERATE / bpm);
    
    frameStart -= beatPhase;
    frameEnd -= beatPhase;
    
    if (abs(floor(frameEnd/beatLength) - floor(frameStart/beatLength)) > 0)
        return true;
    
    return false;
}
