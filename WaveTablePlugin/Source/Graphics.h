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
    
    double interpolateValue(float interpolation, double val1, double val2);
    void setInterpolation(float value);
    
    void paint(juce::Graphics& g) override;
    void drawTable(juce::Graphics& g);
    
protected:
    std::vector<double>* m_wavetable;
};


class InterpolatedDisplay : public WavetableDisplay
{
public:
    void setTableTwo(std::vector<double>* table);
    void setInterpolation(float value);
    
    double interpolateValue(float interpolation, double val1, double val2);
    void drawInterpolatedTable(juce::Graphics& g);
    
    void paint(juce::Graphics& g) override;
    
    
private:
    std::vector<double>* m_wavetable2;
    float interpVal;
};
