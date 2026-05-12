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

WavetableFrameDisplay::WavetableFrameDisplay(){};
WavetableFrameDisplay::~WavetableFrameDisplay() = default;

void WavetableFrameDisplay::setTable(const std::vector<float>& table)
{
    m_table = table;
    repaint();
}

void WavetableFrameDisplay::setColours(juce::Colour colour)
{
    backgroundColour = colour;
}

void WavetableFrameDisplay::paint(juce::Graphics& g)
{
    g.setColour(backgroundColour);
    g.drawRect(getLocalBounds());
    g.fillAll();
    
    drawTable(g);
    
}

void WavetableFrameDisplay::drawTable(juce::Graphics& g)
{
    g.setColour(juce::Colours::whitesmoke);
    int tablesize = static_cast<int>(m_table.size() - 1);
    jassert(tablesize == 2048);
    
    auto pointRadius = 2;
    auto bounds = getLocalBounds();

    float width = bounds.getWidth();
    float incr = width / (float) tablesize;
    
    float current_x = 0.0;
    float current_y = 0.0;
    
    auto table = m_table.data();
    
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
    
    drawInterpolatedTable(g);
}

void InterpolatedDisplay::setTableTwo(const std::vector<float>& table)
{
    m_table2 = table;
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
    //Get Table Size, Remove Guard Point
    int tablesize = static_cast<int>(m_table.size() - 1);
    int tablesize2 = static_cast<int>(m_table2.size() - 1);
    jassert(tablesize == tablesize2);
    
    auto pointRadius = 2;
    auto bounds = getLocalBounds();

    float width = bounds.getWidth();
    float incr = width / (float) tablesize;
    
    float current_x = 0.0;
    float current_y = 0.0;
    
    auto tableOne = m_table.data();
    auto tableTwo = m_table2.data();
    
    for (auto i = 0; i < tablesize; i++) {
        
        auto val = interpolateValue(interpVal, tableOne[i], tableTwo[i]);
        current_y = (1-(val + 1.0)/2) * bounds.getHeight();
        g.drawEllipse(current_x, current_y, pointRadius, pointRadius, 1.0);
        
        current_x += incr;
    }
    
    
}

/*=============================================================================*/
/*----------------Interpolated Wavetable Display-------------------------------*/
/*=============================================================================*/


InterpolatedWavetableDisplay::InterpolatedWavetableDisplay(){};
InterpolatedWavetableDisplay::~InterpolatedWavetableDisplay() = default;

void InterpolatedWavetableDisplay::setWavetable(const std::vector<std::vector<float>> &newWavetable)
{
    m_wavetable = newWavetable;
    repaint();
}


void InterpolatedWavetableDisplay::paint(juce::Graphics& g)
{
    g.setColour(backgroundColour);
    g.drawRect(getLocalBounds());
    g.fillAll();
    
    drawTable(g);
}

void InterpolatedWavetableDisplay::drawTable(juce::Graphics &g)
{
    g.setColour(juce::Colours::greenyellow);
    
    size_t numFrames = m_wavetable.size();
    
    
    
    //Should include extra float range for wrapping interpolation
    float interpRange = interpVal * numFrames;
    
    
    
    int frameNum = static_cast<int>(interpRange);
    int nextFrame = (frameNum + 1) % numFrames;
    
    float floatDifference = interpRange - frameNum;
    
    std::vector<float>& table1 = m_wavetable[frameNum];
    std::vector<float>& table2 = m_wavetable[nextFrame];
    
    size_t frameSize = table1.size();
    
    auto pointRadius = 2;
    auto bounds = getLocalBounds();

    float width = bounds.getWidth();
    float incr = width / (float) frameSize;
    
    float current_x = 0.0;
    float current_y = 0.0;
    
    for (auto i = 0; i < frameSize; i++) {
        
        float interpResult = interpolateValue(floatDifference, table1[i], table2[i]);
        current_y = (1-(interpResult + 1.0)/2) * bounds.getHeight();
        g.drawEllipse(current_x, current_y, pointRadius, pointRadius, 1.0);
        current_x += incr;
    }

}

float InterpolatedWavetableDisplay::interpolateValue(float interpolation, float val1, float val2)
{
    auto interpVal = juce::jlimit(0.0f, 1.0f, interpolation);
    
    auto diff = val1 - val2;
    
    float result = val1 - interpVal * diff;
    return result;
};


void InterpolatedWavetableDisplay::setInterpolation(float value)
{
    interpVal = juce::jlimit(0.0f, 1.0f, value);
}

void InterpolatedWavetableDisplay::setColours(juce::Colour colour)
{
    backgroundColour = colour;
}
