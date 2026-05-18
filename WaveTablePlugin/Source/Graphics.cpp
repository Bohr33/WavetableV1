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
    auto bounds = juce::Rectangle<float>(x, y, w, h);
    auto centre = bounds.getCentre();
    auto radius  = bounds.getWidth() * 0.5f;
    auto outlineShrinkRatio = 0.85;
    

    // --- Body ---
    g.setColour(juce::Colour(0xFF03346E));
//    auto ellipseBounds =  juce::Rectangle<float>()
    auto ellipseBounds = juce::Rectangle<float>(centre.x-radius*outlineShrinkRatio, centre.y-radius*outlineShrinkRatio,w*outlineShrinkRatio, w*outlineShrinkRatio);
    
    g.fillEllipse(ellipseBounds);

    // --- Outline ---
    g.setColour(juce::Colour(0xFFE2E2B6));
    g.drawEllipse(ellipseBounds, 1);

    // --- Background arc track ---
    {
        juce::Path track;
        track.addCentredArc(centre.x, centre.y,
                            radius * 0.75f, radius * 0.75f,
                            0.0f,
                            startAngle, endAngle,
                            true);
        g.setColour(juce::Colour(0xFF1A1A2A));
        g.strokePath(track, juce::PathStrokeType(3.0f,
                     juce::PathStrokeType::curved,
                     juce::PathStrokeType::rounded));
    }

    // --- Filled arc (value) ---
    {
        juce::Path valueArc;
        float currentAngle = startAngle
                           + sliderPos * (endAngle - startAngle);

        valueArc.addCentredArc(centre.x, centre.y,
                               radius * 0.75f, radius * 0.75f,
                               0.0f,
                               startAngle, currentAngle,
                               true);
        g.setColour(juce::Colour(0xFF6B5FE0));   // your accent colour
        g.strokePath(valueArc, juce::PathStrokeType(3.0f,
                     juce::PathStrokeType::curved,
                     juce::PathStrokeType::rounded));
    }

    // --- Dial indent (small circle at current position) ---
    {
        float angle = startAngle
                    + sliderPos * (endAngle - startAngle);

        // Place it on the arc radius
        float indentRadius = radius * 0.75f;
        float indentX = centre.x + indentRadius * std::sin(angle);
        float indentY = centre.y - indentRadius * std::cos(angle);
        float dotSize = 5.0f;

        // Fill (matches background so it looks punched in)
        g.setColour(juce::Colour(0xFF1A1A2A));
        g.fillEllipse(indentX - dotSize * 0.5f,
                      indentY - dotSize * 0.5f,
                      dotSize, dotSize);

        // Rim
        g.setColour(juce::Colour(0xFF6B5FE0));
        g.drawEllipse(indentX - dotSize * 0.5f,
                      indentY - dotSize * 0.5f,
                      dotSize, dotSize, 1.2f);
    }
}
