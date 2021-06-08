//
//  LibraryComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#ifndef LibraryComponent_hpp
#define LibraryComponent_hpp

#include "DataManager.hpp"
#include "QueueTableComponent.hpp"
#include "TrackEditor.hpp"
#include "CommonDefs.hpp"


/**
 The Library view UI, which shows the music library in a sorted table, and a waveform view of the selected track.
 */
class LibraryView : public juce::Component
{
public:
    
    /** Constructor. */
    LibraryView(DataManager* dataManager);
    
    /** Destructor. */
    ~LibraryView() {}
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    /** Triggers the table to be re-sorted - useful when track data has changed. */
    void refreshTable() { trackTable->sort(); }
    
    /** Populates the table with tracks from the data manager. */
    void loadFiles();
    
private:
    
    std::unique_ptr<TrackTableComponent> trackTable; ///< Table component which lists the sorted music library
    
    std::unique_ptr<TrackEditor> trackEditor; ///< Track editor which shows a waveform of the selected track (Functionality will be extended in future)
    
    DataManager* dataManager = nullptr; ///< Pointer to the app's track data manager
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LibraryView) ///< JUCE macro to add a memory leak detector
};

#endif /* LibraryComponent_hpp */
