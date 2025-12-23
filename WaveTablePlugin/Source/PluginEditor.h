/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Graphics.h"

//==============================================================================
/**
*/
class WaveTablePluginAudioProcessorEditor  : public juce::AudioProcessorEditor,
private juce::MidiKeyboardState::Listener
{
public:
    WaveTablePluginAudioProcessorEditor (WaveTablePluginAudioProcessor&);
    ~WaveTablePluginAudioProcessorEditor() override;
    
    //MidiKeyboard Listener Virtual Callbacks
    void handleNoteOn(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)override;
    void handleNoteOff(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void selectNewTable(int itemId);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WaveTablePluginAudioProcessor& audioProcessor;
    juce::MidiKeyboardComponent    keyboardComponent;
    juce::MidiKeyboardState        keyboardState;
    
    juce::Slider    s_interpolation;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> interpolationAttachment;
    

    InterpolatedDisplay m_interpDisplay;
    
    WavetableDisplay m_displayOne;
    juce::ComboBox  waveBankOne;

    WavetableDisplay m_displayTwo;
    juce::ComboBox  waveBankTwo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveTablePluginAudioProcessorEditor)
};
