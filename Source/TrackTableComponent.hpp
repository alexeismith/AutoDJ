//
//  TrackTableComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/03/2021.
//

#ifndef TrackTableComponent_hpp
#define TrackTableComponent_hpp

#include <JuceHeader.h>
#include "TrackInfo.hpp"
#include "TrackEditor.hpp"


class TrackTableSorter
{
public:
    
    /** Constructor.
     Ternary operator converts boolean direction into int to be stored.
     */
    TrackTableSorter(int columnId, bool forwards)
        : columnId(columnId),
          direction(forwards ? 1 : -1) {}
    
    int compareElements(TrackInfo* first, TrackInfo* second);
    
private:
    
    int columnId;
    int direction;
};


class TrackTableComponent : public juce::Component, public juce::TableListBoxModel
{
public:
    
    /** Constructor. */
    TrackTableComponent(TrackEditor* editor);
    
    /** Destructor. */
    ~TrackTableComponent() {}
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& e) override;
    
    void populate(TrackInfo* tracks, int numTracks);
    
    int getNumRows() override { return tracksSorted.size(); }
    
    void paintRowBackground(juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override;
    
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    
    void sortOrderChanged(int newSortColumnId, bool isForwards) override;
    
    void sort();
    
    virtual void addColumns();
    
protected:
    
    std::unique_ptr<juce::TableListBox> table;
    
    juce::Array<TrackInfo*> tracksSorted;
    
    
private:
    
    juce::String getValueForColumn(TrackInfo* track, int columnId);
    
    std::unique_ptr<TrackTableSorter> sorter;
    
    TrackEditor* trackEditor = nullptr; ///< Pointer to the editor which will show the selected track's waveform
    
    int fixedColumnWidth;
    
    juce::Font font { 14.0f };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackTableComponent) ///< JUCE macro to add a memory leak detector
};


#endif /* TrackTableComponent_hpp */
