//
//  TrackTableComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/03/2021.
//

#ifndef TrackTableComponent_hpp
#define TrackTableComponent_hpp

#include <JuceHeader.h>
#include "TrackData.hpp"

class TrackTableComponent : public juce::Component, public juce::TableListBoxModel
{
public:
    
    TrackTableComponent();
    
    ~TrackTableComponent() {}
    
    void populate(juce::Array<TrackData>* trackList);
    
    int getNumRows() override { return numRows; }
    
    void paintRowBackground(juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override;
    
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    
    void resized() override;
    
    void sortOrderChanged(int newSortColumnId, bool isForwards) override;
    
private:
    
    juce::String getValueForColumn(TrackData& track, int columnId);
    
    juce::TableListBox table { {}, this };
    
    juce::Array<TrackData>* tracks = nullptr;
    
    int numRows = 0;
    
    juce::Font font { 14.0f };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackTableComponent)
};


class TrackTableSorter
{
public:
    TrackTableSorter(int columnId, bool forwards)
        : columnId(columnId),
          direction(forwards ? 1 : -1) {}
    
    int compareElements(TrackData first, TrackData second);
    
private:
    
    int columnId;
    int direction;
};

#endif /* TrackTableComponent_hpp */
