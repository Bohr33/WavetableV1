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

void WavetableDisplay::reDraw()
{
    
}

void WavetableDisplay::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::red);
    g.drawRect(getLocalBounds());
    
    if(m_wavetable != nullptr)
        drawTable(g);
    
}


void WavetableDisplay::drawTable(juce::Graphics& g)
{
    
    g.setColour(juce::Colours::whitesmoke);
    int tablesize = (int) m_wavetable->size() - 1;
    juce::Logger::writeToLog("Table Size = " + juce::String(tablesize));
    jassert(tablesize == 2048);
    
    
    auto pointRadius = 5;
    
    
    auto bounds = getLocalBounds();
    
    juce::Logger::writeToLog("Bounds: X = " + juce::String(bounds.getX()) + ", Y = " + juce::String(bounds.getY()) + ", Width = " + juce::String(bounds.getWidth()) + ", Height = " + juce::String(bounds.getHeight()));
    
    
    double width = bounds.getWidth();
    double incr = width / (double) tablesize;
    
    double current_x = 0.0;
    double current_y = 0.0;
    
    g.drawLine(0.0, 0.0, bounds.getWidth(), bounds.getHeight());
    
    
    
    for (auto i = 0; i < tablesize; i++) {
        
        auto table = m_wavetable->data();
        current_y = (table[i] + 1.0)/2 * bounds.getHeight();
        g.drawEllipse(current_x, current_y, pointRadius, pointRadius, 1.0);
        
        current_x += incr;
    }
    
    
}

