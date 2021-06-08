//
//  QueueTableComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/03/2021.
//

#ifndef QueueTableComponent_hpp
#define QueueTableComponent_hpp

#include "TrackTableComponent.hpp"


/**
 Custom appearance of queue table header (top part of table showing column names).
 Since the queue table is not sortable by column, this custom appearance hides the sorting arrow and mouse interaction colours.
 */
class QueueHeaderLook : public juce::LookAndFeel_V4
{
    /** Renders a single column within the table header.
     
     @param[in] g JUCE Graphics handler
     @param[in] header Header component to be rendered
     @param[in] columnName Name of the column to be rendered
     @param[in] columnId ID of the column (unused)
     @param[in] width Width of the column
     @param[in] height Height of the column
     @param[in] isMouseOver Indicates whether the mouse is over the column (unused)
     @param[in] isMouseDown Indicates whether the mouse is pressed on the column (unused)
     @param[in] columnFlags Properties of the column (unused) */
    void drawTableHeaderColumn(juce::Graphics& g, juce::TableHeaderComponent& header, const juce::String& columnName,
                               int /*columnId*/, int width, int height, bool /*isMouseOver*/, bool /*isMouseDown*/, int /*columnFlags*/);
};


/**
 Extension of TrackTableComponent, which shows a queue of upcoming tracks, which can be re-ordered by dragging.
 Currently unused.
 */
class QueueTableComponent : public TrackTableComponent
{
public:
    
    /** Constructor. */
    QueueTableComponent(TrackEditor* editor);
    
    /** Destructor. */
    ~QueueTableComponent() { table.reset(); }
    
    /** Mouse input handler, called when the mouse is pressed on this component.
     
    @param[in] e Information about the input event that occured, such as mouse position */
    void mouseDown(const juce::MouseEvent& e) override;
    
    /** Mouse input handler, called when the mouse is dragged after pressing on this component.
     
    @param[in] e Information about the input event that occured, such as mouse position */
    void mouseDrag(const juce::MouseEvent& e) override;
    
    /** Mouse input handler, called when the mouse is released after pressing on this component.
     
    @param[in] e Information about the input event that occured, such as mouse position */
    void mouseUp(const juce::MouseEvent& e) override { isDragging = false; }

    /** Sets the table columns. */
    void addColumns() override;
    
private:
    
    bool isDragging = false; ///< Flag to indiciate when a drag operation is in progress
    int rowInitialY; ///< Initial y-coordinate of the row being dragged
    
    QueueHeaderLook headerAppearance; ///< Custom appearance of the table header (top part of table showing column names)
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QueueTableComponent) ///< JUCE macro to add a memory leak detector
};

#endif /* QueueTableComponent_hpp */
