//
//  GraphComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#ifndef GraphComponent_hpp
#define GraphComponent_hpp

#define GRAPH_DATA_TYPE float

#include <JuceHeader.h>

class GraphComponent : public juce::Component, public juce::Timer
{
public:
    
    GraphComponent();
    
    ~GraphComponent() {}
    
    void paint(juce::Graphics& g) override;
    
    void timerCallback() override;
    
    void mouseMove (const juce::MouseEvent&) override;
    
    static void store(GRAPH_DATA_TYPE* data, int size);
    static void store(GRAPH_DATA_TYPE* data, int size, GRAPH_DATA_TYPE xMin, GRAPH_DATA_TYPE xMax);
    
private:
    
    juce::Point<float> getPositionForValue(int index);
    void getValueForPosition(juce::Point<float> pos);
    
    inline static std::atomic<bool> ready;
    
    inline static int dataSize;
    
    inline static juce::Array<GRAPH_DATA_TYPE> dataStore;
    
    inline static GRAPH_DATA_TYPE minX = -1;
    inline static GRAPH_DATA_TYPE maxX = -1;
    inline static GRAPH_DATA_TYPE minY = -1;
    inline static GRAPH_DATA_TYPE maxY = -1;
    
    int xVal = 0;
    GRAPH_DATA_TYPE yVal = 0;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphComponent)
};

#endif /* GraphComponent_hpp */
