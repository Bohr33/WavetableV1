/*
  ==============================================================================

    Graphics.h
    Created: 20 Dec 2025 9:39:23pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/
#include <JuceHeader.h>
#include <vector>
#pragma once


class WavetableDisplay : public juce::Component
{
public:
    WavetableDisplay();
    ~WavetableDisplay();
    void setTable(std::vector<double>* table);
    void setTableTwo(std::vector<double>* table);
    
    double interpolateValue(float interpolation, double val1, double val2);
    
    void setInterpolation(float value);
    
    
    void reDraw();
    
    void paint(juce::Graphics& g) override;
    
    void drawTable(juce::Graphics& g);
    void drawInterpolatedTable(juce::Graphics& g);
    
private:
    std::vector<double>* m_wavetable;
    std::vector<double>* m_wavetable2;
    
    float interpVal;
    
};
