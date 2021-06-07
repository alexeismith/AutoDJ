//
//  GraphComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#ifndef GraphComponent_hpp
#define GraphComponent_hpp

#include <JuceHeader.h>

// ----------------------------------------------------------------------
// Set the data type to be given to the graph here:
#define GRAPH_DATA_TYPE float
// ----------------------------------------------------------------------

/**
 Debugging tool that displays a graph of any data array
 Instantiated by MainComponent in its own window
 
 Data access is fully static and thread-safe, so just include this file and call store() from anywhere within the codebase
 */
class GraphComponent : public juce::Component, public juce::Timer
{
public:
    
    /** Constructor: creates GraphComponent and starts its refresh timer */
    GraphComponent();
    
    /** Destructor */
    ~GraphComponent() {}
    
    /** Renders this object using the JUCE graphics handler */
    void paint(juce::Graphics& g) override;
    
    /** JUCE timer callback, used to trigger a repaint */
    void timerCallback() override;
    
    /** Mouse input handler, called when the mouse is over this object */
    void mouseMove (const juce::MouseEvent&) override;
    
    /** Gives the graph a data array to display
     
     @param[in] data    Array of data to display, must be the GRAPH_DATA_TYPE defined at the top of this file
     @param[in] size    Number of elements in the data array
     */
    static void store(const GRAPH_DATA_TYPE* data, int size);
    
    /** Gives the graph a title
     
     @param[in] title   The title to display
     */
    static void setTitle(juce::String title);
    
private:
    
    /** Gets the local screen coordinate for a given element in the data array
     
     @param[in] index   Index of the data array element to be positioned
     
     @return Coordinate of data element in JUCE float point
     */
    juce::Point<float> getPositionForElement(int index);
    
    /** Gets the index of the data array element displayed at a given screen coordinate
     
     @param[in] xPos Local x-coordinate of the mouse
     @return Index of the array element displayed at pos
     */
    int getElementAtPosition(float xPos);
    
    
    inline static std::atomic<bool> ready; ///< Thread-safe flag to indicate whether the graph data is ready to display
    
    inline static juce::Array<GRAPH_DATA_TYPE> dataStore; ///< Static array to store the data to display
    
    inline static GRAPH_DATA_TYPE minY = 0; ///< Minimum value found in the graph data array
    inline static GRAPH_DATA_TYPE maxY = 0; ///< Maximum value found in the graph data array
    
    inline static juce::String title; ///< Graph title (optional, can be left empty)
    
    int mouseOverElement = -1; ///< Index of the data array element that the mouse is hovering over
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphComponent)
};

#endif /* GraphComponent_hpp */
