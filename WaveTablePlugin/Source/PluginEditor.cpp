/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveTablePluginAudioProcessorEditor::WaveTablePluginAudioProcessorEditor (WaveTablePluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), keyboardComponent(p.getMidiKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 500);
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);
    
    //============Sliders==========//
    s_interpolation.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s_interpolation.setRange(0.0, 1.0);
    s_interpolation.onValueChange = [this] {
        m_interpDisplay.setInterpolation(s_interpolation.getValue());
        m_interpDisplay.repaint();
    };
    
    interpolationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "interpolation", s_interpolation);
    
    addAndMakeVisible(s_interpolation);
    
    //========Other GUI==============//
    addAndMakeVisible(waveBankOne);
    addAndMakeVisible(waveBankTwo);
    
    waveBankOne.addItem("Sine", 1);
    waveBankOne.addItem("Triangle", 2);
    waveBankOne.addItem("Sawtooth", 3);
    waveBankOne.addItem("Square", 4);
    
    waveBankTwo.addItem("Sine", 1);
    waveBankTwo.addItem("Triangle", 2);
    waveBankTwo.addItem("Sawtooth", 3);
    waveBankTwo.addItem("Square", 4);
    
    waveBankOne.onChange = [this] {
        int selectedId = waveBankOne.getSelectedId();
        selectNewWaveform(0, selectedId-1);
        juce::Logger::writeToLog("Change on wave bank one");
    };
    
    waveBankTwo.onChange = [this] {
        int selectedId = waveBankTwo.getSelectedId();
        selectNewWaveform(1, selectedId-1);
        juce::Logger::writeToLog("Change on wave bank two");
    };
    
    //========Displays=============//
    
    addAndMakeVisible(m_interpDisplay);
    addAndMakeVisible(m_displayOne);
    addAndMakeVisible(m_displayTwo);
    
    //Set Default Table Display
    
    
    m_interpDisplay.setTable(audioProcessor.getTable(0));
    m_interpDisplay.setTableTwo(audioProcessor.getTable(1));
    
    m_displayOne.setTable(audioProcessor.getTable(0));
    m_displayTwo.setTable(audioProcessor.getTable(1));
    
    m_interpDisplay.setColours(juce::Colours::rebeccapurple);
    m_displayOne.setColours(juce::Colours::gold);
    m_displayTwo.setColours(juce::Colours::peru);
}

WaveTablePluginAudioProcessorEditor::~WaveTablePluginAudioProcessorEditor()
{
    keyboardState.removeListener(this);
}

//==============================================================================
void WaveTablePluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
//    g.drawFittedText ("The Current Of Now", getLocalBounds(), juce::Justification::centred, 1);
}

void WaveTablePluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    int padding = 20;
    
    int display_width = getWidth()/2;
    int display_height = getHeight()/3;
    
    int dx = display_width - display_width/2;
    int dy = display_height/3;
    
    int keyHeight = getHeight()/4.0;
    
    
    int sliderHeight = 150;
    int sliderWidth = 150;
    int slider_y = getHeight() - (keyHeight + padding * 2 + sliderHeight);
    
    int combo_width = getWidth() - (display_width + dx + padding * 2);
    int combo_height = 50;
    
    auto mainWindowBounds = getLocalBounds();
    auto keyBedBounds = mainWindowBounds.removeFromBottom(keyHeight);
    
    auto middleBounds = mainWindowBounds.removeFromBottom(sliderHeight + padding * 2);
    
    auto quarterWidth = mainWindowBounds.getWidth()/4.0;
    
    
    
    auto leftQuarterBounds = mainWindowBounds.removeFromLeft(quarterWidth);
    auto rightQuarterBounds = mainWindowBounds.removeFromRight(quarterWidth);
    
    m_interpDisplay.setBounds(mainWindowBounds);
    
    auto quarterHalf = leftQuarterBounds.getHeight()/2;
    auto leftTopBounds = leftQuarterBounds.removeFromTop(quarterHalf);
    m_displayOne.setBounds(leftTopBounds);
    waveBankOne.setBounds(leftQuarterBounds);
    
    auto rightTopBounds = rightQuarterBounds.removeFromTop(quarterHalf);
    m_displayTwo.setBounds(rightTopBounds);
    waveBankTwo.setBounds(rightQuarterBounds);
    
    
    auto centerPoint = middleBounds.getCentre();
    
    auto sliderBounds = juce::Rectangle<int>(sliderWidth, sliderHeight).withCentre(centerPoint);
    s_interpolation.setBounds(sliderBounds);

    keyboardComponent.setBounds(0 + padding, getHeight() - (keyHeight + padding), getWidth() - 2 * padding, keyHeight);
}

void WaveTablePluginAudioProcessorEditor::selectNewWaveform(int tableId, int waveformId)
{
    //Update Preview Display and Interpoalated Display
    WavetableDisplay* display;
    
    auto table = audioProcessor.getTable(waveformId);
    
    //Set Display Tables
    if(tableId)
    {
        //Second Table
        display = &m_displayTwo;
        m_interpDisplay.setTableTwo(table);
    }
    else
    {
        display = &m_displayOne;
        m_interpDisplay.setTable(table);
    }
        
    display->setTable(table);
    display->repaint();
    
    //Set Waveform in Audio Processor/Synth Voice and alert interpolation display
    audioProcessor.setWaveform(tableId, waveformId);
    m_interpDisplay.repaint();
}

//Midi Keyboard Note Callback Functions
void WaveTablePluginAudioProcessorEditor::handleNoteOn(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    
}

void WaveTablePluginAudioProcessorEditor::handleNoteOff(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    
}
