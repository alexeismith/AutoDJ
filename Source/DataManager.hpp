//
//  TrackDataManager.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#ifndef DataManager_hpp
#define DataManager_hpp

#include <JuceHeader.h>
#include "SqlDatabase.hpp"
#include "TrackSorter.hpp"
#include "AnalysisTest.hpp"
#include "DirectionView.hpp"

class FileParserThread;


/**
 Controls the flow of track data throughout the application.
 Uses an SQL database for persistent storage of track data
 @see SqlDatabase
 */
class DataManager
{
public:
      
    /** Constructor. */
    DataManager();
      
    /** Destructor. */
    ~DataManager();
    
    /** Initialises the data manager and starts a FileParserThread to check
     whether the provided directory contains enough valid music files.
     
     @param[in] directory Chosen music folder
     @param[in] directionView Pointer to direction view
     
     @return False if database initialisation fails */
    bool initialise(juce::File directory, DirectionView* directionView);
    
    /** Fetches the track info array.
     
     @return Pointer array of track data */
    TrackInfo* getTracks() { return tracks; }
    
    /** Fetches the number of tracks in the library.
     
     @return Number of tracks in the track data array */
    int getNumTracks() { return numTracks; }
    
    /** Fetches the number of tracks ready to play
     
     @return Number of tracks that are both analysed and not yet queued */
    int getNumTracksReady() { return numTracksAnalysedUnqueued; }
    
    /** Stores updated track information in the database
     
     @param[in] track Pointer to newly analysed track */
    void storeAnalysis(TrackInfo* track);
    
    /** Notifies that a track has been queued, so the number of tracks ready to play can be decremented. */
    void trackQueued() { numTracksAnalysedUnqueued -= 1; }
    
    /** Checks the state of the directory parsing process.
     
     @param[out] progress Fractional progress value (0.0 to 1.0)
     
     @return True if loading has completed (progress variable is not a safe indicator of this) */
    bool isLoading(double& progress);
    
    /** Checks whether the loaded directory had enough valid tracks.
     
     @return Result of the check */
    bool isDirectoryValid() { return validDirectory.load(); }
    
    /** Checks whether analysis has finished and fetches a progress update.
     
     @param[out] progress Fractional progress of analysis (0.0 to 1.0)
     
     @return True if analysis is fully complete (progress variable is not a safe indicator of this) */
    bool isAnalysisFinished(double& progress);
    
    /** Checks whether analysis if finished, without returning a progress update.
     
     @return True if analysis is fully complete */
    bool isAnalysisFinished();
    
    /** Checks whether the DJ mix can begin, which requires a valid music directory loaded, and enough tracks analysed.
     
     @return Result of the check */
    bool canStartPlaying();
    
    /** Loads the audio data for a given file, optionally converting stereo to mono.
     
     @param[in] filename Name of the audio file to load
     @param[in] mono Indicates desired channel configuration
     
     @return Pointer to the buffer of loaded audio (which will be owned by DataManager during its whole lifetime) */
    juce::AudioBuffer<float>* loadAudio(juce::String filename, bool mono = false);
    
    /** Unloads audio data from memory.
     
     @param[in] buffer Pointer to the audio buffer to release */
    void releaseAudio(juce::AudioBuffer<float>* buffer) { const juce::ScopedLock sl(lock); audioBuffers.removeObject(buffer); }
    
    /** Fetches the track sorter (quadtree).
     
     @return Pointer to TrackSorter instance */
    TrackSorter* getSorter() { return &sorter; }
    
    /** Fetches the overall audio analysis results.
     
     @return Overall results of music library analysis. */
    AnalysisResults getAnalysisResults() { return analysisManager->getResults(); }
    
    /** Fetches the analysis manager
     
     @return Pointer to the analysis manager */
    AnalysisManager* getAnalysisManager() { return analysisManager.get(); }
    
    /** Resets the playing/played flags for all tracks, ready for a new DJ performance. */
    void clearHistory();
    
    std::atomic<bool> trackDataUpdate = false; ///< Thread-safe flag to indicate that track data has changed
      
private:
    
