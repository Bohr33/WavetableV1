/*
  ==============================================================================

    Graphics.cpp
    Created: 20 Dec 2025 9:39:23pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "Graphics.h"

WavetableDisplay::WavetableDisplay(){};
WavetableDisplay::~WavetableDisplay() = default;

void WavetableDisplay::setTable(std::vector<double>* table)
{
    m_wavetable = table;
    repaint();
}

void WavetableDisplay::setTableTwo(std::vector<double>* table)
{
    m_wavetable2 = table;
    repaint();
}

void WavetableDisplay::reDraw()
{
    
}

void WavetableDisplay::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::red);
    g.drawRect(getLocalBounds());
    g.fillAll();
    
    
    
    if (m_wavetable != nullptr && m_wavetable2 != nullptr)
    {
//        drawTable(g);
        drawInterpolatedTable(g);
    }else
    {
        juce::Logger::writeToLog("Ay man one of those pointers is null.....");
    }
    
}

double WavetableDisplay::interpolateValue(float interpolation, double val1, double val2)
{
    
    auto interpVal = juce::jlimit(0.0f, 1.0f, interpolation);
    
    auto diff = val1 - val2;
    
    double result = val1 - interpVal * diff;
    return result;
};

void WavetableDisplay::setInterpolation(float value)
{
    interpVal = juce::jlimit(0.0f, 1.0f, value);
}


void WavetableDisplay::drawTable(juce::Graphics& g)
{
    
    g.setColour(juce::Colours::whitesmoke);
    int tablesize = static_cast<int>(m_wavetable->size()) - 1;
    juce::Logger::writeToLog("Table Size = " + juce::String(tablesize));
    jassert(tablesize == 2048);
    
    
    auto pointRadius = 2;
    
    
    auto bounds = getLocalBounds();
    
    juce::Logger::writeToLog("Bounds: X = " + juce::String(bounds.getX()) + ", Y = " + juce::String(bounds.getY()) + ", Width = " + juce::String(bounds.getWidth()) + ", Height = " + juce::String(bounds.getHeight()));
    
    
    double width = bounds.getWidth();
    double incr = width / (double) tablesize;
    
    double current_x = 0.0;
    double current_y = 0.0;
    
    auto table = m_wavetable->data();
    
    for (auto i = 0; i < tablesize; i++) {
        
        
        current_y = (1-(table[i] + 1.0)/2) * bounds.getHeight();
        g.drawEllipse(current_x, current_y, pointRadius, pointRadius, 1.0);
        
        current_x += incr;
    }
    
    
}

void WavetableDisplay::drawInterpolatedTable(juce::Graphics& g)
{
    
    g.setColour(juce::Colours::whitesmoke);
    int tablesize = static_cast<int>(m_wavetable->size()) - 1;
    int tablesize2 = static_cast<int>(m_wavetable2->size()) - 1;
    juce::Logger::writeToLog("Table Size 1 = " + juce::String(tablesize));
    juce::Logger::writeToLog("Table Size 2 = " + juce::String(tablesize2));
    jassert(tablesize == 2048);
    jassert(tablesize2 == 2048);
    
    
    
    
    auto pointRadius = 2;
    auto bounds = getLocalBounds();
    
    juce::Logger::writeToLog("Bounds: X = " + juce::String(bounds.getX()) + ", Y = " + juce::String(bounds.getY()) + ", Width = " + juce::String(bounds.getWidth()) + ", Height = " + juce::String(bounds.getHeight()));
    
    
    double width = bounds.getWidth();
    double incr = width / (double) tablesize;
    
    double current_x = 0.0;
    double current_y = 0.0;
    
    auto tableOne = m_wavetable->data();
    auto tableTwo = m_wavetable2->data();
    
    for (auto i = 0; i < tablesize; i++) {
        
        auto val = interpolateValue(interpVal, tableOne[i], tableTwo[i]);
        current_y = (1-(val + 1.0)/2) * bounds.getHeight();
        g.drawEllipse(current_x, current_y, pointRadius, pointRadius, 1.0);
        
        current_x += incr;
    }
    
    
}
