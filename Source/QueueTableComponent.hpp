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
    
    QueueTableComponent();
    
    ~QueueTableComponent() { table.reset(); }
    
    void addColumns() override;
    
protected:
    
    
    
    
private:
    
    QueueHeaderLook headerAppearance;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QueueTableComponent)
};







#endif /* QueueTableComponent_hpp */
