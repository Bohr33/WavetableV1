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
    setSize (400, 300);
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);
    
    addAndMakeVisible(midiInputList);
    midiInputList.setText("MIDI Input: ");
    setupExternalMIDI();
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
    g.drawFittedText ("The Current Of Now", getLocalBounds(), juce::Justification::centred, 1);
}

void WaveTablePluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    int padding = 20;
    int keyHeight = getHeight()/4.0;
    
    int comboHeight = 40;
    int comboWidth = 120;
    
    keyboardComponent.setBounds(0 + padding, getHeight() - (keyHeight + padding), getWidth() - 2 * padding, keyHeight);
    midiInputList.setBounds(0 + padding, 0 + padding, comboWidth, comboHeight);
}

//Midi Keyboard Note Callback Functions
void WaveTablePluginAudioProcessorEditor::handleNoteOn(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    
}

void WaveTablePluginAudioProcessorEditor::handleNoteOff(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    
}

void WaveTablePluginAudioProcessorEditor::setupExternalMIDI()
{
    auto midiInputs = juce::MidiInput::getAvailableDevices();
    
    midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");
    
    //Create String Array, fill with MIDI input names, and add to input list
    juce::StringArray midiInputNames;
    for (auto device : midiInputs) {
        midiInputNames.add(device.name);
    }
    midiInputList.addItemList(midiInputNames, 1);
    
    //Function to apply changes when new MIDI device is selected
    midiInputList.onChange = [this] {setMidiInput(midiInputList.getSelectedItemIndex());};
    
    
    
    
    
    
    
    
}

void WaveTablePluginAudioProcessorEditor::setMidiInput(int index)
{
    auto availableDevices = juce::MidiInput::getAvailableDevices();
    
    
}
