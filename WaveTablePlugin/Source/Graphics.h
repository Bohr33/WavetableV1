/*
  ==============================================================================

    Graphics.h
    Created: 20 Dec 2025 9:39:23pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once


class WavetableDisplay : public juce::Component
{
public:
    WavetableDisplay();
    ~WavetableDisplay();
//    void setTable(std::vector<double>* table);
    
    void reDraw();
    
    void paint(juce::Graphics& g) override;
    
private:
//    std::vector<double>* wavetable;
    
};
