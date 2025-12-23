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
    
    
    s_interpolation.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s_interpolation.setRange(0.0, 1.0);
    s_interpolation.onValueChange = [this] {
        m_display.setInterpolation(s_interpolation.getValue());
        m_display.repaint();
    };
    
    interpolationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "interpolation", s_interpolation);
    
    addAndMakeVisible(s_interpolation);
    addAndMakeVisible(waveBank);
    
    addAndMakeVisible(m_display);
    
    waveBank.addItem("Sine", 1);
    waveBank.addItem("Sawtooth", 2);
    
//    waveBank.onChange = [this] {
//        int selectedId = waveBank.getSelectedId();
//        selectNewTable(selectedId);
//    };
    
    //Set Default Table Display
    m_display.setTable(audioProcessor.getTable(0));
    m_display.setTableTwo(audioProcessor.getTable(1));
    
    
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
    
    m_display.setBounds(dx, dy, display_width, display_height);
    
    s_interpolation.setBounds(0 + padding, slider_y, sliderWidth, sliderHeight);
    
    waveBank.setBounds(getWidth() - (combo_width + padding), padding, combo_width, combo_height);

    keyboardComponent.setBounds(0 + padding, getHeight() - (keyHeight + padding), getWidth() - 2 * padding, keyHeight);
}

void WaveTablePluginAudioProcessorEditor::selectNewTable(int itemId)
{
    juce::Logger::writeToLog("New Wavetable Selcted! YAY! OMG YES! NICE! YOU SELECTED >>> " + juce::String(itemId) + "!");
    
    //Table Selct Logic
    
    
    m_display.setTable(audioProcessor.getTable(itemId - 1));
    

}

//Midi Keyboard Note Callback Functions
void WaveTablePluginAudioProcessorEditor::handleNoteOn(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    
}

void WaveTablePluginAudioProcessorEditor::handleNoteOff(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    
}
