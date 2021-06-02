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
    
    LibraryView(DataManager* dataManager);
    
    ~LibraryView() {}
    
    void resized() override;
    
    void refreshTable() { trackTable->sort(); }
    
    void loadFiles();
    
private:
    
    std::unique_ptr<TrackTableComponent> trackTable;
    
    std::unique_ptr<TrackEditor> trackEditor;
    
    DataManager* dataManager;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LibraryView)
};

#endif /* LibraryComponent_hpp */
