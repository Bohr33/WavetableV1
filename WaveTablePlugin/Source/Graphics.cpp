/*
  ==============================================================================

    Graphics.cpp
    Created: 20 Dec 2025 9:39:23pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "Graphics.h"


/*=============================================================================*/
/*------------------------WaveForm Display Class-------------------------------*/
/*=============================================================================*/

WavetableDisplay::WavetableDisplay(){};
WavetableDisplay::~WavetableDisplay() = default;

void WavetableDisplay::setTable(std::shared_ptr<const TableData> table)
{
    m_wavetable = table;
    repaint();
}

void WavetableDisplay::setColours(juce::Colour colour)
{
    backgroundColour = colour;
}

void WavetableDisplay::paint(juce::Graphics& g)
{
    g.setColour(backgroundColour);
    g.drawRect(getLocalBounds());
    g.fillAll();
    
    
    if (m_wavetable != nullptr)
        drawTable(g);
    
}

void WavetableDisplay::drawTable(juce::Graphics& g)
{
    g.setColour(juce::Colours::whitesmoke);
    int tablesize = static_cast<int>(m_wavetable->samples.size()) - 1;
    juce::Logger::writeToLog("TableSize = " + juce::String(tablesize));
    jassert(tablesize == 2048);
    
    auto pointRadius = 2;
    auto bounds = getLocalBounds();

    float width = bounds.getWidth();
    float incr = width / (float) tablesize;
    
    float current_x = 0.0;
    float current_y = 0.0;
    
    auto table = m_wavetable->samples.data();
    
    for (auto i = 0; i < tablesize; i++) {
        current_y = (1-(table[i] + 1.0)/2) * bounds.getHeight();
        g.drawEllipse(current_x, current_y, pointRadius, pointRadius, 1.0);
        current_x += incr;
    }
}




/*=============================================================================*/
/*--------------------Interpolated Display Class-------------------------------*/
/*=============================================================================*/
void InterpolatedDisplay::paint(juce::Graphics& g)
{
    g.setColour(backgroundColour);
    g.drawRect(getLocalBounds());
    g.fillAll();
    
    if (m_wavetable != nullptr && m_wavetable2 != nullptr)
        drawInterpolatedTable(g);
}

void InterpolatedDisplay::setTableTwo(std::shared_ptr<const TableData> table)
{
    m_wavetable2 = table;
    repaint();
}

void InterpolatedDisplay::setInterpolation(float value)
{
    interpVal = juce::jlimit(0.0f, 1.0f, value);
}


float InterpolatedDisplay::interpolateValue(float interpolation, float val1, float val2)
{
    
    auto interpVal = juce::jlimit(0.0f, 1.0f, interpolation);
    
    auto diff = val1 - val2;
    
    float result = val1 - interpVal * diff;
    return result;
};



void InterpolatedDisplay::drawInterpolatedTable(juce::Graphics& g)
{
    
    g.setColour(juce::Colours::whitesmoke);
    int tablesize = static_cast<int>(m_wavetable->samples.size()) - 1;
    int tablesize2 = static_cast<int>(m_wavetable2->samples.size()) - 1;
//    jassert(tablesize == 2048);
//    jassert(tablesize2 == 2048);
    
    auto pointRadius = 2;
    auto bounds = getLocalBounds();

    float width = bounds.getWidth();
    float incr = width / (float) tablesize;
    
    float current_x = 0.0;
    float current_y = 0.0;
    
    auto tableOne = m_wavetable->samples.data();
    auto tableTwo = m_wavetable2->samples.data();
    
    for (auto i = 0; i < tablesize; i++) {
        
        auto val = interpolateValue(interpVal, tableOne[i], tableTwo[i]);
        current_y = (1-(val + 1.0)/2) * bounds.getHeight();
        g.drawEllipse(current_x, current_y, pointRadius, pointRadius, 1.0);
        
        current_x += incr;
    }
    
    
}
