//
//  QueueTableComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/03/2021.
//

#ifndef QueueTableComponent_hpp
#define QueueTableComponent_hpp

#include "TrackTableComponent.hpp"

class QueueHeaderLook : public juce::LookAndFeel_V4
{
    void drawTableHeaderColumn(juce::Graphics& g, juce::TableHeaderComponent& header, const juce::String& columnName,
                               int /*columnId*/, int width, int height, bool isMouseOver, bool isMouseDown, int columnFlags);
};



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

    void addColumns() override;
    
protected:
    
    
    
    
private:
    
    bool isDragging = false;
    
    int rowInitialY;
    
    QueueHeaderLook headerAppearance;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QueueTableComponent) ///< JUCE macro to add a memory leak detector
};







#endif /* QueueTableComponent_hpp */
