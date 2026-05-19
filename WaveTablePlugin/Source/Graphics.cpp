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
    int tablesize = static_cast<int>(m_table.size());
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
    g.setColour(juce::Colours::whitesmoke);
    
    size_t numFrames = m_wavetable.size();
    
    //Should include extra float range for wrapping interpolation
    float interpRange = interpVal * (numFrames - 1);
    
    int frameNum = static_cast<int>(interpRange) % (numFrames - 1);
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



//==================================================
//------------------ADSR Display--------------------
//==================================================


ADSRDisplay::ADSRDisplay(){};
ADSRDisplay::~ADSRDisplay() = default;


void ADSRDisplay::setBackgroundColour(juce::Colour newColour)
{
    backgroundColour = newColour;
    repaint();
}

void ADSRDisplay::setCurveColour(juce::Colour newColour)
{
    curveColour = newColour;
    repaint();
}


void ADSRDisplay::paint(juce::Graphics &g)
{
    g.setColour(backgroundColour);
    g.drawRect(getLocalBounds());
    g.fillAll();
    
    
    generateCurve(g);
    
}

void ADSRDisplay::setParameters(ADSRParameters params)
{
    attackTime = params.attackTime;
    decayTime = params.decayTime;
    sustainLevel = params.sustainLevel;
    relTime = params.releaseTime;
    
    m_params = params;
    
    repaint();
}


void ADSRDisplay::generateCurve(juce::Graphics& g)
{
    
    auto bounds = getLocalBounds().toFloat();
    float boundsWidth = bounds.getWidth();
    float boundsHeight = bounds.getHeight();
    
    float maxHeight = boundsHeight * lineToCeilingRatio;
    
    
    float totalTime = attackTime + decayTime + relTime;
    
    if(totalTime < 0.0001f)
        return;
    
    float attWidth = (attackTime / totalTime) * boundsWidth;
    float decWidth = (decayTime / totalTime) * boundsWidth;
    float relWidth = (relTime / totalTime) * boundsWidth;
    
    
    float x0 = 0;
    float x1 = attWidth;
    float x2 = x1 + decWidth;
    float x3 = x2 + relWidth;
    
    float y0 = boundsHeight - 0;
    float y1 = boundsHeight - maxHeight;
    float y2 = boundsHeight - (maxHeight * sustainLevel);
    float y3 = boundsHeight - 0;
    
    
    juce::Path path;
    
    path.startNewSubPath(x0, y0);
    path.lineTo(x1, y1);
    path.lineTo(x2, y2);
    path.lineTo(x3, y3);
    
    g.setColour(curveColour);
    
    g.strokePath(path, juce::PathStrokeType(lineThickness));
    
    
}


// Look and Feel Class
void MyLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h, float sliderPos, float startAngle, float endAngle, juce::Slider &)
{
    
    auto radius = (float) juce::jmin(w / 2, h / 2) - 4.0f;
    auto centerX = (float) x + (float) w * 0.5f;
    auto centerY = (float) y + (float) h * 0.5f;
    auto rx = centerX - radius;
    auto ry = centerY - radius;
    auto rw = radius * 2.0f;
    auto angle = startAngle + sliderPos * (endAngle - startAngle);
    
    auto outlineThickness = 2.0;
    
    //fill
    g.setColour (juce::Colour(0xFF021526));
    g.fillEllipse (rx, ry, rw, rw);
    // outline
    g.setColour(juce::Colour(0xFFE2E2B6));
    g.drawEllipse (rx, ry, rw, rw, outlineThickness);
    
    //Pointer
    juce::Path p;
    auto pointerLength = radius * 0.5f;
    auto pointerThickness = 4.0f;
    p.addRectangle(-pointerThickness * 0.5, -radius, pointerThickness, pointerLength);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centerX, centerY));
    
    g.fillPath(p);
}
