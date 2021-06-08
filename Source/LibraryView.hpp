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

class LibraryView : public juce::Component
{
public:
    
    /** Constructor. */
    LibraryView(DataManager* dataManager);
    
    /** Destructor. */
    ~LibraryView() {}
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    void refreshTable() { trackTable->sort(); }
    
    void loadFiles();
    
private:
    
    std::unique_ptr<TrackTableComponent> trackTable;
    
    std::unique_ptr<TrackEditor> trackEditor;
    
    DataManager* dataManager;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LibraryView) ///< JUCE macro to add a memory leak detector
};

#endif /* LibraryComponent_hpp */