    /** Adjusts the channels of the provided audio buffer to meet to given specification.
     
     @param[in,out] buffer Pointer to audio buffer
     @param[in] mono Determines whether the buffer should be changed to stereo or mono */
    void adjustChannels(juce::AudioBuffer<float>* buffer, bool mono);
    
    /** Prints the information for a given track to the debug console.
     
     @param[in] data Track data to print */
    void printTrackInfo(TrackInfo data);
    
    /** Parses the provided audio file.
     If it is valid, the track data is checked against the SQL database,
     and added to the track data array.
     Note: this function is only called by FileParserThread.
     
     @param[in] file Audio file to parse */
    void parseFile(juce::File file);
    
    /** Fetches metadata from the provided audio file,
     and determines whether it is valid for use in AutoDJ.
     
     @param[in] file Audio file to check
     @param[out] trackInfo Output location for track metadata
     
     @return True if the audio file is valid */
    bool getTrackInfo(juce::File file, TrackInfo& trackInfo);
    
    /** Generates the hash for a given audio file, using the xxHash32 algorithm.
     This hash can be used to uniquely identify the file.
     (xxHash32 doesn't guarantee unique hash for EVERY possible file, but highly unlikely to run into problems here)
     
     @param[in] file Audio file to hash
     
     @return Hash of file*/
    int getHash(juce::File file);
    
    /** Resets the data manager ready to open a new music directory. */
    void reset();
    
    juce::OwnedArray<juce::AudioBuffer<float>> audioBuffers; ///< Array of audio buffer loaded by loadAudio()
    
    std::unique_ptr<AnalysisManager> analysisManager; ///< Analysis manager
    
    juce::AudioFormatManager formatManager; ///< Audio file format handler
    juce::WildcardFileFilter fileFilter; ///< File filter, used for finding releveant files in the chosen directory
    
    SqlDatabase database; ///< SQL database for persistent storage of track data
    
    TrackSorter sorter; ///< Track sorter which uses a quadtree representation to sort tracks in 2D
    
    TrackInfo* tracks; ///< The primary array of track data
    
    int numTracks; ///< Number of tracks in the track data array
    int numTracksAnalysed; ///< Number of analysed tracks in the array
    int numTracksAnalysedUnqueued; ///< Number of analysed, unqueued tracks in the array (i.e. number of track left to choose from)
    
    std::unique_ptr<juce::DirectoryContentsList> dirContents; ///< Directory scanner, which discovers files in the chosen directory
    juce::TimeSliceThread thread {"BackgroundUpdateThread"}; ///< Background thread which dirContents uses to update itself
    
    juce::CriticalSection lock; ///< RAII lock to ensure thread-safety while acessing data within this class
    
    std::unique_ptr<FileParserThread> parser; ///< Thread for parsing audio files in the chosen directory
    friend class FileParserThread; ///< Gives FileParserThread access to private members (functions and variables) in this class
    
    std::atomic<bool> initialised = false; ///< Thread-safe flag to indicate whether the data manager has been initialised
    
    std::atomic<bool> validDirectory = false; ///< Thread-safe variable to indicates whether the chosen music folder is valid
    
    DirectionView* directionView; ///< Pointer to the Direction view, so it can be refreshed when track data changes
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataManager) ///< JUCE macro to add a memory leak detector
};


/**
 Dedicated thread for parsing audio files in the chosen music folder.
 Stores a pointer to the DataManager instance, which it uses to parse the files
 */
class FileParserThread : public juce::Thread
{
public:
    
    /** Constructor. */
    FileParserThread(DataManager* dm) : juce::Thread("FileParser"), dataManager(dm) {}
    
    /** Destructor. */
    ~FileParserThread() { stopThread(10000); }
    
    /** Thread running loop, which exits once all audio files have been parsed. */
    void run();
    
    /** Fetches the file parsing progress.
     
     @return Fractional progress of file parsing (0.0 to 1.0) */
    double getProgress() { return progress.load(); }
    
private:
    
    DataManager* dataManager = nullptr; ///< Pointer to the track data manager
    std::atomic<double> progress = 0.0; ///< Thrad-safe progress tracker (0.0 to 1.0)
    
};

#endif /* TrackDataManager_hpp */
