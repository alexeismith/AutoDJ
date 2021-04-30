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
    table->setInterceptsMouseClicks(false, false);
}


void QueueTableComponent::addColumns()
{
    uint8_t columnFlags = juce::TableHeaderComponent::visible;
    
    table->getHeader().addColumn("Artist", 1, 210, 0, 0, columnFlags);
    table->getHeader().addColumn("Title", 2, 310, 0, 0, columnFlags);
    table->getHeader().addColumn("Length", 3, 80, 0, 0, columnFlags);
    table->getHeader().addColumn("BPM", 4, 60, 0, 0, columnFlags);
    table->getHeader().addColumn("Key", 5, 60, 0, 0, columnFlags);
    table->getHeader().addColumn("Groove", 6, 80, 0, 0, columnFlags);
}


void QueueTableComponent::mouseDown(const juce::MouseEvent& e)
{
    if(e.getPosition().getY() < table->getHeader().getHeight())
    {
        table->selectRow(-1);
        isDragging = false;
        return;
    }
    
    int row = table->getRowContainingPosition(e.getPosition().getX(), e.getPosition().getY());
    table->selectRow(row);
    
    if (row != -1)
    {
        rowInitialY = e.getPosition().getY();
        isDragging = true;
    }
}


void QueueTableComponent::mouseDrag(const juce::MouseEvent& e)
{
    int distance, newRow, row = table->getSelectedRow();
    if (!isDragging) return;
    
    distance = e.getPosition().getY() - rowInitialY;
    
    int rowShift = floor(distance / table->getRowHeight());
    
    newRow = row + rowShift;
    newRow = juce::jmin(newRow, getNumRows() - 1);
    newRow = juce::jmax(newRow, 0);
    rowShift = newRow - row;
    rowInitialY += rowShift * table->getRowHeight();
    
//    DBG(newRow);
    tracksSorted.move(row, newRow);
    table->selectRow(newRow);
    table->repaint();
    
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
