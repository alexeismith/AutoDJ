//
//  TrackEditor.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#ifndef TrackEditor_hpp
#define TrackEditor_hpp

#include "WaveformView.hpp"
#include "DataManager.hpp"
#include "AnalyserSegments.hpp"

/**
 Shows the waveform view of a track. In future, should also be used to correct temporal analysis data.
 */
class TrackEditor : public juce::Component
{
public:
    
    /** Constructor. */
    TrackEditor(DataManager* dm);
    
    /** Destructor. */
    ~TrackEditor() {}
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    void paint(juce::Graphics& g) override;
    
    /** Loads a new track into the editor.
     
     @param[in] track Information on the track to be loaded */
    void load(Track track);
    
private:
    
    std::unique_ptr<WaveformView> waveform; ///< Scrollable waveform view of track audio
    
    Track track; ///< Current track information
    
    DataManager* dataManager = nullptr; ///< Pointer to the app's track data manager
    
    juce::String message; ///< String to display when no track is loaded
    
    std::unique_ptr<AnalyserSegments> analyserSegments; ///< Track segmentation analyser, for debugging of the segmentation algorithm (see SHOW_SEGMENTS macro in .cpp file)
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackEditor) ///< JUCE macro to add a memory leak detector
};

#endif /* TrackEditor_hpp */
