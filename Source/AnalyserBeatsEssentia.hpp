//
//  AnalyserBeatsEssentia.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 11/04/2021.
//

#ifndef AnalyserBeatsEssentia_hpp
#define AnalyserBeatsEssentia_hpp

#include <JuceHeader.h>
#include "ThirdParty/qm-dsp/dsp/tempotracking/DownBeat.h"
#include <essentia.h>
#include <algorithmfactory.h>


/**
 Temporal MIR analyser that uses Essentia beat tracking and QM-DSP downbeat to extract tempo, beat phase and downbeat position.

 Beat tracking algorithm is based on: https://doi.org/10.1109/TASL.2011.2160854
 Downbeat algorithm is based on: https://ieeexplore.ieee.org/document/7071189
 Phase correction using pulse trains is based on: https://doi.org/10.1109/TASLP.2014.2348916
*/
class AnalyserBeatsEssentia
{
public:
    
    /** Constructor. */
    AnalyserBeatsEssentia(essentia::standard::AlgorithmFactory& factory);
    
    /** Destructor. */
    ~AnalyserBeatsEssentia() {}
    
    /** Analyses the provided audio data.
    
     @param[in] audio Pointer to audio data to be analysed
     @param[out] progress Variable in which to store analysis progress
     @param[out] bpm Output location for tempo result
     @param[out] beatPhase Output location for beat phase result
     @param[out] downbeat  Output location for downbeat result */
    void analyse(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int& bpm, int& beatPhase, int& downbeat);
    
private:
    
    /** Resets the analyser ready for a new track. */
    void reset();
    
    /** Performs beat tracking to extract tempo and beat phase.
    
     @param[in] audio Pointer to audio data to be analysed
     @param[out] progress Variable in which to store analysis progress
     @param[in] numFrames Number of frames to be output by the onset detection function
     @param[out] bpm Output location for tempo result
     @param[out] beatPhase Output location for beat phase result */
    void getTempo(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int& bpm, int& beatPhase);
    
    /** Determines an overall tempo and beat phase from the provided beat grid,
     by finding the values which are dominnat across the constant sections of the beat grid.
     This is called by getTempo() after beat tracking is performed.
    
     @param[in] beats Beat grid (array of beat positions)
     @param[out] bpm Output location for tempo result
     @param[out] beatPhase Output location for beat phase result */
    void processBeats(std::vector<double> beats, int bpm, int& beatPhase);
    
    /** Uses pulse train correlation to correct off-beat phase estimations.
     
     It checks the provided beat phase result against a cross-correlation of the
     input audio and a train of ideal impulses at the given tempo. If the cross-correlation
     is close to the off-beat of the phase result, the result is changed to that off-beat value.
     
     @param[in] audio Pointer to audio data to be analysed
     @param[in] bpm Tempo result, in beats-per-minute
     @param[in,out] beatPhase Beat phase result to be checked */
    void pulseTrainsPhase(juce::AudioBuffer<float>* audio, int bpm, int& beatPhase);
    
    /** Find the downbeat position in the provided audio, based on its tempo and beat phase results.
    
     @param[in] audio Pointer to audio data to be analysed
     @param[in] numFrames Number of frames which were output by the onset detection function
     @param[in] bpm Tempo result, in beats-per-minute
     @param[in] beatPhase Beat phase result, in audio samples
     @param[out] downbeat  Output location for downbeat result */
    void getDownbeat(juce::AudioBuffer<float>* audio, int bpm, int beatPhase, int& downbeat);
    
    /** Checks whether a given onset signal frame contains a beat.
     Used for constructing a beat grid based on the tempo and beat phase results.
    
     @param[in] frame Index of the onset signal frame
     @param[in] bpm Tempo result, in beats-per-minute
     @param[in] beatPhase Beat phase result, in audio samples
    
     @return True if the frame contains a beat */
    bool isBeat(int frame, int bpm, int beatPhase);
    
    std::unique_ptr<essentia::standard::Algorithm> rhythmExtractor; ///< Essentia beat tracker
    std::unique_ptr<essentia::standard::Algorithm> percivalTempo; ///< Essentia tempo estimator (unused in normal config - see BeatTests.hpp)
    std::unique_ptr<essentia::standard::Algorithm> percivalPulseTrains; ///< Pulse train correlation algorithm
    std::unique_ptr<essentia::standard::Algorithm> onsetGlobal; ///< Essentia onset detection function
    
    std::unique_ptr<DownBeat> downBeat; ///< QM-DSP downbeat detector
    
    juce::IIRFilter filter; ///< Low-pass filter (unused in normal config - see BeatTests.hpp)
    juce::AudioBuffer<float> filteredBuffer; ///< Intermediate audio buffer for filtered audio (unused in normal config - see BeatTests.hpp)
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserBeatsEssentia) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalyserBeatsEssentia_hpp */
