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


class WavetableFrameDisplay : public juce::Component
{
public:
    WavetableFrameDisplay();
    ~WavetableFrameDisplay();
    
    void setTable(const std::vector<float>& table);
    void setColours(juce::Colour colour);
    
    float interpolateValue(float interpolation, float val1, float val2);
    void setInterpolation(float value);
    
    void paint(juce::Graphics& g) override;
    void drawTable(juce::Graphics& g);
    
protected:
    std::vector<float> m_table;
    juce::Colour backgroundColour;
};


class InterpolatedDisplay : public WavetableFrameDisplay
{
public:
    void setTableTwo(const std::vector<float>& table);
    void setInterpolation(float value);
    
    float interpolateValue(float interpolation, float val1, float val2);
    void drawInterpolatedTable(juce::Graphics& g);
    
    void paint(juce::Graphics& g) override;
    
    
private:
    std::vector<float> m_table2;
    
    float interpVal;
};


class InterpolatedWavetableDisplay : public juce::Component
{
    
public:
    InterpolatedWavetableDisplay();
    ~InterpolatedWavetableDisplay();
    
    void setWavetable(const std::vector<std::vector<float>>& newWavetable);
    void setColours(juce::Colour colour);
    
    float interpolateValue(float interpolation, float val1, float val2);
    void setInterpolation(float value);
    
    void paint(juce::Graphics& g) override;
    void drawTable(juce::Graphics& g);
    
private:
    std::vector<std::vector<float>> m_wavetable;
    juce::Colour backgroundColour;
    
    float interpVal;
    
};
