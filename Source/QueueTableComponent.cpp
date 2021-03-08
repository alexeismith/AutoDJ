//
//  QueueTableComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/03/2021.
//

#include "QueueTableComponent.hpp"


QueueTableComponent::QueueTableComponent()
{
    table->getHeader().setLookAndFeel(&headerAppearance);
}


void QueueTableComponent::addColumns()
{
    uint8_t columnFlags = juce::TableHeaderComponent::visible;
    
    table->getHeader().addColumn("Artist", 1, 150, 0, 0, columnFlags);
    table->getHeader().addColumn("Title", 2, 200, 0, 0, columnFlags);
    table->getHeader().addColumn("Length", 3, 100, 0, 0, columnFlags);
    table->getHeader().addColumn("BPM", 4, 100, 0, 0, columnFlags);
    table->getHeader().addColumn("Key", 5, 100, 0, 0, columnFlags);
    table->getHeader().addColumn("Energy", 6, 100, 0, 0, columnFlags);
}


void QueueHeaderLook::drawTableHeaderColumn(juce::Graphics& g, juce::TableHeaderComponent& header, const juce::String& columnName,
                                            int /*columnId*/, int width, int height, bool isMouseOver, bool isMouseDown, int columnFlags)
{
//    auto highlightColour = header.findColour (juce::TableHeaderComponent::highlightColourId);

//    if (isMouseDown)
//        g.fillAll (highlightColour);
//    else if (isMouseOver)
//        g.fillAll (highlightColour.withMultipliedAlpha (0.625f));

    juce::Rectangle<int> area (width, height);
    area.reduce (4, 0);

//    if ((columnFlags & (juce::TableHeaderComponent::sortedForwards | juce::TableHeaderComponent::sortedBackwards)) != 0)
//    {
//        juce::Path sortArrow;
//        sortArrow.addTriangle (0.0f, 0.0f,
//                               0.5f, (columnFlags & juce::TableHeaderComponent::sortedForwards) != 0 ? -0.8f : 0.8f,
//                               1.0f, 0.0f);
//
//        g.setColour (juce::Colour (0x99000000));
//        g.fillPath (sortArrow, sortArrow.getTransformToScaleToFit (area.removeFromRight (height / 2).reduced (2).toFloat(), true));
//    }

    g.setColour (header.findColour (juce::TableHeaderComponent::textColourId));
    g.setFont (juce::Font ((float) height * 0.5f, juce::Font::bold));
    g.drawFittedText (columnName, area, juce::Justification::centredLeft, 1);
}
