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
    
    QueueTableComponent(TrackEditor* editor);
    
    ~QueueTableComponent() { table.reset(); }
    
    void mouseDown(const juce::MouseEvent& e) override;
    
    void mouseDrag(const juce::MouseEvent& e) override;
    
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
