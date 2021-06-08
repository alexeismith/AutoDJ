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


/**
 Simple class for sorting tracks in the TrackTableComponent.
 Conforms to JUCE's element comparator template:
 https://docs.juce.com/master/tutorial_table_list_box.html#tutorial_table_list_box_sorting_data
 */
class TrackTableSorter
{
public:
    
    /** Constructor. Ternary operator converts boolean direction into int to be stored. */
    TrackTableSorter(int columnId, bool ascending)
        : columnId(columnId),
          direction(ascending ? 1 : -1) {}
    
    /** Destructor. */
    ~TrackTableSorter() {}
    
    /** Element comparator function.
     
     @param[in] first Pointer to first track to be compared
     @param[in] second Pointer to second track to be compared
     
     @return -1 if first element sorts higher, 1 if second element sorts higher, 0 if it's a draw */
    int compareElements(TrackInfo* first, TrackInfo* second);
    
private:
    
    int columnId; ///< ID of track table column to sort against
    int direction; ///< Direction of sort (1: ascending, -1: descending)
};


/**
 Table to display the music library - uses a juce::TableListBox to render the actual table UI.
 */
class TrackTableComponent : public juce::Component, public juce::TableListBoxModel
{
public:
    
    /** Constructor. */
    TrackTableComponent(TrackEditor* editor);
    
    /** Destructor. */
    ~TrackTableComponent() {}
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    /** Input handler, called when a table cell is clicked.
     
     @param[in] rowNumber Number of the cell's row
     @param[in] columnId ID of the cell's column
     @param[in] e Information about the input event that occured, such as mouse position */
    void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& e) override;
    
    /** Populates the table with track data.
     
     @param[in] tracks Array of track data, owned by DataManager
     @param[in] numTracks Number of elements in track data array */
    void populate(TrackInfo* tracks, int numTracks);
    
    /** Gets the number of rows/tracks in the table.
     
     @return Number of rows in table */
    int getNumRows() override { return tracksSorted.size(); }
    
    /** Called by the JUCE message thread to paint the background of each row.
     
     @param[in] g JUCE graphics handler
     @param[in] rowNumber Number of row to be painted
     @param[in] width Width of row (unused)
     @param[in] height Height of row (unused)
     @param[in] rowIsSelected Indiciates whether the row is currently selected */
    void paintRowBackground(juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override;
    
    /** Called by the JUCE message thread to paint the content in each table cell.
    
     @param[in] g JUCE graphics handler
     @param[in] rowNumber Row number of cell to be painted
     @param[in] columnId Column ID of cell to be painted
     @param[in] width Width of cell in pixels
     @param[in] height Height of cell in pixels
     @param[in] rowIsSelected Indiciates whether the cell's row is currently selected */
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    
    /** Tells the table which column to sort and in what direction.
     
     @param[in] columnId ID of the data column to sort tracks against
     @param[in] ascending Sorting direction */
    void sortOrderChanged(int columnId, bool ascending) override;
    
    /** Refreshes the order of elements in the table, based on the existing sort parameters. */
    void sort();
    
    /** Sets the table columns.
     This is outside of the constructor so that QueueTableComponent can override and set different columns. */
    virtual void addColumns();
    
protected:
    
    std::unique_ptr<juce::TableListBox> table; ///< JUCE UI component that displays the table
    
    juce::Array<TrackInfo*> tracksSorted; ///< Sorted array of pointers to the track information, to avoid duplicate data
    
private:
    
    /** Fetches the string to display for a given track and table column.
     
     @param[in] track Pointer to track data, containing the information to display
     @param[in] columnId ID of the table column in which the information will be displayed
     
     @return String to display in the table */
    juce::String getValueForColumn(TrackInfo* track, int columnId);
    
    std::unique_ptr<TrackTableSorter> sorter; ///< Element comparator for sorting tracks in the table
    
    TrackEditor* trackEditor = nullptr; ///< Pointer to the editor which will show the selected track's waveform
    
    int fixedColumnWidth; ///< Width of the fixed-size table columns, in pixels
    
    juce::Font font { 14.0f }; ///< Size of the font to be rendered in the table cells
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackTableComponent) ///< JUCE macro to add a memory leak detector
};


#endif /* TrackTableComponent_hpp */
