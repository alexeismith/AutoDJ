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
    
    void sortOrderChanged (int newSortColumnId, bool isForwards) override;
    
//    void paint (juce::Graphics& g) override { g.setColour(juce::Colours::red); g.fillAll(); }
    
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
    TrackTableSorter (const juce::String& attributeToSortBy, bool forwards)
        : attributeToSort (attributeToSortBy),
          direction (forwards ? 1 : -1)
    {}
    
    int compareElements (juce::XmlElement* first, juce::XmlElement* second) const
    {
        auto result = first->getStringAttribute (attributeToSort)
                            .compareNatural (second->getStringAttribute (attributeToSort)); // [1]

        if (result == 0)
            result = first->getStringAttribute ("ID")
                           .compareNatural (second->getStringAttribute ("ID"));             // [2]

        return direction * result;                                                          // [3]
    }
    
private:
    juce::String attributeToSort;
    int direction;
};

#endif /* TrackTableComponent_hpp */
