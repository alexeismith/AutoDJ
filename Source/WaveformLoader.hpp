//
//  WaveformLoader.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#ifndef WaveformLoader_hpp
#define WaveformLoader_hpp

#include "WaveformScrollBar.hpp"
#include "DataManager.hpp"


/**
 Thread for loading data to display in track waveforms.
 
 Waveform data is provided in arrays which determine the colour and level of each waveform frame.
 A waveform frame is a single position along the horizontal axis which corresponds to a small window of audio data.
 The size of the window is determined by WAVEFORM_FRAME_SIZE in WaveformComponent.hpp.
 */
class WaveformLoader : juce::Thread
{
public:
    
    /** Constructor. */
    WaveformLoader(DataManager* dm, WaveformComponent* waveform, WaveformScrollBar* scrollBar, bool hideWhenEmpty);
    
    /** Destructor. */
    ~WaveformLoader() { stopThread(3000); }
    
    /** Triggers the loading of a new track into the waveforms, on a separate thread.
    
    @param[in] track Pointer to the track information
    @param[in] force If the supplied track is already loaded, this indicates whether it should be re-loaded */
    void load(Track* track, bool force);
    
    /** Starts the thread, which exits after loading is complete. */
    void run();
    
private:
    
    /** Loads the waveform data.
     This function checks for a new request at various points,
     and aborts if there is a new track to load. */
    void process();
    
    /** Loads the data for a single waveform frame.
     @param[in] index Index of the waveform frame to be loaded */
    void processFrame(int index);
    
    /** Resets the state of the loader, ready for a new track. */
    void reset();
    
    juce::CriticalSection lock; ///< RAII lock to ensure thread-safety while acessing data within this class
    
    DataManager* dataManager = nullptr; ///< Pointer to the app's track data manager
    
    WaveformComponent* waveform = nullptr; ///< Pointer to large waveform to receive the data loaded here
    WaveformScrollBar* scrollBar = nullptr; ///< Pointer to thin waveform to receive the data loaded here
    
    Track track; ///< Information on track currently being loaded
    Track newTrack; ///< Information on a newly requested track to load
    
    bool newRequest = false; ///< Indicates whether there is a new track to load
    
    bool loadedAudio = false; ///< Indicates whether this thread loaded the audio data of the current track (If so, it must be released after load)
    
    bool hideWhenEmpty; ///< Indicates whether the waveforms should be hidden when there is no track loaded
    
    juce::AudioBuffer<float> processBuffers; ///< Buffers to hold the track audio data while it is processed (i.e. filtered)
    
    juce::Array<juce::Colour> colours; ///< Array of colours for each waveform frame, determined by the frequency content in that frame
    juce::Array<float> levels; ///< Array of levels for each waveform frame, representing the magnitude of audio in that frame
    
    juce::IIRFilter filterLow; ///< IIR filter for low-passing track audio, which will determine the amount of red in each waveform frame
    juce::IIRFilter filterMid; ///< IIR filter for band-passing track audio, which will determine the amount of green in each waveform frame
    juce::IIRFilter filterHigh; ///< IIR filter for high-passing track audio, which will determine the amount of blue in each waveform frame
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformLoader) ///< JUCE macro to add a memory leak detector
};

#endif /* WaveformLoader_hpp */
