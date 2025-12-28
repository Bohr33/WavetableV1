/*
  ==============================================================================

    Graphics.h
    Created: 20 Dec 2025 9:39:23pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/
#include <JuceHeader.h>
#include <vector>
#include "WaveTable.h"
#pragma once


class WavetableDisplay : public juce::Component
{
public:
    WavetableDisplay();
    ~WavetableDisplay();
    
    void setTable(std::shared_ptr<const TableData> table);
    void setColours(juce::Colour colour);
    
    float interpolateValue(float interpolation, float val1, float val2);
    void setInterpolation(float value);
    
    void paint(juce::Graphics& g) override;
    void drawTable(juce::Graphics& g);
    
protected:
    std::shared_ptr<const TableData> m_wavetable;
    juce::Colour backgroundColour;
};


class InterpolatedDisplay : public WavetableDisplay
{
public:
    void setTableTwo(std::shared_ptr<const TableData> table);
    void setInterpolation(float value);
    
    float interpolateValue(float interpolation, float val1, float val2);
    void drawInterpolatedTable(juce::Graphics& g);
    
    void paint(juce::Graphics& g) override;
    
    
private:
    std::shared_ptr<const TableData> m_wavetable2;
    float interpVal;
};
